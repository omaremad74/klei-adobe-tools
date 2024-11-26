#pragma once

#include <stdint.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <forward_list>

#include "tinyxml2.h"

#include "build.hpp"
#include "anim.hpp"

#include "binary_util.hpp"

/*
XFL format
[bank_name].xfl -> "PROXY-CS5"

DOMDocument.xml ->
    folders ->
        FORCEEXPORT
        IMPORT
        NOEXPORT
    media ->
        imported/decompiled images e.g. IMPORT/example-0.png
    symbols ->
        xml data for each symbols e.g. NOEXPORT/example.xml
    anims_name.xml -> contains animation data, layers with frames

LIBRARY ->
    FORCEEXPORT ->
        contains symbols to be exported as part of the build
    IMPORT -> 
        contains imported images and symbol data for each image
    NOEXPORT -> 
        contains symbols used in animations but not exported as part of the build
    [bank_name].xml -> contains animations to be exported
*/

// A class to hold data for Adobe's XFL file type (An uncompressed animation project making use of xml)
class XFL {
	public:
        //Load an existing xfl project to object
		XFL(const std::filesystem::path& folderpath);
        //Load build+bank to object
        XFL(const Anim& anim, const Build& bild);

        //Export into an .xfl project.
        void ExportProject(const std::filesystem::path& folderpath);
        //Add animations from klei format Anim
        void AddAnimationFromKleiFormat(const Anim& anim);
        //Add build from klei format Build
        void AddBuildFromKleiFormat(const Build& bild);

	private:
        enum LAYER_TYPE : uint8_t {
            NORMAL  = 0,
            GUIDE   = 1,
            FOLDER  = 2,
        };

        enum SYMBOL_TYPE : uint8_t {
            MOVIE_CLIP  = 0,
            GRAPHIC     = 1,
            BUTTON      = 2,
        };

        //enum INSTANCE_TYPE "DOMBitmapInstance", "DOMSymbolInstance"

        static constexpr const char* LAYER_TO_STRING[3] = {"normal", "guide", "folder"};
        static constexpr const char* SYMBOL_TO_STRING[3] = {"movieclip", "graphic", "button"};

        struct element {
            element() {};
            element(const std::string& library_item, const uint32_t first_frame, const float a, const float b, const float c, const float d, const float tx, const float ty, const float tz) 
                : library_item(library_item), first_frame(first_frame), a(a), b(b), c(c), d(d), tx(tx), ty(ty), tz(tz) {}

            std::string library_item; //filesystem::path?
            uint32_t first_frame = 1;
            float a = 1.0f, b = 0.0f, c = 0.0f, d = 1.0f, tx = 0.0f, ty = 0.0f, tz = 0.0f;
        };

        struct frame {
            frame(const uint32_t index, const uint32_t duration) 
                : index(index), duration(duration) {}
            frame(const uint32_t index, const uint32_t duration, const std::string name) 
                : index(index), duration(duration), name(name) {}
            frame(const uint32_t index, const uint32_t duration,
                const std::string& library_item, const uint32_t first_frame, const float a, const float b, const float c, const float d, const float tx, const float ty, const float tz) 
                : index(index), duration(duration), symbol(library_item, first_frame, a, b, c, d, tx, ty, tz) {}
    
            uint32_t index = 0, duration = 0;
            element symbol; //Each frame can have ONLY one element.
            std::string name; //For ANIMATIONS layer
        };

        struct layer {
            layer(const std::string name) 
                : name(name) {}
            layer(const std::string name, const LAYER_TYPE type) 
                : name(name), type(type) {}
            layer(const std::string name, const LAYER_TYPE type, const std::vector<frame>& frames) 
                : name(name), type(type), frames(frames) {}

            std::string name;
            LAYER_TYPE type = LAYER_TYPE::NORMAL;
            std::vector<frame> frames;
        };

        struct anim_folder { //meant for transferring Klei Anim data to xfl
            anim_folder(const uint32_t symbol_hash, const uint32_t folder_hash, const float order, const uint32_t max_elements) 
                : symbol_hash(symbol_hash), folder_hash(folder_hash), order(order), elements(new const Anim::element*[max_elements]()) {}
            ~anim_folder() { delete[] elements; }

            anim_folder(anim_folder&& other) 
                : symbol_hash(other.symbol_hash), folder_hash(other.folder_hash), order(other.order), elements(other.elements), num_elements(other.num_elements) {  
                other.symbol_hash = 0;
                other.folder_hash = 0;
                other.order = 0.0f;
                other.elements = nullptr;
                other.num_elements = 0;
            }

            anim_folder& operator=(anim_folder&& other) {
                if (this != &other) {
                    delete[] elements;
                    symbol_hash = other.symbol_hash;
                    folder_hash = other.folder_hash;
                    order = other.order;
                    elements = other.elements;
                    num_elements = other.num_elements;
                    //
                    other.symbol_hash = 0;
                    other.folder_hash = 0;
                    other.order = 0.0f;
                    other.elements = nullptr;
                    other.num_elements = 0;
                }
                //
                return *this;
            }

            uint32_t symbol_hash = 0, folder_hash = 0;
            float order = 0.0f;
            const Anim::element** elements = nullptr;
            uint32_t num_elements = 0; //when reach 0 after being initialized, we kill off this folder.
        };

        struct library_item {
            library_item(const std::string& name) 
                : name(name) {}
            library_item(const std::string& name, const SYMBOL_TYPE type) 
                : name(name), type(type) {}

            std::filesystem::path name;
            SYMBOL_TYPE type = SYMBOL_TYPE::MOVIE_CLIP;
            std::vector<layer> layers;
        };

        std::vector<Magick::Image> m_Images;
        std::vector<library_item> m_LibraryItems;

        float m_FrameRate = 30; //It's going to usually be 30.
        std::string name;

};