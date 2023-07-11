# == Project == #

cmake_minimum_required(VERSION 3.25)
project(DesertTetris)

# == Sub Directories == #

add_subdirectory(external/raylib)

# == Options == #

set(BUILD_EXAMPLES OFF)

# == Executable == #

set(PRELOAD_ASSET_DIR "assets")

add_executable(game
    "src/include/common.h"
    "src/app.cpp"
    "src/main.cpp"
    "src/pixelfont.cpp"
    "src/assets.cpp"
    "src/transitions.cpp"
    "src/simulation.cpp"
    "src/game.cpp"
    "src/animations.cpp"
    "src/intro.cpp"
    "src/ending.cpp"
)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

target_include_directories(game PRIVATE "src/include")
target_precompile_headers(game PUBLIC "src/include/common.h")
target_link_libraries(game PRIVATE raylib)

if (MSVC)
    target_compile_options(game PRIVATE /std:c++20)
else()
    target_compile_options(game PRIVATE -Wall -std=c++20 -Wno-reorder)
endif()

# == Copy Assets == #

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/${PRELOAD_ASSET_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${PRELOAD_ASSET_DIR}
)

add_dependencies(game copy_assets)

# == Emscripten == #

if (EMSCRIPTEN) # emcmake cmake .. -DPLATFORM=Web && cmake --build .
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=c++17 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY  --preload-file ${PRELOAD_ASSET_DIR} --shell-file C:/Users/jakec/Documents/Coding/shell.html")
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${PRELOAD_DIR} --shell-file C:/Users/jakec/Documents/Coding/shell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()
