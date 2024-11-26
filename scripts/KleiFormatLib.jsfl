//Library for working with Don't Starve format
//TODO: Get it working for other Klei games like ONI, Mark of the Ninja, Invisible Inc, etc.

// Initialization

kadobe.Initialize(fl.configDirectory) // For logger and imagemagick
fl.outputPanel.clear()

//Keep in sync with common.hpp's KLEI_FORMATS enum definition
KLEI_FORMATS = {
    DONT_STARVE             : 0,
    DONT_STARVE_TOGETHER    : 0,
    EETS_MUNCHIES           : 1,
    SHANK                   : 2,
    INVISIBLE_INC           : 3,
    MARK_OF_THE_NINJA       : 4,
    GRIFTLANDS              : 5,
    HOT_LAVA                : 6,
    ROTWOOD                 : 7,
    OXYGEN_NOT_INCLUDED     : 8, //No tex, just png.
}

STRINGS = {
    EXPORT_ANIMATION : {
        NOT_GUIDE : "Your animation markers layer is not set as a guide layer, please fix that!",
        NO_ANIMATION_LAYER : "No animation markers layer found, please make one!",
        DUPLICATE_SYMBOL : "Duplicate symbol found in a frame. You must keep it to one!",
        WRONG_SYMBOL_TYPE : "Incorrect symbol type, your animations symbol should be of 'Movie Clip' type.",
    },
    EXPORT_BUILD : {
        WRONG_SYMBOL_TYPE : "Incorrect symbol type, your build symbol should be of 'Graphic' type.",
    },
    COMMON_ERRORS : {
        UNSAVED_DOC : "The document is unsaved and has no proper directory path, please 'Save As' before you try to compile!",
    },
}

//

function PadZeroes(num, size) {
    num = num.toString();
    while (num.length < size) num = "0" + num;
    return num;
}

function Print(print_message) {
    fl.trace(print_message)
    kadobe.LogPrint(print_message)
}

function KFormatError(type, error) {
    return STRINGS[type][error]
}

function Round(value, decimals) {
    return Number(Math.round(value+'e'+decimals)+'e-'+decimals)
}

function GetFrame(symbol, start_frame, current_frame, framecount) {
    if (symbol.loop == "loop") //Let's make sure this is properly looping!
        return (symbol.firstFrame - start_frame + current_frame) % (framecount + 1)

    return symbol.firstFrame
}

function GetLibraryName(library_item) {
    return library_item.name.split("/").pop() //(Omar): get rid of the folder pathing
}

function GetBaseFolderName(library_item) {
    return library_item.name.split("/")[0]
}

//

KleiFormatExporter = function() {
    var MAIN_DOC = fl.getDocumentDOM()
    if (!MAIN_DOC.pathURI)
        throw KFormatError("COMMON_ERRORS", "UNSAVED_DOC")

    this.frame_rate = MAIN_DOC.frameRate
    this.timeline = MAIN_DOC.getTimeline()
    this.library = MAIN_DOC.library
    this.anim_markers_layer = null
    this.main_path = MAIN_DOC.pathURI.slice(0, -4) //(Omar): The path just without the extension

    FLfile.createFolder(this.main_path) //Where our export data will go
}

KleiFormatExporter.prototype.ExportAnim = function(version) {
    this.anim_markers_layer = this.timeline.layers[0]

    if (this.anim_markers_layer.name.toUpperCase() != "ANIMATIONS") throw KFormatError("EXPORT_ANIMATION", "NO_ANIMATION_LAYER")
    if (this.anim_markers_layer.layerType != "guide")               throw KFormatError("EXPORT_ANIMATION", "NOT_GUIDE")

    var anims_xml = <Anims></Anims>

    for (var f = 0; f < this.anim_markers_layer.frameCount; f++) {
        var bank_name = this.timeline.name
        var current_frame = this.anim_markers_layer.frames[f]
        var anim_name = current_frame.name
        var num_frames = current_frame.duration
        var include_anim = current_frame.labelType != "comment"

        if (!include_anim) continue //(Omar): I LOVE YOU `CONTINUE`, HOW LONG HAS IT BEEN? I'VE BEEN TRAPPED IN LUA, CONSTRAINED AND CHAINED, BLINDED FROM A GREATER PARADISE.

        var anim = <anim></anim>
        anim.@name = anim_name
        anim.@root = bank_name
        anim.@numframes = num_frames
        anim.@framerate = this.frame_rate

        for (var ff = f; ff < f+num_frames; ff++) {
            var rect = this.timeline.getBounds(ff+1, false)
            if (rect == 0) continue

            var width = rect.right - rect.left
            var height = rect.bottom - rect.top
            var x = Round(rect.left + (width / 2), 2)
            var y = Round(rect.top + (height / 2), 2)

            var frame = <frame></frame>
            frame.@w = width
            frame.@h = height
            frame.@x = x
            frame.@y = y

            //start at 1, we don't care about the animation guideline layer! (0th layer)
            for (var l = 1; l < this.timeline.layerCount; l++) {
                var frame_container = this.timeline.layers[l].frames[ff]
                if (!frame_container) continue
                if (frame_container.elements.length > 1) throw KFormatError("EXPORT_ANIMATION", "DUPLICATE_SYMBOL")

                var symbol = frame_container.elements[0]

                var element = <element></element>
                element.@name = GetLibraryName(symbol.libraryItem)
                element.@layername = this.timeline.layers[l].name,
                element.@frame = GetFrame(symbol, frame_container.startFrame, ff, symbol.libraryItem.timeline.frameCount)
                element.@z_index = l - 1
                //
                element.@m_a = symbol.matrix.a
                element.@m_b = symbol.matrix.b
                element.@m_c = symbol.matrix.c
                element.@m_d = symbol.matrix.d
                element.@m_tx = symbol.matrix.tx
                element.@m_ty = symbol.matrix.ty
                //
                /*
                if (version == KLEI_FORMATS::OXYGEN_NOT_INCLUDED) {
                    element.@r = symbol.colorRedPercent
                    element.@g = symbol.colorGreenPercent
                    element.@b = symbol.colorBluePercent
                    element.@a = symbol.colorAlphaPercent
                }
                */
            
                frame.appendChild(element)
            }

            anim.appendChild(frame)
        }

        anims_xml.appendChild(anim)

        f += num_frames - 1 //Skip ahead
    }

    //Write
    FLfile.write(this.main_path + "/animation.xml", anims_xml.toXMLString())
}

KleiFormatExporter.prototype.ExportBuild = function(version) {
    var build_xml = <Build></Build>
    build_xml.@name = document.name.slice(0, -4)

    for (var i = 0; i < this.library.items.length; i++) {
        var item = this.library.items[i]
        if (GetBaseFolderName(item) != "FORCEEXPORT") continue
        if (item.itemType != "graphic")               throw KFormatError("EXPORT_BUILD", "WRONG_SYMBOL_TYPE")

        var symbol = <Symbol></Symbol>
        symbol.@name = GetLibraryName(item)

        var layer = item.timeline.layers[0]
        for (var f = 0; f < item.timeline.frameCount; f++) {
            if (layer.frames[f] == null) continue
            var rect = item.timeline.getBounds(f+1, false) //This does NOT start at 0, learned that the hard way
            if (rect == 0) continue
            
            var duration = layer.frames[f].duration
            var width = Round(rect.right - rect.left, 0)
            var height = Round(rect.bottom - rect.top, 0)
            var x = Round(rect.left + (width / 2), 2)
            var y = Round(rect.top + (height / 2), 2)
            //
            var frame = <Frame></Frame>
            frame.@framenum = f
            frame.@duration = duration
            frame.@image = symbol.@name + PadZeroes(f, 4) // exportToPNGSequence exports in the following format e.g. "test0001.png", "test0023.png"
            frame.@w = width
            frame.@h = height
            frame.@x = x
            frame.@y = y
            
            symbol.appendChild(frame)

            f += duration - 1
        }

        item.exportToPNGSequence(this.main_path + "/" + symbol.@name + ".png");

        build_xml.appendChild(symbol)
    }

    //Write
    FLfile.write(this.main_path + "/build.xml", build_xml.toXMLString())
}

// global funcs

ExportKleiAnimation = function(game_format) {
    game_format = game_format ? game_format : KLEI_FORMATS.DONT_STARVE //We'll default to DS format, because, well, you know, it's the popular one.

    try {
        var exporter = new KleiFormatExporter()
        exporter.ExportAnim(game_format)
    } catch(err) {
        Print("Caught exception while Exporting a Klei Format Animation:\n" + err)
    }
}

ExportKleiBuild = function(game_format) {
    game_format = game_format ? game_format : KLEI_FORMATS.DONT_STARVE

    try {
        var exporter = new KleiFormatExporter()
        exporter.ExportBuild(game_format)
    } catch(err) {
        Print("Caught exception while Exporting a Klei Format Build:\n" + err)
    }
}

CompileKleiFormat = function(game_format) {
    game_format = game_format ? game_format : KLEI_FORMATS.DONT_STARVE

    try {
        var exporter = new KleiFormatExporter()
        exporter.ExportAnim(game_format)
        exporter.ExportBuild(game_format)

        var data_path = FLfile.uriToPlatformPath(exporter.main_path)
        kadobe.CompileKleiFormat(data_path, game_format)
    } catch(err) {
        Print("Caught exception while Exporting to Klei Format: " + err)
    }
}

DecompileKleiFormat = function(game_format) {
    game_format = game_format ? game_format : KLEI_FORMATS.DONT_STARVE

    var file_description = "Klei Format Zip (*.zip)"
    var file_filter = "zip"
    var data_uri = fl.browseForFileURL("select", "Select a ZIP file containing Klei Format Data", file_description, file_filter)

    if (!data_uri || !data_uri.length)
        return false

    var output_uri = fl.browseForFolderURL("Select a folder to output the XFL Project")

    if (!output_uri || !output_uri.length)
        return false

    var data_path = FLfile.uriToPlatformPath(data_uri)
    var output_path = FLfile.uriToPlatformPath(output_uri)

    kadobe.DecompileKleiFormat(data_path, output_path, game_format)
}

//DecompileKleiFormat(KLEI_FORMATS.DONT_STARVE)
//CompileKleiFormat(KLEI_FORMATS.DONT_STARVE)