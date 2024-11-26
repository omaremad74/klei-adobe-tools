#include "anim.hpp"
#include "logger.hpp"

Anim::Anim(std::istream& kanim, KLEI_FORMATS game_format) {
    ReadStream(kanim, game_format);
}

Anim::Anim(const std::filesystem::path& animpath, KLEI_FORMATS game_format) {
    bool is_zip = animpath.extension() == ".zip";

    if (is_zip) {
        ZipStream anim_data = GetZipStreamFromFile(animpath, "anim.bin"); //TODO we're assuming the anim is `anim.bin`, true for DS/T but not for all formats!

        ReadStream(anim_data.stream, game_format);

        delete[] anim_data.buffer;
    } else {
        std::ifstream kanim(animpath / "anim.bin", std::ios::in | std::ios::binary);

        if (!kanim.is_open())
            throw std::format(STRINGS::FILE_NOOPEN, "Anim::Anim(const std::filesystem::path& animpath, KLEI_FORMATS game_format)", animpath.string());

        ReadStream(kanim, game_format);

        kanim.close();
    }
}

void Anim::ReadStream(std::istream& kanim, KLEI_FORMATS game_format) {
    m_AnimType = game_format;

    /*
    switch (game_format) {
    
    }
    */

    std::string magic;
    read_bin_string(kanim, magic, 4);
    if (magic != K_MAGICS::ANIM)
        throw STRINGS::ANIMATION_ERRORS::INCORRECT_MAGIC;

    read_bin_data(kanim, m_Version);
    read_bin_data(kanim, m_NumElementRefs);
    read_bin_data(kanim, m_NumFrames);
    read_bin_data(kanim, m_NumEvents);
    read_bin_data(kanim, m_NumAnims);

    m_Anims.resize(m_NumAnims);

    for (auto& anim : m_Anims) {
        read_bin_kleistring(kanim, anim.name);

        read_bin_data(kanim, anim.facings);
        read_bin_data(kanim, anim.bank_hash);
        read_bin_data(kanim, anim.frame_rate);
        
        read_bin_data(kanim, anim.num_frames);
        anim.frames.resize(anim.num_frames);

        for (auto& frame : anim.frames) {
            read_bin_data(kanim, frame.x);
            read_bin_data(kanim, frame.y);
            read_bin_data(kanim, frame.w);
            read_bin_data(kanim, frame.h);

            read_bin_data(kanim, frame.num_events); //Ver 4 and Rotwood only I think?
            frame.event_hashes.resize(frame.num_events);

            for (uint32_t& event_hash : frame.event_hashes) {
                read_bin_data(kanim, event_hash);
            }

            read_bin_data(kanim, frame.num_elements);
            frame.elements.resize(frame.num_elements);

            for (auto& element : frame.elements) {
                read_bin_data(kanim, element.symbol_hash);
                read_bin_data(kanim, element.symbol_frame);
                read_bin_data(kanim, element.folder_hash);

                read_bin_data(kanim, element.a);
                read_bin_data(kanim, element.b);
                read_bin_data(kanim, element.c);
                read_bin_data(kanim, element.d);
                read_bin_data(kanim, element.tx);
                read_bin_data(kanim, element.ty);
                read_bin_data(kanim, element.tz);
            }
        }
    }

    read_bin_data(kanim, m_NumHashes);
    m_Hashes.resize(m_NumHashes);

    for (uint32_t& hash : m_Hashes) {
        read_bin_data(kanim, hash);
        read_bin_kleistring(kanim, m_SymbolHashesToNames[hash]);
    }
}

float Anim::ValidateFrameRate() const {
    float last_frame_rate = NULL;

    for (const auto& anim : m_Anims) {
        const float anim_frame_rate = anim.frame_rate;

        if (last_frame_rate && last_frame_rate != anim_frame_rate) {
            using namespace STRINGS::ANIMATION_WARNINGS;

            Logger::Print(DIFFERING_FRAME_RATE::ONE);
            Logger::Print(std::format(DIFFERING_FRAME_RATE::TWO, anim.name));
        }

        last_frame_rate = anim_frame_rate;
    }

    return last_frame_rate;
}

std::string Anim::GetFolderName(const element& e) const {
    return m_SymbolHashesToNames.at(e.folder_hash);
}

std::string Anim::GetSymbolName(const element& e) const {
    return m_SymbolHashesToNames.at(e.symbol_hash);
}

size_t Anim::GetNumElements() const {
    size_t total = 0;
    //
    for (const auto& animation : m_Anims) {
        for (const auto& anim_frame : animation.frames) {
            total += anim_frame.elements.size();
        }
    }
    //
    return total;
}