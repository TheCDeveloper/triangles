#include <SDL3/SDL.h>
#include <triangles/opengl/opengl.hpp>


int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    tri::InitializationInfo init_info = {
        .resizable = true,
        .title = "Test"
    };

    tri::OpenGLRenderer renderer;
    renderer.init(init_info);

    if (!renderer.initialized()) {
        return 1;
    }


    tri::TextureHandle texture = renderer.create_texture("tests/res/player.bmp");


    float x = 0.0f;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            renderer.event(event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
        }

        x += 0.08f * 16.7f;

        renderer.clear();
        renderer.render_texture(texture, x, x/2, 100, 100);
        renderer.present();
    }


    renderer.destroy_texture(texture);
    renderer.deinit();
    SDL_Quit();
    return 0;
}
