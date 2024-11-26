#include "ktex.hpp"

KTex::KTex() {
    
}

KTex::KTex(const std::string& name) 
    : m_Name(name) {
    
}

KTex::KTex(const std::string& name, const Magick::Image img)
    : m_Name(name) {

}

KTex::KTex(const std::string& name, const std::filesystem::path& imagepath, KLEI_FORMATS tex_type)
    : m_Name(name) {
    this->ReadFile(imagepath, tex_type);
}

//imagepath is a folder or zip file
void KTex::ReadFile(const std::filesystem::path& imagepath, KLEI_FORMATS tex_type) {
    const bool is_zip = imagepath.extension() == ".zip";

    if (is_zip) {
        ZipStream tex_data = GetZipStreamFromFile(imagepath, m_Name); //TODO we're assuming the anim is `build.bin`, true for DS/T but not for all formats!

        ReadStream(tex_data.stream, tex_type);

        delete[] tex_data.buffer;
    } else {
        std::ifstream ktex(imagepath / m_Name, std::ios::in | std::ios::binary);

        if (!ktex.is_open())
            throw std::format(STRINGS::FILE_NOOPEN, "KTex::ReadFile(const std::filesystem::path& imagepath)", imagepath.string());

        ReadStream(ktex, tex_type);

        ktex.close();
    }
}

void KTex::ReadStream(std::istream& ktex, KLEI_FORMATS tex_type) {
    std::string magic;
    read_bin_string(ktex, magic, 4);
    if (magic != K_MAGICS::TEX)
        throw STRINGS::TEXTURE_ERRORS::INCORRECT_MAGIC;

    m_Tex_type = tex_type;

    switch(tex_type) {
        case KLEI_FORMATS::DONT_STARVE:
            DecompileDontStarveFormat(ktex);
            break;
        case KLEI_FORMATS::EETS_MUNCHIES:
            //DecompileEetsMunchiesFormat(ktex);
            break;
        case KLEI_FORMATS::HOT_LAVA:
            //DecompileHotLavaFormat(ktex);
            break;
        case KLEI_FORMATS::INVISIBLE_INC:
            //DecompileInvisibleIncFormat(ktex);
            break;
        case KLEI_FORMATS::MARK_OF_THE_NINJA:
            //DecompileMOTNFormat(ktex);
            break;
        case KLEI_FORMATS::SHANK:
            //DecompileShankFormat(ktex);
            break;
        case KLEI_FORMATS::ROTWOOD: case KLEI_FORMATS::GRIFTLANDS:
            DecompileRotwoodOrGriftlandsFormat(ktex);
            break;
    };
}

void KTex::DecompileDontStarveFormat(std::istream& ktex) {
    read_bin_data(ktex, m_Flags.data);

    m_MipMaps.resize(m_Flags.spec.mipmap_count);

    for (auto& mip : m_MipMaps) {
        read_bin_data(ktex, mip.width);
        read_bin_data(ktex, mip.height);
        read_bin_data(ktex, mip.pitch);
        read_bin_data(ktex, mip.data_size);
    }

    bool is_compressed = m_Flags.spec.compression == COMPRESSION_FORMATS::DXT1 || 
                         m_Flags.spec.compression == COMPRESSION_FORMATS::DXT3 || 
                         m_Flags.spec.compression == COMPRESSION_FORMATS::DXT5;

    for (auto& mip : m_MipMaps) {
        char* const mipmap_data = new char[mip.data_size];
        ktex.read(mipmap_data, mip.data_size);

        if (is_compressed) {
            const uint32_t NUM_PIXEL_DATA = 4 * mip.width * mip.height;
            unsigned char* rgba = new unsigned char[NUM_PIXEL_DATA];
            const int32_t squish_compression_type = GetSquishCompressionFormat(m_Flags.spec.compression);

            squish::DecompressImage(rgba, mip.width, mip.height, mipmap_data, squish_compression_type);

            mip.data.update(rgba, NUM_PIXEL_DATA);
            delete[] rgba;
        } else {
            mip.data.update(mipmap_data, mip.data_size);
        }

        delete[] mipmap_data;
    }
}

void KTex::DecompileRotwoodOrGriftlandsFormat(std::istream& ktex) {
    char magical_byte; //Idk what this byte is. i think it's pitch?
    read_bin_data(ktex, magical_byte);
    read_bin_data(ktex, m_DDS_Data.width);
    read_bin_data(ktex, m_DDS_Data.height);
    
    const int32_t length = static_cast<int32_t>(ktex.tellg());
    char* const dds_file_data = new char[length];
    ktex.read(dds_file_data, length);
    m_DDS_Data.image.read(dds_file_data);
    
    delete[] dds_file_data;
}

void KTex::WriteImage(const std::filesystem::path& outpath) {
    switch (m_Tex_type) {
        case KLEI_FORMATS::DONT_STARVE:
            WriteDontStarveFormatImage(outpath);
            break;
        case KLEI_FORMATS::EETS_MUNCHIES:
            //WriteEetsMunchiesFormatImage(outpath);
            break;
        case KLEI_FORMATS::HOT_LAVA:
            //WriteHotLavaFormatImage(outpath);
            break;
        case KLEI_FORMATS::INVISIBLE_INC:
            //WriteInvisibleIncFormatImage(outpath);
            break;
        case KLEI_FORMATS::MARK_OF_THE_NINJA:
            //WriteMOTNFormatImage(outpath);
            break;
        case KLEI_FORMATS::SHANK:
            //WriteShankFormatImage(outpath);
            break;
        case KLEI_FORMATS::ROTWOOD: case KLEI_FORMATS::GRIFTLANDS:
            WriteRotwoodOrGriftlandsFormatImage(outpath);
            break;
    }
}

void KTex::WriteDontStarveFormatImage(const std::filesystem::path& outpath) {
    const KTex::MipMap& mip = m_MipMaps[0];
    Magick::Image png;
    png.read(mip.data, Magick::Geometry(mip.width, mip.height), 8, "RGBA");
    png.flip();
    png.magick("png");
    png.defineValue("png", "color-type", "6"); // png color type 6 means RGBA
    png.write(outpath.string());
}

void KTex::WriteRotwoodOrGriftlandsFormatImage(const std::filesystem::path& outpath) {
    m_DDS_Data.image.flip();
    m_DDS_Data.image.magick("dds");
    m_DDS_Data.image.write(outpath.string());
}

inline int32_t KTex::GetSquishCompressionFormat(const uint32_t compression) {
    return COMPRESSION_CONVERT[compression];
}