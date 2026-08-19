#include <triangles/opengl/opengl.hpp>
#include <SDL3/SDL.h>
#include <glad/gl.h>

using namespace TRIANGLES_NAMESPACE;


struct OpenGLRenderer::Internal final {
    SDL_GLContext sdl_glc;
};


OpenGLRenderer::OpenGLRenderer()
    : internal_(new Internal()) {}


OpenGLRenderer::~OpenGLRenderer() {}


bool OpenGLRenderer::init(const InitializationInfo &info) {
    if (initialized_) { return true; }

    window_ = SDL_CreateWindow(info.title, info.width, info.height, SDL_WINDOW_OPENGL);

    if (!window_) {
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    internal_->sdl_glc = SDL_GL_CreateContext(window_);

    if (!internal_->sdl_glc) {
        SDL_DestroyWindow(window_);
        return false;
    }

    if (!gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress)) {
        SDL_GL_DestroyContext(internal_->sdl_glc);
        SDL_DestroyWindow(window_);
        return false;
    }

    initialized_ = true;
    return true;
}


void OpenGLRenderer::deinit() {
    if (!initialized_) { return; }

    SDL_GL_DestroyContext(internal_->sdl_glc);
    SDL_DestroyWindow(window_);
    initialized_ = false;
}


void OpenGLRenderer::clear(u8 r, u8 g, u8 b, u8 a) noexcept {
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void OpenGLRenderer::present() noexcept {
    SDL_GL_SwapWindow(window_);
}
