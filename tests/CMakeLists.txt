# Tests CMakeLists.txt for BadCppLib

# Test executables
add_executable(basic_test basic_test.cpp)

# Module tests
add_executable(test_core modules/test_core.cpp)
add_executable(test_result modules/test_result.cpp)
add_executable(test_string_utils modules/test_string_utils.cpp)
add_executable(test_math_utils modules/test_math_utils.cpp)
add_executable(test_file_utils modules/test_file_utils.cpp)
add_executable(test_containers modules/test_containers.cpp)
add_executable(test_time_utils modules/test_time_utils.cpp)
add_executable(test_functional modules/test_functional.cpp)
add_executable(test_debug modules/test_debug.cpp)
add_executable(test_memory modules/test_memory.cpp)
add_executable(test_test_framework modules/test_test_framework.cpp)
add_executable(test_integration modules/test_integration.cpp)

# Link with library
target_link_libraries(basic_test PRIVATE badcpplib)
target_link_libraries(test_core PRIVATE badcpplib)
target_link_libraries(test_result PRIVATE badcpplib)
target_link_libraries(test_string_utils PRIVATE badcpplib)
target_link_libraries(test_math_utils PRIVATE badcpplib)
target_link_libraries(test_file_utils PRIVATE badcpplib)
target_link_libraries(test_containers PRIVATE badcpplib)
target_link_libraries(test_time_utils PRIVATE badcpplib)
target_link_libraries(test_functional PRIVATE badcpplib)
target_link_libraries(test_debug PRIVATE badcpplib)
target_link_libraries(test_memory PRIVATE badcpplib)
target_link_libraries(test_test_framework PRIVATE badcpplib)
target_link_libraries(test_integration PRIVATE badcpplib)

# Add tests to CTest
add_test(NAME BasicTests COMMAND basic_test)
add_test(NAME CoreTests COMMAND test_core)
add_test(NAME ResultTests COMMAND test_result)
add_test(NAME StringUtilsTests COMMAND test_string_utils)
add_test(NAME MathUtilsTests COMMAND test_math_utils)
add_test(NAME FileUtilsTests COMMAND test_file_utils)
add_test(NAME ContainersTests COMMAND test_containers)
add_test(NAME TimeUtilsTests COMMAND test_time_utils)
add_test(NAME FunctionalTests COMMAND test_functional)
add_test(NAME DebugTests COMMAND test_debug)
add_test(NAME MemoryTests COMMAND test_memory)
add_test(NAME TestFrameworkTests COMMAND test_test_framework)
add_test(NAME IntegrationTests COMMAND test_integration)

# Test properties
set_target_properties(basic_test test_core test_result test_string_utils test_math_utils 
    test_file_utils test_containers test_time_utils test_functional test_debug 
    test_memory test_test_framework test_integration PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Target to run tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS basic_test test_core test_result test_string_utils test_math_utils 
            test_file_utils test_containers test_time_utils test_functional test_debug 
            test_memory test_test_framework test_integration
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests"
) 