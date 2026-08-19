#pragma once
#include "../renderer.hpp"


namespace TRIANGLES_NAMESPACE {

class OpenGLRenderer final : public IRenderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer();


    bool init(const InitializationInfo &info) override;
    void deinit() override;


private:
    struct Internal;
    Internal *internal_;
};

} // namespace TRIANGLES_NAMESPACE
