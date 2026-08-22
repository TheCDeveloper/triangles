#pragma once
#include "../renderer.hpp"


namespace TRIANGLES_NAMESPACE {

class OpenGLRenderer final : public IRenderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer();


    bool init(const InitializationInfo &info) override;
    void deinit() override;

    TextureHandle create_texture(const char *path) override;
    void destroy_texture(TextureHandle texture) override;

    void clear(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255) noexcept override;
    void present() noexcept override;

    void render_texture(TextureHandle handle, f32 x, f32 y, f32 w, f32 h) noexcept override;


private:
    struct Internal;
    Internal *internal_;
};

} // namespace TRIANGLES_NAMESPACE
