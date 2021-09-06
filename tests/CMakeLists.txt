project(Painter_tests)

add_executable(Painter_tests
        main.cpp
        color.cpp)

target_link_libraries(Painter_tests gtest gtest_main Painter-lib)

enable_testing()
add_test(Tests Painter_tests)