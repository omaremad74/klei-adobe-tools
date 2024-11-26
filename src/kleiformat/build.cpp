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
            throw std::format(STRINGS::FILE_NOOPEN, "Build::Build(const std::filesystem::path& bildpath, KLEI_FORMATS game_format)", bildpath.string());

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
        throw STRINGS::BUILD_ERRORS::INCORRECT_MAGIC;

    read_bin_data(kbild, m_Version);
    read_bin_data(kbild, m_NumSymbols);
    read_bin_data(kbild, m_NumFrames);

    uint32_t buildnamelength;
    read_bin_data(kbild, buildnamelength);
    read_bin_string(kbild, m_Name, buildnamelength);

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

            if (frame.num_vertices % 6 != 0) {
                throw std::format(STRINGS::BUILD_ERRORS::INCORRECT_VERT_COUNT, frame.num, symbol.hash_name);
            }
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
            sprite.crop(Magick::Geometry(std::floor(w * mip.width + 0.5), std::floor(h * mip.height + 0.5), std::floor(mip.width * u), std::floor(mip.height * v)));

            Magick::Geometry scaling_geo(frame.w, frame.h);
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

uint32_t Build::GetNumFrames() const {
    uint32_t total = 0;
    //
    for (const auto& symbol : m_Symbols) {
        total += symbol.frames.size();
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