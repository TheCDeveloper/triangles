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


    SDL_Window *window() noexcept { return window_; }
    const SDL_Window *window() const noexcept { return window_; }


protected:
    bool initialized_ = false;
    SDL_Window *window_ = nullptr;
};

} // namespace TRIANGLES_NAMESPACE
