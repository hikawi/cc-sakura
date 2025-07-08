# Thanks GPT for fixing this shit after i accidentally deleted it

cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
project(CCSakura C ASM)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(PROJECT_VERSION "0.1.0")

# Output directories
set(SDLTTF_VENDORED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Add SDL3 and related libraries
add_subdirectory(vendored/SDL      EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf   EXCLUDE_FROM_ALL)

# Add your source files
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS include/*.h)

# Create the game executable
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    add_executable(CCSakura MACOSX_BUNDLE ${GAME_SOURCES} ${GAME_HEADERS})
elseif(WIN32)
    option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
    add_executable(CCSakura WIN32 ${GAME_SOURCES} ${GAME_HEADERS})
else()
    add_executable(CCSakura ${GAME_SOURCES} ${GAME_HEADERS})
endif()

# Set warnings as error
if(MSVC)
    target_compile_options(CCSakura PRIVATE /W4 /WX)
else()
    target_compile_options(CCSakura PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

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
