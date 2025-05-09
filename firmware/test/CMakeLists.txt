include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

# ===========================

# GTest and GMock do not play nicely with these flags, so disable them
if(NOT MSVC)
        target_compile_options(gtest PRIVATE
                -Wno-ctor-dtor-privacy
                -Wno-missing-include-dirs
                -Wno-sign-promo)
        target_compile_options(gmock PRIVATE -Wno-pedantic)
endif()

function(configure_test testExecutable)
    # Link against gtest library
    target_link_libraries(${testExecutable} gtest gtest_main gmock_main)
    if(NOT MSVC)
        # Disable variadic macro warnings (can be a problem when compiling with Clang)
        target_compile_options(${testExecutable} PRIVATE -Wno-gnu-zero-variadic-macro-arguments)
        # Compile and link with coverage
        target_compile_options(${testExecutable} PRIVATE -O0 --coverage)
        target_link_options(${testExecutable} PRIVATE --coverage)
    endif()
    # Create test name as the capitalized form of the test executable
    string(TOUPPER ${testExecutable} testName)
    # Add executable to test suite
    add_test(${testName} ${testExecutable} ${GTEST_RUN_FLAGS})
endfunction(configure_test)

# ====== Build
set(testsuite ${CMAKE_CURRENT_SOURCE_DIR}/testsuite)
set(mocks_dir ${CMAKE_CURRENT_SOURCE_DIR}/mocks)


# HardwareInterface test
add_executable(piano_leds_test
        ${firmware_src_dir}/PianoLEDs.cpp
        ${mocks_dir}/Arduino.cpp
        ${testsuite}/PianoLEDsTest.cpp)
target_include_directories(piano_leds_test PUBLIC
        ${firmware_src_dir}
        ${testsuite}
        ${mocks_dir})
target_compile_options(piano_leds_test PRIVATE -g -DFW_DEBUG)
configure_test(piano_leds_test)
