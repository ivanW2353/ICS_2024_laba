file(GLOB TEST_INPUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/input/*.in")
foreach(TEST_INPUT_FILE ${TEST_INPUT_FILES})
    # Extract the test name from the input file name.
    get_filename_component(TEST_NAME ${TEST_INPUT_FILE} NAME_WE)
    # Find the corresponding output file.
    set(TEST_EXPECTED_OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/output/${TEST_NAME}.out")

    if (NOT EXISTS ${TEST_EXPECTED_OUTPUT_FILE})
        set(TEST_EXPECTED_OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/output/${TEST_NAME}.err")
    endif()

    # Add a test for the current input file.
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE=${TEST_INPUT_FILE}
            -DEXPECTED_OUTPUT_FILE=${TEST_EXPECTED_OUTPUT_FILE}
            -DEXECUTABLE=$<TARGET_FILE:lc3-assembler>
            -P ${CMAKE_SOURCE_DIR}/cmake/run_test.cmake
    )
endforeach()
