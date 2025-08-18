# Thanks GPT for fixing this shit after i accidentally deleted it

cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
project(CCSakura C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Add SDL3 and related libraries
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

# Add your source files
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS include/*.h)

# Create the game executable
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)

    set(MACOSX_BUNDLE_ICON_FILE "assets/icon.png")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "0.3")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "0.3")
    add_executable(CCSakura MACOSX_BUNDLE ${GAME_SOURCES} ${GAME_HEADERS})
elseif(WIN32)
    add_executable(CCSakura WIN32 ${GAME_SOURCES} ${GAME_HEADERS})
else()
    add_executable(CCSakura ${GAME_SOURCES} ${GAME_HEADERS})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(CCSakura PRIVATE /W4 /WX /O0 /fsanitize=address)
    else()
        target_compile_options(CCSakura PRIVATE -Wall -Wextra -pedantic -Werror -O0 -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(CCSakura PRIVATE -fsanitize=address,undefined)
    endif()
else()
    if(MSVC)
        target_compile_options(CCSakura PRIVATE /W4 /O2)
    else()
        target_compile_options(CCSakura PRIVATE -Wall -Wextra -pedantic -O2)
    endif()
endif()

# Link SDL3 libraries
target_link_libraries(CCSakura PRIVATE
    SDL3::SDL3
    $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
    SDL3_ttf::SDL3_ttf
    ZLIB::ZLIB
)

# Include directories
target_include_directories(CCSakura PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
