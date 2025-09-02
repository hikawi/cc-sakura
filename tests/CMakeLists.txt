find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

message(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(CCSakuraTest ${TEST_SOURCES})

target_include_directories(
  CCSakuraTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/
                       ${GTest_INCLUDE_DIRS})
target_link_libraries(
  CCSakuraTest
  PRIVATE
    CCSakuraLib
    SDL3::SDL3
    $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
    SDL3_ttf::SDL3_ttf
    zstd::libzstd
    GTest::gtest
    GTest::gmock)

gtest_discover_tests(CCSakuraTest)
