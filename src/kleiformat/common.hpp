#pragma once

#include <stdint.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <map>

#include "strings.hpp"
#include "binary_util.hpp"

// To make ImageMagick shut up about it's warnings!!!
#ifdef _MSC_VER
#	pragma warning( disable: 4251 )
#	pragma warning( disable: 4275 )
#endif

enum class KLEI_FORMATS : uint8_t {
    DONT_STARVE             = 0,
    DONT_STARVE_TOGETHER    = 0, // Same as DS.
    EETS_MUNCHIES           = 1,
    SHANK                   = 2,
    INVISIBLE_INC           = 3,
    MARK_OF_THE_NINJA       = 4,
    GRIFTLANDS              = 5,
    HOT_LAVA                = 6,
    ROTWOOD                 = 7,
    OXYGEN_NOT_INCLUDED     = 8, // NOTE: No tex, just png.
};

enum FACINGS : uint8_t {
    RIGHT       = 1 << 0,
    UP          = 1 << 1,
    LEFT        = 1 << 2,
    DOWN        = 1 << 3,
    UPRIGHT     = 1 << 4,
    UPLEFT      = 1 << 5,
    DOWNRIGHT   = 1 << 6,
    DOWNLEFT    = 1 << 7,
};

static const char* GetFacingsLabel(uint8_t facings) {
    switch(facings) {
        case FACINGS::LEFT | FACINGS::RIGHT | FACINGS::UP | FACINGS::DOWN | FACINGS::DOWNLEFT | FACINGS::DOWNRIGHT | FACINGS::UPLEFT | FACINGS::UPRIGHT:
            return "_all";
        case FACINGS::LEFT | FACINGS::RIGHT:
            return "_side";
        case FACINGS::DOWNLEFT | FACINGS::DOWNRIGHT:
            return "_downside";
        case FACINGS::UPLEFT | FACINGS::UPRIGHT:
            return "_upside";
        case FACINGS::LEFT | FACINGS::RIGHT | FACINGS::UP | FACINGS::DOWN:
            return "_90s";
        case FACINGS::DOWNLEFT | FACINGS::DOWNRIGHT | FACINGS::UPLEFT | FACINGS::UPRIGHT:
            return "_45s";
        case FACINGS::RIGHT:
            return "_right";
        case FACINGS::LEFT:
            return "_left";
        case FACINGS::UP:
            return "_up";
        case FACINGS::DOWN:
            return "_down";
    }

    return "_unknown";
}

// This is the known hash algorithim for Don't Starve and Rotwood. From a quick lance Invisible Inc has a different hashing algorithim so other Klei games 
// may have it different too.
static uint32_t kstrhash(const std::string& str) {
    uint32_t hash = 0;
    for (const char c : str) {
        hash = (tolower(c) + (hash << 6) + (hash << 16) - hash);
    }
    return hash;
}