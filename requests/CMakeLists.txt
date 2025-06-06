add_library(turing_request
    TuringRequest.cpp)

target_include_directories(turing_request PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(turing_request PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(turing_request PRIVATE ${CURL_LIBRARIES})
target_link_libraries(turing_request PUBLIC
                    turing_curl_client
                    turing_common_s
                    nlohmann_json
                    typed_column
                    common_headers)
