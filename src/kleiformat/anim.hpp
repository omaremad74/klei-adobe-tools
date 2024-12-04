#pragma once

#include "common.hpp"

//SEE NOTES.md FOR ANIM FORMATS

class Anim {
    private: //Structs
        struct element {
           uint32_t symbol_hash, folder_hash, symbol_frame;
           float col_a, col_b, col_g, col_r; //Ver 5
           float a, b, c, d, tx, ty, tz;
        };

        struct frame {
            float x, y, w, h;
            uint32_t num_events, num_elements;
            std::vector<uint32_t> event_hashes;
            std::vector<element> elements;
        };

        struct anim {
            std::string name;
            uint32_t bank_hash, num_frames;
            float frame_rate;
            uint8_t facings; //Ver 4, 5 (Rotwood)
            std::vector<frame> frames;
        };

    public:
        //
        friend class XFL;
        friend class Build;

        //Load an anim(.bin) file to object
        Anim(const std::filesystem::path& animpath, KLEI_FORMATS game_format);
        //Load a stream expected to contain anim data to object
        Anim(std::istream& kanim, KLEI_FORMATS game_format);

        uint32_t m_Version;
        KLEI_FORMATS m_AnimType;

        //Validate our frame rates to make sure they're all the same.
        float ValidateFrameRate() const;
        //Get folder name of a element (Make sure it's an element that belongs to us!)
        std::string GetFolderName(const element& e) const;
        //Get symbol name of a element (Make sure it's an element that belongs to us!)
        std::string GetSymbolName(const element& e) const;
        //Get full name of animation including label if it exists
        std::string GetFullAnimationName(const anim& a) const;
        // Get the number of total elements in our animation
        size_t GetNumElements() const;
        // Read a stream into our object
        void ReadStream(std::istream& kanim, KLEI_FORMATS game_format);

    private:

        uint32_t m_NumElementRefs;
        uint32_t m_NumFrames;
        uint32_t m_NumEvents;
        uint32_t m_NumAnims;
        uint32_t m_NumHashes;

        std::vector<anim> m_Anims;
        std::vector<uint32_t> m_Hashes;

        std::map<uint32_t, std::string> m_SymbolHashesToNames;

        //std::string bank_name;
};