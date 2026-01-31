cmake_minimum_required(VERSION 3.25)
project(CSplatoon CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(CSplatoon MACOSX_BUNDLE src/main.cpp)

target_link_libraries(CSplatoon PRIVATE raylib)

# macOS .app の Resources に assets をコピー
add_custom_command(
        TARGET CSplatoon POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_BUNDLE_CONTENT_DIR:CSplatoon>/Resources/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/assets
        $<TARGET_BUNDLE_CONTENT_DIR:CSplatoon>/Resources/assets
)


target_link_libraries(CSplatoon PRIVATE raylib)
