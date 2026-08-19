#include <SDL3/SDL.h>
#include <triangles/opengl/opengl.hpp>


int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    tri::InitializationInfo init_info = {
        .title = "Test"
    };

    tri::OpenGLRenderer renderer;
    renderer.init(init_info);


    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
        }

        renderer.clear();
        renderer.present();
    }


    renderer.deinit();
    SDL_Quit();
    return 0;
}
