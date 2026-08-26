cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(Triangles VERSION 0.1.0 LANGUAGES C CXX)

find_package(SDL3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_library(glad STATIC "lib/glad/src/gl.c")
target_include_directories(glad PUBLIC "lib/glad/include")
target_link_libraries(glad PRIVATE OpenGL::GL)

add_library(stb_image STATIC "lib/stb/src/main.c")
target_include_directories(stb_image PUBLIC "lib/stb/include")


file(GLOB TRIANGLES_SOURCES CONFIGURE_DEPENDS "src/opengl/*.cpp")
add_library(triangles STATIC ${TRIANGLES_SOURCES})
target_include_directories(triangles PUBLIC "include")
target_link_libraries(triangles
    PUBLIC SDL3::SDL3
    PRIVATE OpenGL::GL glad stb_image
)


if(BUILD_TESTS)
    file(GLOB TEST_SOURCES "tests/*.cpp")
    add_executable(test ${TEST_SOURCES})
    target_link_libraries(test PRIVATE SDL3::SDL3 triangles)
endif()
