cmake_minimum_required(VERSION 3.19)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)

project(MiniUCD LANGUAGES C)

find_package(MiniUCD)

# PicoTest
include(FetchContent)
FetchContent_Populate(
    picotest
    GIT_REPOSITORY https://github.com/fredericbonnet/picotest.git
    GIT_TAG v1.4.3
)
set(PicoTest_DIR ${picotest_SOURCE_DIR})
find_package(PicoTest REQUIRED)

add_executable(test_miniucd)
target_sources(test_miniucd
    PRIVATE
        main.c
        traverse.c
        hooks.c
        testUcdPropertyAccessors.c
)
target_link_libraries(test_miniucd
    PRIVATE
        MiniUCD::MiniUCD
        PicoTest::PicoTest
)
enable_testing()
picotest_discover_tests(test_miniucd
    TEST_LIST_OPTION "-l"
)
