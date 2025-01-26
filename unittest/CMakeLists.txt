include(GoogleTest)

add_executable(assembler_unittests
    parser_test.cpp
    instruction_test.cpp
)

target_link_libraries(assembler_unittests PRIVATE assembler gtest_main)
gtest_discover_tests(assembler_unittests)
