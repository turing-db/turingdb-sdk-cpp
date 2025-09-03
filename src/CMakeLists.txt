add_library(turing_client_cpp_s STATIC
    turingsdk/TuringClient.cpp
    turingsdk/TuringClientResult.cpp
    turingsdk/JsonUtils.cpp

    turingsdk/curl/CurlClient.cpp
    turingsdk/curl/CurlClientResult.cpp)

target_include_directories(turing_client_cpp_s PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Curl
target_include_directories(turing_client_cpp_s PUBLIC ${CURL_INCLUDE_DIRS})
target_link_libraries(turing_client_cpp_s PUBLIC ${CURL_LIBRARIES})

target_link_libraries(turing_client_cpp_s PUBLIC
                      turing_common_s
                      nlohmann_json)

install(TARGETS turing_client_cpp_s
        LIBRARY DESTINATION lib)

install(DIRECTORY turingsdk DESTINATION include FILES_MATCHING PATTERN "*.h")
