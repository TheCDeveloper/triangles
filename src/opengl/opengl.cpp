#include <triangles/opengl/opengl.hpp>
#include <vector>
#include <SDL3/SDL.h>
#include <stb_image.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace TRIANGLES_NAMESPACE;


namespace {

const char *VSHADER_SOURCE = R"(
#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_uv;
out vec2 v_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
    gl_Position = u_proj * u_view * u_model * vec4(a_position, 0.0f, 1.0f);
    v_uv = a_uv;
}
)";

const char *FSHADER_SOURCE = R"(
#version 330 core

in vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_image;

void main() {
    frag_color = texture(u_image, v_uv);
}
)";


// TODO: log shader compilation errors
static GLuint create_shader_(GLenum type, const char *const *src) noexcept {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, src, nullptr);
    glCompileShader(shader);
    return shader;
}


struct TextureInfo final {
    bool allocated = false;
    GLuint texture = 0;
};

}


struct OpenGLRenderer::Internal final {
    SDL_GLContext sdl_glc = nullptr;
    GLuint program = 0;
    GLint u_image = 0;
    GLint u_model = 0;
    GLint u_view = 0;
    GLint u_proj = 0;
    GLuint vertex_array = 0;
    GLuint vertex_buffer = 0;

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);

    std::vector<TextureInfo> textures;


    bool init_2d() noexcept;
    void deinit_2d() noexcept;

    void handle_resize(int width, int height);
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

    u_image = glGetUniformLocation(program, "u_image");
    u_model = glGetUniformLocation(program, "u_model");
    u_view = glGetUniformLocation(program, "u_view");
    u_proj = glGetUniformLocation(program, "u_proj");


    const GLfloat vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        false,
        4 * sizeof(GLfloat),
        reinterpret_cast<const void *>(2 * sizeof(GLfloat)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}


void OpenGLRenderer::Internal::deinit_2d() noexcept {
    for (const auto &info : textures) {
        glDeleteTextures(1, &info.texture);
    }

    textures.clear();

    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteProgram(program);
}


void OpenGLRenderer::Internal::handle_resize(int width, int height) {
    glViewport(0, 0, width, height);
    projection_matrix = glm::ortho<float>(0.0f, width, height, 0.0f);
}


OpenGLRenderer::OpenGLRenderer()
    : internal_(new Internal()) {}


OpenGLRenderer::~OpenGLRenderer() {
    deinit();
    delete internal_;
}


bool OpenGLRenderer::init(const InitializationInfo &info) {
    if (initialized_) { return true; }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window_ = SDL_CreateWindow(info.title, info.width, info.height, SDL_WINDOW_OPENGL);

    if (!window_) {
        return false;
    }

    SDL_SetWindowResizable(window_, info.resizable);

    internal_->sdl_glc = SDL_GL_CreateContext(window_);

    if (!internal_->sdl_glc) {
        SDL_DestroyWindow(window_);
        return false;
    }

    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress)) {
        SDL_GL_DestroyContext(internal_->sdl_glc);
        SDL_DestroyWindow(window_);
        return false;
    }

    internal_->handle_resize(info.width, info.height);
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


TextureHandle OpenGLRenderer::create_texture(const char *path) {
    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 4);

    if (!data) {
        return NULL_HANDLE;
    }

    GLenum texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    TextureInfo &info = internal_->textures.emplace_back();
    info.allocated = true;
    info.texture = texture;

    return internal_->textures.size();
}


void OpenGLRenderer::destroy_texture(TextureHandle texture) {
    usize index = texture - 1;

    if (index >= internal_->textures.size()) {
        return;
    }

    TextureInfo &info = internal_->textures[index];

    if (!info.allocated) {
        return;
    }

    glDeleteTextures(1, &info.texture);
    info.allocated = false;

    if (texture == internal_->textures.size()) {
        internal_->textures.pop_back();
    }
}


void OpenGLRenderer::event(const SDL_Event &event) {
    switch (event.type) {
        case SDL_EVENT_WINDOW_RESIZED:
            internal_->handle_resize(event.window.data1, event.window.data2);
            break;

        default:
            break;
    }
}


void OpenGLRenderer::clear(u8 r, u8 g, u8 b, u8 a) noexcept {
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void OpenGLRenderer::present() noexcept {
    SDL_GL_SwapWindow(window_);
}


void OpenGLRenderer::render_texture(TextureHandle handle, f32 x, f32 y, f32 w, f32 h) noexcept {
    glm::mat4 model_matrix =
        glm::translate(glm::mat4(1.0f), {x, y, 0.0f})
        * glm::scale(glm::mat4(1.0f), {w, h, 1.0f});


    glUseProgram(internal_->program);
    glUniform1i(internal_->u_image, 0);
    glUniformMatrix4fv(internal_->u_model, 1, GL_FALSE, &model_matrix[0][0]);
    glUniformMatrix4fv(internal_->u_view, 1, GL_FALSE, &internal_->view_matrix[0][0]);
    glUniformMatrix4fv(internal_->u_proj, 1, GL_FALSE, &internal_->projection_matrix[0][0]);

    // TODO: check texture validity
    glBindTexture(GL_TEXTURE_2D, internal_->textures[handle - 1].texture);
    glBindVertexArray(internal_->vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
