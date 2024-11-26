#include "kanimviewer.hpp"

int main(int argc, char** argv) {
    //Logger::Initialize("log.txt");
    /*
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Print(std::format("[ERROR] SDL_Init: {}", SDL_GetError()));
        return -1;
    }

    //TODO WIDTH AND HEIGHT SET PROPERLY
    SDL_Window* window = SDL_CreateWindow("Klei Animation Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        Logger::Print(std::format("[ERROR] SDL_CreateWindow: {}", SDL_GetError()));
        return -1;
    }
    */

    //SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    /*
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    */

    //Main loop
    bool done_app = false;
    while (!done_app) {
        /*
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            //ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done_app = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done_app = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();



        // Rendering
        ImGui::Render();
        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        */
    }

    /*
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    */

	return 0;
}