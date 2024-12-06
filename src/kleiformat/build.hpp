#pragma once

#include "common.hpp"
#include "ktex.hpp"

//SEE NOTES.md FOR BUILD FORMATS

class Build {
    private:
        struct vertex {
            float x, y, z, u, v, w;
        };

        struct frame {
            float x, y, w, h;
            uint32_t num, duration, num_vertices, vb_start_index;
            std::vector<vertex> vertices;
        };
        
        struct symbol {
            uint32_t num_frames, hash_name;
            std::string name;
            std::vector<frame> frames;
        };

    public:
        //
        friend class XFL;
        friend class Anim;
        friend class KTex;

        //Load a build(.bin) to object
        Build(const std::filesystem::path& bildpath, KLEI_FORMATS game_format);
        //Export and write atlas's images at out path
        void ExportAtlas(const std::filesystem::path& out_path) const;
        //Export and return list of atlas images (for XFL)
        std::vector<Magick::Image> ExportAtlas() const;
        //Get symbol name of a element (Make sure it's an element that belongs to us!)
        std::string GetSymbolName(const symbol& e) const;
        // Get the number of total symbols in our build
        size_t GetNumSymbols() const;
        // Get the number of total frames in our build
        size_t GetNumFrames() const;
        // Get the number of total vertices in our build
        size_t GetNumVertices() const;
        // Read a stream into our object
        void ReadStream(std::istream& kbild, KLEI_FORMATS game_format, const std::filesystem::path& bildpath);
        // Write to a file
        void WriteToFile(const std::filesystem::path& bildpath);

        uint32_t m_Version;
        KLEI_FORMATS m_BildType;

    private:
        uint32_t m_NumSymbols;
        uint32_t m_NumFrames;
        uint32_t m_NumMaterials;
        uint32_t m_NumVertices;
        uint32_t m_NumHashes;

        std::vector<KTex> m_Materials;
        std::vector<symbol> m_Symbols;
        std::vector<uint32_t> m_Hashes;

        std::map<uint32_t, std::string> m_SymbolHashesToNames;

        std::string m_Name;
    
    private:
        struct BBox {
            float u, v, w, h;
        };

        // Get BBox of a frame // TODO optimization potential, cache bbox if we can
        static BBox GetFrameBBox(const frame& frame);

};