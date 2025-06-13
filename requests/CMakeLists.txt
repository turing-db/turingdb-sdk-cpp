add_library(turing_client_cpp_request STATIC
    TuringRequest.cpp
    TuringRequestResult.cpp
    JsonUtils.cpp)

target_include_directories(turing_client_cpp_request PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(turing_client_cpp_request PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(turing_client_cpp_request PRIVATE ${CURL_LIBRARIES})
target_link_libraries(turing_client_cpp_request PUBLIC
                    turing_client_cpp_curl
                    turing_common_s
                    nlohmann_json
                    turing_client_cpp_typed_column
                    turing_client_cpp_common_headers)
