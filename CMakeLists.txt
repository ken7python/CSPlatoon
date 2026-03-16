cmake_minimum_required(VERSION 3.25)
project(CSplatoon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------------------------------------------------------------------
# Platform detection
# emcmake cmake ... を使うと EMSCRIPTEN が自動でセットされる
# ---------------------------------------------------------------------------
if (EMSCRIPTEN)
    set(PLATFORM "Web" CACHE STRING "Raylib platform" FORCE)
endif()

# ---------------------------------------------------------------------------
# Raylib (disable extras to keep build small/fast)
# ---------------------------------------------------------------------------
set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------------------
# Executable — Web と Mac で分岐
# ---------------------------------------------------------------------------
if (EMSCRIPTEN)
    # ------------------------------------------------------------------
    # Web ビルド (Emscripten)
    # ビルド方法:
    #   emcmake cmake -S . -B build-web
    #   cmake --build build-web
    # 成果物: build-web/CSplatoon.html + .js + .wasm + .data
    # ------------------------------------------------------------------
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    add_executable(CSplatoon src/main.cpp)
    target_link_libraries(CSplatoon PRIVATE raylib)
    target_compile_options(CSplatoon PRIVATE -O3)

    # while(!WindowShouldClose()) ループを複数使っているため ASYNCIFY が必要
    # LINK_FLAGS に文字列で渡す（target_link_options はフラグを分割してしまうため使わない）
    set(WASM_LINK_FLAGS
        "-s USE_GLFW=3"
        "-s WASM=1"
        "-s ASYNCIFY=1"
        "-s ASYNCIFY_STACK_SIZE=65536"   # デフォルト4096→64KB: ASYNCIFY の状態保存バッファ不足による再確保を減らす
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s INITIAL_MEMORY=268435456"
        "-s STACK_SIZE=16777216"
        "-s EXPORTED_RUNTIME_METHODS=ccall,cwrap,HEAPF32"
        "-s EXPORTED_FUNCTIONS=_main,_malloc,_free"
        "-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$dynCall"
        "-O3"                            # リンク時最適化 (LTO 相当)
        "--preload-file ${CMAKE_SOURCE_DIR}/src/assets@assets"
        "--shell-file ${CMAKE_SOURCE_DIR}/src/shell.html"
    )
    list(JOIN WASM_LINK_FLAGS " " WASM_LINK_FLAGS_STR)

    set_target_properties(CSplatoon PROPERTIES
        OUTPUT_NAME "index"
        LINK_FLAGS "${WASM_LINK_FLAGS_STR}"
    )

    target_compile_definitions(CSplatoon PRIVATE DOT_SIZE=10)

else()
    # ------------------------------------------------------------------
    # macOS / Windows / Desktop ビルド
    # ビルド方法:
    #   cmake -S . -B build && cmake --build build
    # 成果物(macOS): build/CSplatoon.app
    # 成果物(Windows): build/Release/CSplatoon.exe  (+ assets/)
    # ------------------------------------------------------------------

    if (WIN32)
        # WIN32 フラグ: コンソールウィンドウを表示しない GUI サブシステム
        add_executable(CSplatoon WIN32 src/main.cpp)
    else()
        add_executable(CSplatoon MACOSX_BUNDLE src/main.cpp)
    endif()

    target_link_libraries(CSplatoon PRIVATE raylib)

    # Default to Release when no build type is specified (single-config generators)
    if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    endif()

    # Optimize in Release
    if (MSVC)
        target_compile_options(CSplatoon PRIVATE
            $<$<CONFIG:Release>:/O2 /GL>
        )
        target_link_options(CSplatoon PRIVATE
            $<$<CONFIG:Release>:/LTCG>
        )
    else()
        target_compile_options(CSplatoon PRIVATE
            $<$<CONFIG:Release>:-O3>
        )
    endif()

    target_compile_definitions(CSplatoon PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )

    # Assets
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

endif()
