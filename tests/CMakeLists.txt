project(Painter_tests)

add_executable(Painter_tests
        main.cpp

        convert.cpp
        json.cpp
        generation.cpp
        sorting.cpp
        )

target_link_libraries(Painter_tests gtest gtest_main Painter-lib)

enable_testing()
add_test(Tests Painter_tests)