add_library(turing_test_utils INTERFACE)

target_link_libraries(turing_test_utils
    INTERFACE GTest::gtest
            gtest_main
            turing_common_s
            spdlog)

function(add_turing_test exe_name cpp_file)
    add_executable(${exe_name} ${cpp_file})
    target_link_libraries(${exe_name} PRIVATE turing_test_utils)
    gtest_discover_tests(${exe_name}
          DISCOVERY_TIMEOUT 10
          DISCOVER_MODE PRE_TEST)
endfunction()
