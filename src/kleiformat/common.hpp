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
    DONT_STARVE_TOGETHER    = 0, //Same as DS.
    EETS_MUNCHIES           = 1,
    SHANK                   = 2,
    INVISIBLE_INC           = 3,
    MARK_OF_THE_NINJA       = 4,
    GRIFTLANDS              = 5,
    HOT_LAVA                = 6,
    ROTWOOD                 = 7,
    OXYGEN_NOT_INCLUDED     = 8, //No tex, just png.
};

static uint32_t kstrhash(const std::string& str) {
	uint32_t hash = 0;
	for (const char c : str) {
		hash = (tolower(c) + (hash << 6) + (hash << 16) - hash);
	}
	return hash;
}