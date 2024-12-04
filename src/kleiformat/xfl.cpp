#include "xfl.hpp"

#define MAKE_XML_ELEMENT(doc, root, name) root->InsertFirstChild(doc.NewElement(name))->ToElement()

XFL::XFL(const std::filesystem::path& folderpath) {

}

XFL::XFL(const Anim& anim, const Build& bild) {
    //Make sure these match!
    if (anim.m_AnimType != bild.m_BildType)
        throw std::invalid_argument(STRINGS::DIFFERING_FORMATS);

    //TODO animations might reference symbols not in our build! add them too!
    AddAnimationFromKleiFormat(anim);
    AddBuildFromKleiFormat(bild);
}

void XFL::ExportProject(const std::filesystem::path& folderpath) {
    std::filesystem::create_directory(folderpath);

    //xfl file only has the magic string written in
    const std::filesystem::path xflpath = folderpath / "anims.xfl";
    std::ofstream xfl(xflpath, std::ios::out);

    if (!xfl.is_open())
        throw std::runtime_error(std::format(STRINGS::FILE_NOOPEN, "XFL::ExportProject", xflpath.string()));
    
    xfl << K_MAGICS::XFL;
    xfl.close();

    // DOMDocument

    tinyxml2::XMLDocument dom_xml;
    tinyxml2::XMLElement* dom_root = dom_xml.InsertEndChild(dom_xml.NewElement("DOMDocument"))->ToElement();

    if (dom_root) {
        dom_root->SetAttribute("xmlns:xsi",         "http://www.w3.org/2001/XMLSchema-instance");
        dom_root->SetAttribute("xmlns",             "http://ns.adobe.com/xfl/2008/");
        dom_root->SetAttribute("backgroundColor",   "#666666");
        dom_root->SetAttribute("frameRate",         m_FrameRate);
        dom_root->SetAttribute("currentTimeline",   1);
        dom_root->SetAttribute("xflVersion",        2.96);

        tinyxml2::XMLElement* folders = dom_root->InsertEndChild(dom_xml.NewElement("folders"))->ToElement();
        tinyxml2::XMLElement* media = dom_root->InsertEndChild(dom_xml.NewElement("media"))->ToElement();
        tinyxml2::XMLElement* symbols = dom_root->InsertEndChild(dom_xml.NewElement("symbols"))->ToElement();

        if (folders) {
            folders->InsertEndChild(dom_xml.NewElement("DOMFolderItem"))->ToElement()->SetAttribute("name", "FORCEEXPORT");
            folders->InsertEndChild(dom_xml.NewElement("DOMFolderItem"))->ToElement()->SetAttribute("name", "IMPORT");
            folders->InsertEndChild(dom_xml.NewElement("DOMFolderItem"))->ToElement()->SetAttribute("name", "NOEXPORT");
        }

        if (media) {
            for (const Magick::Image& sprite : m_Images) {
                const std::string name = std::format("IMPORT/{}.png", sprite.fileName());
                tinyxml2::XMLElement* media_item = media->InsertEndChild(dom_xml.NewElement("DOMBitmapItem"))->ToElement();

                media_item->SetAttribute("name", name.c_str());
                media_item->SetAttribute("href", name.c_str());
                media_item->SetAttribute("useImportedJPEGData", "false");
                media_item->SetAttribute("compressionType", "lossless");
            }
        }

        if (symbols) {
            for (const auto& item : m_LibraryItems) {
                symbols->InsertEndChild(dom_xml.NewElement("Include"))->ToElement()->SetAttribute("href", item.name.string().c_str());
            }
        }

    }

    const std::filesystem::path dom_xml_path = folderpath / "DOMDocument.xml";
    dom_xml.SaveFile(dom_xml_path.string().c_str());

    // Library Items //
    /*
    PROJECT ->
        LIBRARY ->
            IMPORT -> png files and xml files
            NOEXPORT -> xml files
    */
    const std::filesystem::path library_path = folderpath / "LIBRARY";
    const std::filesystem::path import_path = library_path / "IMPORT";
    const std::filesystem::path noexport_path = library_path / "NOEXPORT";

    std::filesystem::create_directory(library_path);
    std::filesystem::create_directory(import_path);
    std::filesystem::create_directory(noexport_path);

    const std::string image_path_to_format = (import_path / "{}.png").string();
    for (Magick::Image& sprite : m_Images) {
        sprite.write(std::vformat(image_path_to_format, std::make_format_args(sprite.fileName())).c_str());
    }

    for (const auto& item : m_LibraryItems) {
        const std::string no_extension_name = std::filesystem::path(item.name).replace_extension().string();

        tinyxml2::XMLDocument item_xml;

        tinyxml2::XMLElement* item_root = item_xml.InsertEndChild(item_xml.NewElement("DOMSymbolItem"))->ToElement();
        tinyxml2::XMLElement* item_timeline_root = item_root->InsertEndChild(item_xml.NewElement("timeline"))->ToElement();
        tinyxml2::XMLElement* item_timeline = item_timeline_root->InsertEndChild(item_xml.NewElement("DOMTimeline"))->ToElement();

        item_root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        item_root->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
        item_root->SetAttribute("name", no_extension_name.c_str());
        item_root->SetAttribute("symbolType", SYMBOL_TO_STRING[item.type]);

        const std::string no_extension_filename = item.name.filename().replace_extension().string();
        item_timeline->SetAttribute("name", no_extension_filename.c_str());

        tinyxml2::XMLElement* layers = item_timeline->InsertEndChild(item_xml.NewElement("layers"))->ToElement();
        for (const auto& layer : item.layers) {
            tinyxml2::XMLElement* xml_layer = layers->InsertEndChild(item_xml.NewElement("DOMLayer"))->ToElement();

            xml_layer->SetAttribute("name", layer.name.c_str());
            xml_layer->SetAttribute("layerType", LAYER_TO_STRING[layer.type]);

            tinyxml2::XMLElement* frames_root = xml_layer->InsertEndChild(item_xml.NewElement("frames"))->ToElement();
            for (const auto& frame : layer.frames) {
                tinyxml2::XMLElement* xml_frame = frames_root->InsertEndChild(item_xml.NewElement("DOMFrame"))->ToElement();

                xml_frame->SetAttribute("index", frame.index);
                xml_frame->SetAttribute("duration", frame.duration);
                xml_frame->SetAttribute("keyMode", "9728"); // This should always be 9728.

                if (!frame.name.empty()) {
                    xml_frame->SetAttribute("name", frame.name.c_str());
                    xml_frame->SetAttribute("labelType", "name");
                }

                // Only one element per frame!
                tinyxml2::XMLElement* element_root = xml_frame->InsertEndChild(item_xml.NewElement("elements"))->ToElement();
                if (!frame.symbol.library_item.empty()) {
                    const char* INSTANCE_TYPE = frame.symbol.library_item.substr(frame.symbol.library_item.size() - 3) == "png" ? "DOMBitmapInstance" : "DOMSymbolInstance";

                    tinyxml2::XMLElement* xml_element = element_root->InsertEndChild(item_xml.NewElement(INSTANCE_TYPE))->ToElement();
                    tinyxml2::XMLElement* matrix_root = xml_element->InsertEndChild(item_xml.NewElement("matrix"))->ToElement();
                    tinyxml2::XMLElement* xml_matrix = matrix_root->InsertEndChild(item_xml.NewElement("Matrix"))->ToElement();

                    xml_element->SetAttribute("libraryItemName", frame.symbol.library_item.c_str());
                    xml_element->SetAttribute("symbolType", "graphic"); // This should always be graphic
                    xml_element->SetAttribute("firstFrame", frame.symbol.first_frame);       // TODO
                    xml_element->SetAttribute("loop", "single frame");  // TODO we might want this to be changable

                    xml_matrix->SetAttribute("a", frame.symbol.a);
                    xml_matrix->SetAttribute("b", frame.symbol.b);
                    xml_matrix->SetAttribute("c", frame.symbol.c);
                    xml_matrix->SetAttribute("d", frame.symbol.d);
                    xml_matrix->SetAttribute("tx", frame.symbol.tx);
                    xml_matrix->SetAttribute("ty", frame.symbol.ty);

                    // Extra pieces of data that Adobe Animate wants
                    tinyxml2::XMLElement* t_point = item_xml.NewElement("transformationPoint");
                    t_point->InsertEndChild(item_xml.NewElement("Point"));
                    xml_element->InsertEndChild(t_point);
                }
            }
        }

        const std::filesystem::path item_xml_path = library_path / item.name;
        item_xml.SaveFile(item_xml_path.string().c_str());
    }

}

/*
void XFL::ExportDOMDocument(const std::filesystem::path& folderpath) {

}
*/

void XFL::AddAnimationFromKleiFormat(const Anim& anim) {
    //TODO on anims.xml name
    m_FrameRate = anim.ValidateFrameRate();

    library_item& animations = m_LibraryItems.emplace_back("animations.xml", SYMBOL_TYPE::MOVIE_CLIP);
    layer& animations_layer = animations.layers.emplace_back("ANIMATIONS", LAYER_TYPE::GUIDE);

    uint32_t total_timeline_frames = 0; //also the start frame for the next animation guideline as we iterate.
    for (const auto& animation : anim.m_Anims) {
        animations_layer.frames.emplace_back(total_timeline_frames, animation.num_frames, anim.GetFullAnimationName(animation));
        total_timeline_frames += animation.num_frames;
    }

    //

    std::vector<anim_folder> anim_folders;
    anim_folders.reserve(anim.GetNumElements());

    uint32_t num_frame = 0;
    for (const auto& animation : anim.m_Anims) {
        for (const auto& anim_frame : animation.frames) {
            for (const auto& anim_element : anim_frame.elements) {
                auto& folder = anim_folders.emplace_back(anim_element.symbol_hash, anim_element.folder_hash, anim_element.tz, total_timeline_frames);
                folder.elements[num_frame] = &anim_element;
                ++folder.num_elements;
            }

            ++num_frame;
        }
    }

    // (Omar): We shouldn't actually need to sort. The binary data is guaranteed to be written in layering order.
    // But... just in case!
    auto folder_comparator = [](const auto& a, const auto& b) { return a.order < b.order; };
    if (!std::is_sorted(anim_folders.begin(), anim_folders.end(), folder_comparator)) //O(n)
        std::sort(anim_folders.begin(), anim_folders.end(), folder_comparator); //O(nlogn)

    for (uint32_t timeline_frame = 0; timeline_frame < total_timeline_frames; ++timeline_frame) {
        for (auto folder = anim_folders.rbegin(); folder != anim_folders.rend(); ) {
            const Anim::element* element = folder->elements[timeline_frame];
            if (element != nullptr) {
                auto current_folder = std::next(folder).base();

                for (auto next_folder = ++std::next(folder).base(); next_folder != anim_folders.end(); ++next_folder) {
                    if (next_folder->elements[timeline_frame] != nullptr)
                        break;

                    if (next_folder->folder_hash == folder->folder_hash && next_folder->symbol_hash == folder->symbol_hash)
                        current_folder = next_folder;
                }

                if (current_folder->elements != folder->elements) {
                    current_folder->elements[timeline_frame] = element;
                    folder->elements[timeline_frame] = nullptr;

                    ++current_folder->num_elements;
                    --folder->num_elements;
                }
            }

            bool is_empty = folder->num_elements == 0;

            ++folder;

            if (is_empty) // Remove folder if it's empty now to make iterating faster.
                anim_folders.erase( folder.base() ); // Regular iterator is offset from reverse iterator by one, so this is fine!
        }
    }

    for (const auto& folder : anim_folders) {
        const std::string symbol_name = "NOEXPORT/" + anim.m_SymbolHashesToNames.at(folder.symbol_hash);

        std::vector<frame> frames;
        frames.reserve(folder.num_elements); //TODO Not perfect, we might add extra blank frames!

        uint32_t last_added_frame = 0;
        for (uint32_t timeline_frame = 0; timeline_frame < total_timeline_frames; ++timeline_frame) {
            const Anim::element* e = folder.elements[timeline_frame];
            if (e == nullptr)
                continue;

            if (last_added_frame < timeline_frame) // Insert Empty frame range since our last frame!
                frames.emplace_back(last_added_frame, timeline_frame-last_added_frame);

            last_added_frame = timeline_frame + 1;
            frames.emplace_back(timeline_frame, 1, symbol_name, e->symbol_frame, e->a, e->b, e->c, e->d, e->tx, e->ty, e->tz);
        }

        animations.layers.emplace_back(anim.m_SymbolHashesToNames.at(folder.folder_hash), LAYER_TYPE::NORMAL, std::move(frames));
    }
}

void XFL::AddBuildFromKleiFormat(const Build& bild) {
    const auto atlased_images = bild.ExportAtlas();
    m_Images.insert(m_Images.end(), atlased_images.begin(), atlased_images.end());

    // Adding library items.

    for (const auto& bild_symbol : bild.m_Symbols) {
        const std::string symbol_name = bild.GetSymbolName(bild_symbol);
        auto& library_item = m_LibraryItems.emplace_back(std::format("NOEXPORT/{}.xml", symbol_name), SYMBOL_TYPE::GRAPHIC);
        auto& item_layer = library_item.layers.emplace_back("sprites");
        item_layer.frames.reserve(bild_symbol.num_frames);

        uint32_t last_added_frame = 0;
        for (const auto& bild_frame : bild_symbol.frames) {
            if (last_added_frame < bild_frame.num) // Insert Empty frame range since our last frame!
                item_layer.frames.emplace_back(last_added_frame, bild_frame.num-last_added_frame);

            last_added_frame = bild_frame.num + bild_frame.duration;

            item_layer.frames.emplace_back(bild_frame.num, bild_frame.duration, std::format("IMPORT/{}-{}", symbol_name, bild_frame.num),
                0, 1.0f, 0.0f, 0.0f, 1.0f, bild_frame.x, bild_frame.y, 0.0f);
        }
    }

    for (const auto& sprite : atlased_images) {
        auto& library_item = m_LibraryItems.emplace_back(std::format("IMPORT/{}.xml", sprite.fileName()), SYMBOL_TYPE::GRAPHIC);
        auto& item_layer = library_item.layers.emplace_back("sprite");

        const float tx = -static_cast<float>(sprite.columns() / 2);
        const float ty = -static_cast<float>(sprite.rows() / 2);

        item_layer.frames.emplace_back(0, 1, std::format("IMPORT/{}.png", sprite.fileName()),
            0, 1.0f, 0.0f, 0.0f, 1.0f, tx, ty, 0.0f);
    }
}