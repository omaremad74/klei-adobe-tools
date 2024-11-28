#pragma once

#define GLOBAL_STRING constexpr inline const char*

namespace STRINGS {
    namespace ANIMATION_ERRORS {
        GLOBAL_STRING INCORRECT_MAGIC = "[ERROR] This is not a valid Klei ANIM file!";
        GLOBAL_STRING INVALID_VERSION = "We can't export to this KAnim version for it doesn't exist!";
    };

    namespace BUILD_ERRORS {
        GLOBAL_STRING INCORRECT_MAGIC = "[ERROR] This is not a valid Klei BILD file!";
        GLOBAL_STRING INVALID_VERSION = "[ERROR] We can't export to this KBild version for it doesn't exist!";
        GLOBAL_STRING INCORRECT_VERT_COUNT = "[ERROR] Frame {} for Symbol {} has an incorrect amount of vertices. It should be divisible by 6 as it is meant to be constructed of pairs of two triangles.";
    };

    namespace TEXTURE_ERRORS {
        GLOBAL_STRING INCORRECT_MAGIC = "[ERROR] This is not a valid Klei KTEX file!";
        GLOBAL_STRING INVALID_VERSION = "[ERROR] We can't export to this KTex version for it doesn't exist!";
    };

    namespace ZIP_ERRORS {
        GLOBAL_STRING ERROR_CODE = "[ERROR] Received error when trying to open a zip file: {}";
        GLOBAL_STRING UNABLE_TO_LOCATE_FILE = "[ERROR] File: {} does not exist in zip: {}";
        GLOBAL_STRING UNABLE_TO_STAT_FILE = "[ERROR] Unable to stat file {} in {}. Error: {}";
        GLOBAL_STRING UNABLE_TO_OPEN_FILE = "[ERROR] Unable to open file: {} in {}. Error: {}";
        GLOBAL_STRING UNABLE_TO_READ_FILE = "[ERROR] Unable to read file contents: {} in {}. Error: {}";
        GLOBAL_STRING UNABLE_TO_ADD_DIR = "[ERROR] Unable to add directory: {}, to zip archive.";
    };

    //

    namespace ANIMATION_WARNINGS {
        namespace DIFFERING_FRAME_RATE {
            GLOBAL_STRING ONE = "[WARNING] - We loaded some animations in the XFL constructor but they have mismatching frame rates!";
            GLOBAL_STRING TWO = "We can only support one specified frame rate, our troublesome animation is: {}";
        };
    };

    //

    GLOBAL_STRING DIFFERING_FORMATS = "[ERROR] XFL::XFL(const Anim& anim, const Build& bild) was supplied two differing format types.";
    GLOBAL_STRING FILE_NOOPEN = "[ERROR] {} - We couldn't open the file at the path {}. Are you sure it exists and has proper permissions?";
    GLOBAL_STRING UNABLE_READ_STREAM = "[ERROR] We were unable to read from the stream, it might be corrupted.";
    GLOBAL_STRING UNABLE_WRITE_STREAM = "[ERROR] We were unable to write to the stream, it might be corrupted.";

    // JS API errors
    namespace JSAPI {
        GLOBAL_STRING UNABLE_TO_READ_STRING = "[ERROR] We were unable to read a string provided by the JavaScript Context!";
    };
    
};

namespace K_MAGICS {
    GLOBAL_STRING ANIM = "ANIM";
    GLOBAL_STRING BILD = "BILD";
    GLOBAL_STRING TEX  = "KTEX";
    GLOBAL_STRING XFL  = "PROXY-CS5"; //Not a Klei magic technically.
}

#undef GLOBAL_STRING