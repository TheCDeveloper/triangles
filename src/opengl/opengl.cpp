#include <triangles/opengl/opengl.hpp>
#include <SDL3/SDL.h>
#include <glad/gl.h>

using namespace TRIANGLES_NAMESPACE;


struct OpenGLRenderer::Internal final {

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

    if (!gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress)) {
        return false;
    }

    return true;
}


void OpenGLRenderer::deinit() {
    if (!initialized_) { return; }
    SDL_DestroyWindow(window_);
}
