#include <iostream>
#include <curl/curl.h>

#include "TuringRequest.h"

size_t func(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::cout.write(ptr, size * nmemb);
    std::cout << std::endl;

    // Return the amount of data actually processed
    return size * nmemb;
}

int main() {
    turingClient::TuringRequest request;
    request.listAvailableGraphs(func);
}
