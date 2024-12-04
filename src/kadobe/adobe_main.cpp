#include "adobe_main.hpp"

/* JS extensions
InitializeKleiLib - For initializing imagemagick and logger

CompileKleiFormat - Takes a path to intemedtiary data + anim/bild version specifications and compiles bins(or whatever format)

DecompileKleiFormat - Takes a path to a zip/folder and an out path, looks for atlases/bins and exports out to a xfl accordingly

LogPrint - To print to the log from the JavaScript end
*/

// Helper functions
static std::filesystem::path GetPath(JSContext* ctx, JSValue value) {
    uint32_t  path_size = 0;
    uint16_t* path_ptr  = nullptr;

    if (!JS_ValueToString(ctx, value, &path_ptr, &path_size))
        throw std::runtime_error(STRINGS::JSAPI::UNABLE_TO_READ_STRING);

    return reinterpret_cast<char16_t*>(path_ptr);
}

static bool InitializeKleiLib(JSContext* ctx, JSObject* obj, unsigned int argc, JSValue argv[], JSValue* rval) {
    if (argc != 1)
        return false;

    static bool initialized = false;
    if (initialized) return true; //We're already initialized.

    initialized = true;

    const std::filesystem::path config_path = GetPath(ctx, argv[0]);
    //
    const std::string dll_path = (config_path / "External Libraries" / "kadobe.dll").string();
    const std::string log_path = (config_path / "klei_library_log.txt").string();
    //
    Magick::InitializeMagick(dll_path.c_str());
    Logger::Initialize(log_path);

    return true;
}

static bool DecompileKleiFormat(JSContext* ctx, JSObject* obj, unsigned int argc, JSValue argv[], JSValue* rval) {
    if (argc != 3)
        return false;

    const std::filesystem::path folder_path = GetPath(ctx, argv[0]);
    const std::filesystem::path out_path = GetPath(ctx, argv[1]);
    long js_game_format;

    if (!JS_ValueToInteger(ctx, argv[1], &js_game_format))
        return false;

    const KLEI_FORMATS game_format = static_cast<KLEI_FORMATS>(js_game_format);

    Logger::Print("DecompileKleiFormat Start Execution");
    Logger::Print("Path to Klei Format Data: " + folder_path.string());
    Logger::Print("Path to output XFL project: " + out_path.string());
    Logger::Print(std::format("Klei Format: {}", static_cast<uint8_t>(game_format)));

    try {
        Anim anim(folder_path, game_format);
        Build bild(folder_path, game_format);

        XFL xfl_doc(anim, bild);
        xfl_doc.ExportProject(out_path);
    } catch (const std::exception& e) {
        Logger::Print(e.what());
    }

    Logger::Print("DecompileKleiFormat End Execution");

    return true;
}

static bool CompileKleiFormat(JSContext* ctx, JSObject* obj, unsigned int argc, JSValue argv[], JSValue* rval) {
    if (argc != 3)
        return false;

    const std::filesystem::path path_to_data = GetPath(ctx, argv[0]);
    long js_game_format;
    double scale;

    if (!JS_ValueToInteger(ctx, argv[1], &js_game_format))
        return false;

    if (!JS_ValueToDouble(ctx, argv[2], &scale))
        return false;

    const KLEI_FORMATS game_format = static_cast<KLEI_FORMATS>(js_game_format);

    try {
        SaveFolderToZip(path_to_data);
    } catch (const std::exception& e) {
        Logger::Print(e.what());
    }

    //TODO Phase out DS Mod Tools dependency
    const std::string command = std::format(
       R""("cmd.exe /v /c "for /f "tokens=2*" %a in ('reg.exe query "HKEY_CURRENT_USER\Software\Valve\Steam" /v "SteamPath" ^| find /i "SteamPath"') do set STEAM_PATH=%b && echo STEAM_PATH: !STEAM_PATH! && cd !STEAM_PATH!\steamapps\common\Don't Starve Mod Tools\mod_tools\ && echo Compiling: && buildtools\windows\Python27\python.exe tools\scripts\buildanimation.py {} --scale {}"")""
    , path_to_data.string() + ".zip", scale);

    int errorcode = std::system(command.c_str());
    if (errorcode) {
        Logger::Print(std::format("CompileKleiFormat executed a command but returned errorcode: {}", errorcode));
        return false;
    }

    return true;
}

static bool LogPrint(JSContext* ctx, JSObject* obj, unsigned int argc, JSValue argv[], JSValue* rval) {
    if (argc != 1)
        return false;

    Logger::Print(GetPath(ctx, argv[0]).string()); // Bit bad, but I needed a quick and dirty way to convert utf16 to utf8. And filesystem::path standard handles that :)
    return true;
}

void MM_Init() {
    // JS APIs
    JS_DefineFunction(JSStr("Initialize"),          &InitializeKleiLib,     1); //path to config folder
    JS_DefineFunction(JSStr("DecompileKleiFormat"), &DecompileKleiFormat,   3); //path to folder, out path, game_format
    JS_DefineFunction(JSStr("CompileKleiFormat"),   &CompileKleiFormat,     3); //path to inter. folder, game_format
    JS_DefineFunction(JSStr("LogPrint"),            &LogPrint,              1); //print messages
}