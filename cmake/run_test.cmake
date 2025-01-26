# Run the assembler and capture output.
execute_process(
    COMMAND ${EXECUTABLE} ${INPUT_FILE}
    OUTPUT_VARIABLE ASM_OUTPUT
    ERROR_VARIABLE ASM_ERROR
    RESULT_VARIABLE ASM_RESULT
)

# Detect the expected output file suffix.
get_filename_component(EXTENSION ${EXPECTED_OUTPUT_FILE} EXT)
if (${EXTENSION} STREQUAL ".out")
    set(EXPECTED_RETURN_CODE 0)
elseif(${EXTENSION} STREQUAL ".err")
    set(EXPECTED_RETURN_CODE 1)
else()
    message(FATAL_ERROR "Unexpected extension: ${EXTENSION}")
endif()

if (NOT ASM_RESULT EQUAL EXPECTED_RETURN_CODE)
    message(FATAL_ERROR "Assembler failed for ${INPUT_FILE}: Expected return code ${EXPECTED_RETURN_CODE}, but got ${ASM_RESULT}")
endif()

# Read the expected output file.
file(READ ${EXPECTED_OUTPUT_FILE} EXPECTED_OUTPUT)

# Compare the output with the expected output.
if (ASM_OUTPUT STREQUAL EXPECTED_OUTPUT)
    message(STATUS "Test ${TEST_NAME} passed")
else()
    message(FATAL_ERROR "Test ${TEST_NAME} failed: \nExpected:\n${EXPECTED_OUTPUT}\nbut got:\n${ASM_OUTPUT}")
endif()
