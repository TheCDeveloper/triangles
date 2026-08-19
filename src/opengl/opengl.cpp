#include <triangles/opengl/opengl.hpp>
#include <SDL3/SDL.h>
#include <glad/gl.h>

using namespace TRIANGLES_NAMESPACE;


namespace {

const char *VSHADER_SOURCE = R"(
#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec3 a_color;
out vec3 v_color;

void main() {
    gl_Position = vec4(a_position, 0.0f, 1.0f);
    v_color = a_color;
}
)";

const char *FSHADER_SOURCE = R"(
#version 330 core

in vec3 v_color;
out vec4 frag_color;

void main() {
    frag_color = vec4(v_color, 1.0f);
}
)";


// TODO: log shader compilation errors
static GLuint create_shader_(GLenum type, const char *const *src) noexcept {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, src, nullptr);
    glCompileShader(shader);
    return shader;
}

}


struct OpenGLRenderer::Internal final {
    SDL_GLContext sdl_glc = nullptr;
    GLuint program = 0;
    GLuint vertex_array = 0;
    GLuint vertex_buffer = 0;


    bool init_2d() noexcept;
    void deinit_2d() noexcept;
};


// TODO: error handling
bool OpenGLRenderer::Internal::init_2d() noexcept {
    program = glCreateProgram();

    GLuint vshader = create_shader_(GL_VERTEX_SHADER, &VSHADER_SOURCE);
    GLuint fshader = create_shader_(GL_FRAGMENT_SHADER, &FSHADER_SOURCE);

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);


    const GLfloat vertices[] = {
        0.0, 0.5, 1, 0, 0,
        -0.5, -0.5, 0, 1, 0,
        0.5, -0.5, 0, 0, 1
    };

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 15 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 5 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        false,
        5 * sizeof(GLfloat),
        reinterpret_cast<const void *>(2 * sizeof(GLfloat)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}


void OpenGLRenderer::Internal::deinit_2d() noexcept {
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteProgram(program);
}


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

    internal_->init_2d();

    initialized_ = true;
    return true;
}


void OpenGLRenderer::deinit() {
    if (!initialized_) { return; }

    internal_->deinit_2d();
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
