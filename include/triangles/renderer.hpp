#pragma once
#include "common.hpp"
#include "types.hpp"

struct SDL_Window;


namespace TRIANGLES_NAMESPACE {

struct InitializationInfo final {
    i32 width = 1280, height = 720;
    const char *title = "Window";
};


/**
 * @brief Renderer interface.
 *
 * Every renderer implements from this.
 * Renderer owns the SDL Window.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;


    /**
     * @brief Initialize renderer.
     *
     * @param info Window creation information.
     * @returns true on successs, otherwise false.
     *
     * @note SDL should be initialized before initialization.
     */
    virtual bool init(const InitializationInfo &info) = 0;

    /**
     * @brief Deinitialize renderer.
     */
    virtual void deinit() = 0;


    /**
     * @breif Clears the screen.
     *
     * Clears the screen with the specified color, opaque black by default.
     *
     * @param r Red channel
     * @param g Green channel
     * @param b Blue channel
     * @param a Alpha channel
     */
    virtual void clear(u8 r=0, u8 g=0, u8 b=0, u8 a=255) = 0;

    /**
     * @breif Presents the rendering.
     *
     * Presents the rendering to the screen/window.
     */
    virtual void present() = 0;


    SDL_Window *window() noexcept { return window_; }
    const SDL_Window *window() const noexcept { return window_; }


protected:
    bool initialized_ = false;
    SDL_Window *window_ = nullptr;
};

} // namespace TRIANGLES_NAMESPACE
