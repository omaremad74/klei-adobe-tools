#pragma once

#include "common.hpp"
#include "squish/squish.h"
#include <Magick++.h>

//TODO pre multiply alpha?

//SEE NOTES.md FOR BUILD FORMATS

class KTex {
    public:
        friend class Build;
        //
        enum COMPRESSION_FORMATS : uint32_t {
            DXT1 = 0,
            DXT3 = 1,
            DXT5 = 2,
            RGBA = 3
        };

        enum TEXTURE_TYPES : uint32_t {
            ONE_DIMENSION = 0,
            TWO_DIMENSION = 1
        };

        //No loaded data.
        KTex();
        //Name loaded.
        KTex(const std::string& name);
        //Load an image to convert into texture
        KTex(const std::string& name, const Magick::Image img);
        //Load existing KTEX
        KTex(const std::string& name, const std::filesystem::path& texpath, KLEI_FORMATS tex_type);
        //Read a tex file(zipped or folder path) into class
        void ReadFile(const std::filesystem::path& imagepath, KLEI_FORMATS tex_type);
        //Read a stream into class
        void ReadStream(std::istream& ktex, KLEI_FORMATS game_format);
        //Get squish compression
        static inline int32_t GetSquishCompressionFormat(const uint32_t compression);
        //Write tex file to png
        void WriteImage(const std::filesystem::path& outpath);

        static constexpr inline const uint32_t COMPRESSION_CONVERT[3] = {squish::kDxt1, squish::kDxt3, squish::kDxt5};

    //
    private:
        struct MipMap {
            uint16_t width, height, pitch;
            uint32_t data_size;
            Magick::Blob data;
        };

        struct MipMapDDS {
            uint16_t width, height, pitch;
            Magick::Image image;
        };

        struct image_specs { //4 bytes
            uint32_t platform       : 4;
            uint32_t compression    : 5;
            uint32_t tex_type       : 4;
            uint32_t mipmap_count   : 5;
            uint32_t flags          : 2;
            uint32_t fill           : 12;
        };

        union flags {
            uint32_t data;
            image_specs spec;
        };

        flags m_Flags;
        std::vector<MipMap> m_MipMaps; //DS/T
        MipMapDDS m_DDS_Data; //Rotwood+Griftlands

        std::string m_Name;
        KLEI_FORMATS m_Tex_type;

    private:
        void DecompileDontStarveFormat(std::istream& ktex);    // DS/T
        void DecompileRotwoodOrGriftlandsFormat(std::istream& ktex);                                   // Rotwood + Griftlands
        //
        void WriteDontStarveFormatImage(const std::filesystem::path& outpath);                          // DS/T
        void WriteRotwoodOrGriftlandsFormatImage(const std::filesystem::path& outpath);                 // Rotwood + Griftlands
};