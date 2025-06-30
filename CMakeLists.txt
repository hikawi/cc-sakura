# Thanks GPT for fixing this shit after i accidentally deleted it

cmake_minimum_required(VERSION 3.21)
project(CCSakura C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Output binary to /bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add SDL3 and related libraries
add_subdirectory(vendored/SDL      EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf   EXCLUDE_FROM_ALL)

# Add your source files
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.c")

# Create the game executable
add_executable(CCSakura ${GAME_SOURCES})

# Link SDL3 libraries
target_link_libraries(CCSakura PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
)

# Include directories
target_include_directories(CCSakura PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendored/SDL/include
    ${CMAKE_SOURCE_DIR}/vendored/SDL_image/include
    ${CMAKE_SOURCE_DIR}/vendored/SDL_mixer/include
    ${CMAKE_SOURCE_DIR}/vendored/SDL_ttf/include
)

# What is this weird thing GPT added
# macOS: fix dynamic library path resolution at runtime
if(APPLE)
    set_target_properties(CCSakura PROPERTIES
        INSTALL_RPATH "@executable_path/../lib"
    )
endif()
