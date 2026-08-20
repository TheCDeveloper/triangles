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
 * Defines the interface used by renderers.
 * The renderer owns the SDL Window.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;


    /**
     * @brief Initialize renderer.
     *
     * Initializes the renderer. No-op when the renderer is already initialized.
     *
     * @param info Window creation information.
     * @return true on success, otherwise false.
     *
     * @pre SDL must be initialized.
     */
    virtual bool init(const InitializationInfo &info) = 0;

    /**
     * @brief Deinitialize renderer.
     *
     * Deinitializes the renderer. No-op when the renderer is already deinitialized.
     */
    virtual void deinit() = 0;


    /**
     * @brief Clears the screen.
     *
     * Clears the screen with the specified color, opaque black by default.
     *
     * @param r Red channel
     * @param g Green channel
     * @param b Blue channel
     * @param a Alpha channel
     *
     * @pre Renderer must be initialized.
     */
    virtual void clear(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255) = 0;

    /**
     * @brief Presents the rendering.
     *
     * Presents the rendering to the screen/window.
     *
     * @pre Renderer must be initialized.
     */
    virtual void present() = 0;


    /**
     * @brief Returns the initialization status of the renderer.
     * @return Initialization status.
     */
    bool initialized() const noexcept { return initialized_; }

    /**
     * @brief Returns the SDL window handle.
     * @return Mutable SDL_Window owned by the renderer.
     * @note The pointer is invalid when the renderer is uninitialized.
     */
    SDL_Window *window() noexcept { return window_; }

    /**
     * @brief Returns the SDL window handle.
     * @return Immutable SDL_Window owned by the renderer.
     * @note The pointer is invalid when the renderer is uninitialized.
     */
    const SDL_Window *window() const noexcept { return window_; }


protected:
    bool initialized_ = false;
    SDL_Window *window_ = nullptr;
};

} // namespace TRIANGLES_NAMESPACE
