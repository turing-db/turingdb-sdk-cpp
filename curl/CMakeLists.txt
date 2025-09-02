set(curl_client_sources STATIC
    CurlClient.cpp
    CurlRequest.cpp
    CurlClientResult.cpp)

add_library(turing_client_cpp_curl ${curl_client_sources})

target_include_directories(turing_client_cpp_curl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(turing_client_cpp_curl PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(turing_client_cpp_curl PRIVATE ${CURL_LIBRARIES})

target_link_libraries(turing_client_cpp_curl PRIVATE 
    turing_common_s
    turing_client_cpp_common_headers)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/turingsdk)

