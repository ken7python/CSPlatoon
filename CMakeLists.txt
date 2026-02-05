cmake_minimum_required(VERSION 3.25)
project(CSplatoon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Raylib (disable extras to keep build small/fast)
set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(CSplatoon MACOSX_BUNDLE src/main.cpp)
target_link_libraries(CSplatoon PRIVATE raylib)

# Default to Release when no build type is specified (single-config generators)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Optimize in Release
target_compile_options(CSplatoon PRIVATE
    $<$<CONFIG:Release>:-O3>
)
target_compile_definitions(CSplatoon PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

# Assets (audio)
file(GLOB ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/assets/*")

# Copy assets next to the executable so relative paths work
add_custom_command(
    TARGET CSplatoon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:CSplatoon>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/assets
            $<TARGET_FILE_DIR:CSplatoon>/assets
)

# macOS .app: bundle assets into Resources as well
if (APPLE)
    set_source_files_properties(${ASSET_FILES}
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/assets"
    )
    target_sources(CSplatoon PRIVATE ${ASSET_FILES})

    add_custom_command(
        TARGET CSplatoon POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_BUNDLE_CONTENT_DIR:CSplatoon>/Resources/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/src/assets
                $<TARGET_BUNDLE_CONTENT_DIR:CSplatoon>/Resources/assets
    )
endif()
