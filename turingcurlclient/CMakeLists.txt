set(curl_client_sources
    CurlClient.cpp
    CurlRequest.cpp
    CurlClientResult.cpp)

add_library(turing_curl_client ${curl_client_sources})

target_include_directories(turing_curl_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(turing_curl_client PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(turing_curl_client PRIVATE ${CURL_LIBRARIES})

target_link_libraries(turing_curl_client PRIVATE 
    turing_common_s
    common_headers)

