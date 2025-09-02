add_library(turing_client_cpp_s STATIC
    TuringClient.cpp
    TuringClientResult.cpp
    JsonUtils.cpp)

target_include_directories(turing_client_cpp_s PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(turing_client_cpp_s PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(turing_client_cpp_s PRIVATE ${CURL_LIBRARIES})
target_link_libraries(turing_client_cpp_s PUBLIC
                    turing_client_cpp_curl
                    turing_common_s
                    nlohmann_json
                    turing_client_cpp_typed_column
                    turing_client_cpp_common_headers)

install(TARGETS turing_client_cpp_s)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/turingsdk)
