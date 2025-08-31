# Thanks GPT for fixing this shit after i accidentally deleted it

cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
project(
  CCSakura
  VERSION 0.3.0
  LANGUAGES CXX)

# Set C standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Add SDL3 and related libraries
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(zstd REQUIRED)

# Add your source files
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM GAME_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS include/*.h)

# Add the game library
add_library(CCSakuraLib STATIC ${GAME_SOURCES})
target_include_directories(CCSakuraLib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(
  CCSakuraLib
  PRIVATE
    SDL3::SDL3
    $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
    SDL3_ttf::SDL3_ttf
    zstd::libzstd)
target_compile_definitions(
  CCSakuraLib
  PRIVATE APPLICATION_NAME="Sakura and the Clow Cards"
          APPLICATION_IDENTIFIER="dev.luny.ccsakura"
          APPLICATION_VERSION="${PROJECT_VERSION}"
          APPLICATION_ORGANIZATION="dev.luny.ccsakura"
          APPLICATION_APP_NAME="Cardcaptor Sakura"
          APPLICATION_ORIGINAL_WIDTH=1200
          APPLICATION_ORIGINAL_HEIGHT=1080)

# Create the game executable
if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)

  set(MACOSX_BUNDLE_ICON_FILE "assets/icon.png")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
  add_executable(CCSakura MACOSX_BUNDLE src/main.cpp)
elseif(WIN32)
  add_executable(CCSakura WIN32 src/main.cpp)
else()
  add_executable(CCSakura src/main.cpp)
endif()

# Don't optimize for debug builds and use O2 for release builds.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(MSVC)
    target_compile_options(CCSakura PRIVATE /W4 /WX /O0 /fsanitize=address)
  else()
    target_compile_options(
      CCSakura
      PRIVATE -Wall
              -Wextra
              -pedantic
              -Werror
              -O0
              -fsanitize=address,undefined
              -fno-omit-frame-pointer)
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
target_link_libraries(
  CCSakura
  PRIVATE
    CCSakuraLib
    SDL3::SDL3
    $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
    SDL3_ttf::SDL3_ttf
    zstd::libzstd)
target_include_directories(CCSakura PRIVATE ${CMAKE_SOURCE_DIR}/include)
