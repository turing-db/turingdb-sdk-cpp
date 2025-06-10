add_executable(TestCli
               TestCli.cpp)

target_include_directories(TestCli PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(TestCli PRIVATE ${CURL_LIBRARIES})

target_link_libraries(TestCli PRIVATE 
    typed_column
    turing_request)


install(
    TARGETS     TestCli 
    DESTINATION executable/)

