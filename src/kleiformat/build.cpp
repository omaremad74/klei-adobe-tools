#include "build.hpp"

Build::Build(const std::filesystem::path& bildpath, KLEI_FORMATS game_format) {
    const bool is_zip = bildpath.extension() == ".zip";

    if (is_zip) {
        ZipStream bild_data = GetZipStreamFromFile(bildpath, "build.bin"); //TODO we're assuming the anim is `build.bin`, true for DS/T but not for all formats!

        ReadStream(bild_data.stream, game_format, bildpath);

        delete[] bild_data.buffer;
    } else {
        std::ifstream kbild(bildpath / "build.bin", std::ios::in | std::ios::binary);

        if (!kbild.is_open())
            throw std::runtime_error(std::format(STRINGS::FILE_NOOPEN, "Build::Build(const std::filesystem::path& bildpath, KLEI_FORMATS game_format)", bildpath.string()));

        ReadStream(kbild, game_format, bildpath);

        kbild.close();
    }
}

void Build::ReadStream(std::istream& kbild, KLEI_FORMATS game_format, const std::filesystem::path& bildpath) {
    m_BildType = game_format;

    /*
    switch (game_format) {
    
    }
    */

    std::string magic;
    read_bin_string(kbild, magic, 4);
    if (magic != K_MAGICS::BILD)
        throw std::runtime_error(STRINGS::BUILD_ERRORS::INCORRECT_MAGIC);

    read_bin_data(kbild, m_Version);
    read_bin_data(kbild, m_NumSymbols);
    read_bin_data(kbild, m_NumFrames);
    read_bin_kleistring(kbild, m_Name);

    read_bin_data(kbild, m_NumMaterials);
    m_Materials.resize(m_NumMaterials);

    for (KTex& material : m_Materials) {
        read_bin_kleistring(kbild, material.m_Name);
        material.ReadFile(bildpath, game_format);
    }

    m_Symbols.resize(m_NumSymbols);

    for (auto& symbol : m_Symbols) {
        read_bin_data(kbild, symbol.hash_name);
        read_bin_data(kbild, symbol.num_frames);
        symbol.frames.resize(symbol.num_frames);

        for (auto& frame : symbol.frames) {
            read_bin_data(kbild, frame.num);
            read_bin_data(kbild, frame.duration);
            read_bin_data(kbild, frame.x);
            read_bin_data(kbild, frame.y);
            read_bin_data(kbild, frame.w);
            read_bin_data(kbild, frame.h);
            read_bin_data(kbild, frame.vb_start_index);
            read_bin_data(kbild, frame.num_vertices);
            frame.vertices.resize(frame.num_vertices);

            if (frame.num_vertices % 6 != 0)
                throw std::runtime_error(std::format(STRINGS::BUILD_ERRORS::INCORRECT_VERT_COUNT, frame.num, symbol.hash_name));
        }
    }

    read_bin_data(kbild, m_NumVertices);

    for (auto& symbol : m_Symbols) {
        for (auto& frame : symbol.frames) {
            for (auto& vert : frame.vertices) {
                read_bin_data(kbild, vert.x);
                read_bin_data(kbild, vert.y);
                read_bin_data(kbild, vert.z);
                read_bin_data(kbild, vert.u);
                read_bin_data(kbild, vert.v);
                read_bin_data(kbild, vert.w);
            }
        }
    }

    read_bin_data(kbild, m_NumHashes);
    m_Hashes.resize(m_NumHashes);

    for (uint32_t& hash : m_Hashes) {
        read_bin_data(kbild, hash);
        read_bin_kleistring(kbild, m_SymbolHashesToNames[hash]);
    }
}

void Build::WriteToFile(const std::filesystem::path& bildpath) {
    /*
    switch (game_format) {
    
    }
    */

    std::ofstream kbild(bildpath, std::ios::out | std::ios::binary);

    if (!kbild.is_open())
        throw std::runtime_error(std::format(STRINGS::FILE_NOOPEN, "Build::WriteToFile", bildpath.string()));

    write_bin_kleistring(kbild, K_MAGICS::BILD);

    write_bin_data(kbild, 6); //TODO, convert klei format to anim and build version nums
    write_bin_data(kbild, static_cast<uint32_t>(GetNumSymbols()));
    write_bin_data(kbild, static_cast<uint32_t>(GetNumFrames()));
    write_bin_kleistring(kbild, m_Name);

    write_bin_data(kbild, static_cast<uint32_t>(m_Materials.size()));
    for (KTex& material : m_Materials) {
        write_bin_kleistring(kbild, material.m_Name);
        material.WriteImage(std::filesystem::path(bildpath).replace_filename(material.m_Name));
    }

    for (const auto& symbol : m_Symbols) {
        write_bin_data(kbild, symbol.hash_name);
        write_bin_data(kbild, static_cast<uint32_t>(symbol.frames.size()));

        for (const auto& frame : symbol.frames) {
            write_bin_data(kbild, frame.num);
            write_bin_data(kbild, frame.duration);
            write_bin_data(kbild, frame.x);
            write_bin_data(kbild, frame.y);
            write_bin_data(kbild, frame.w);
            write_bin_data(kbild, frame.h);
            write_bin_data(kbild, frame.vb_start_index);
            write_bin_data(kbild, static_cast<uint32_t>(frame.vertices.size()));
        }
    }

    write_bin_data(kbild, static_cast<uint32_t>(GetNumVertices()));

    for (const auto& symbol : m_Symbols) {
        for (const auto& frame : symbol.frames) {
            for (const auto& vert : frame.vertices) {
                write_bin_data(kbild, vert.x);
                write_bin_data(kbild, vert.y);
                write_bin_data(kbild, vert.z);
                write_bin_data(kbild, vert.u);
                write_bin_data(kbild, vert.v);
                write_bin_data(kbild, vert.w);
            }
        }
    }

    write_bin_data(kbild, static_cast<uint32_t>(m_SymbolHashesToNames.size()));

    for (const uint32_t hash : m_Hashes) {
        write_bin_data(kbild, hash);
        write_bin_kleistring(kbild, m_SymbolHashesToNames.at(hash));
    }

    kbild.close();
}

//Write images to path
void Build::ExportAtlas(const std::filesystem::path& out_path) const {
    for (auto& sprite : ExportAtlas()) {
        sprite.write(std::format("{}.png", (out_path / sprite.fileName()).string()));
    }
}

//Return list of images (for XFL)
std::vector<Magick::Image> Build::ExportAtlas() const {
    std::vector<Magick::Image> images;
    images.reserve(GetNumFrames());
    //
    for (const auto& symbol : m_Symbols) {
        const std::string symbol_format_name = GetSymbolName(symbol) + "-{}"; // "-{}.png"

        for (const auto& frame : symbol.frames) {
            const size_t material_index = static_cast<size_t>(frame.vertices[0].w);
            const auto [u, v, w, h] = GetFrameBBox(frame);
            const KTex::MipMap& mip = m_Materials[material_index].m_MipMaps[0];

            Magick::Image& sprite = images.emplace_back();
            sprite.read(mip.data, Magick::Geometry(mip.width, mip.height), 8, "RGBA");
            sprite.crop(Magick::Geometry(
                static_cast<size_t>(std::floor(w * mip.width + 0.5)),
                static_cast<size_t>(std::floor(h * mip.height + 0.5)),
                static_cast<ssize_t>(std::floor(u * mip.width)),
                static_cast<ssize_t>(std::floor(v * mip.height))
            ));

            Magick::Geometry scaling_geo(static_cast<size_t>(frame.w), static_cast<size_t>(frame.h));
            scaling_geo.aspect(true);

            sprite.resize(scaling_geo);
            sprite.flip();
            sprite.magick("png");
            sprite.defineValue("png", "color-type", "6"); //color-type 6 means RGBA https://www.w3.org/TR/PNG-Chunks.html
            sprite.fileName(std::vformat(symbol_format_name, std::make_format_args(frame.num)));
        }
    }
    //
    return images;
}

size_t Build::GetNumSymbols() const {
    return m_Symbols.size();
}

size_t Build::GetNumFrames() const {
    size_t total = 0;
    //
    for (const auto& symbol : m_Symbols) {
        total += symbol.frames.size();
    }
    //
    return total;
}

size_t Build::GetNumVertices() const {
    size_t total = 0;
    //
    for (const auto& symbol : m_Symbols) {
        for (const auto& frame : symbol.frames) {
            total += frame.vertices.size();
        }
    }
    //
    return total;
}

Build::BBox Build::GetFrameBBox(const frame& frame) {
    float min_u = 1.0f, min_v = 1.0f, max_u = 0.0f, max_v = 0.0f;
    //
    for (const auto& vert : frame.vertices) {
        min_u = std::min(min_u, vert.u);
        min_v = std::min(min_v, vert.v);

        max_u = std::max(max_u, vert.u);
        max_v = std::max(max_v, vert.v);
    }
    //
    return {min_u, min_v, max_u-min_u, max_v-min_v};
}

std::string Build::GetSymbolName(const symbol& s) const {
    return m_SymbolHashesToNames.at(s.hash_name);
}