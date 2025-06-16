add_executable(TestClient
               TestClient.cpp)

target_include_directories(TestClient PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(TestClient PRIVATE ${CURL_LIBRARIES})

target_link_libraries(TestClient PRIVATE 
    turing_client_cpp_typed_column
    turing_client_cpp_s)


install(
    TARGETS     TestClient 
    DESTINATION executable/)

