#pragma once

#include <memory>

#include "CurlClient.h"
#include "TuringClientResult.h"
#include "CallBackSignatures.h"
#include "TypedColumn.h"

namespace turingClient {

class TuringClient {
public:
    struct callbackReturnValues {
        TuringClientResult<void>& errorResult;
        std::vector<std::string>& stringResult;
    };

    explicit TuringClient(const std::string& url);
    explicit TuringClient(std::string&& url);
    ~TuringClient() = default;

    const std::string& getUrl() { return _url; }

    void setUrl(const std::string& url) { _url = url; }
    void setUrl(std::string&& url) { _url = std::move(url); }

    bool setBearerToken(const std::string& token);
    void removeBearerToken();
    bool setInstanceId(const std::string& instanceId);
    void removeInstanceId();

    TuringClientError& getError() { return _result.error(); }

    bool listLoadedGraphs(std::vector<std::string> &ret);
    bool listAvailableGraphs(std::vector<std::string> &ret);
    bool loadGraph(const std::string &graph);
    bool history(std::vector<std::string> &ret, const std::string &graph,
                 const std::string &commit = "",
                 const std::string &change = "");

    bool query(const std::string &query, const std::string &graph,
               std::vector<std::unique_ptr<TypedColumn>> &ret,
               const std::string &commit = "", const std::string &change = "");

  private:
    static size_t listAvailableGraphsCallBack(char *ptr, size_t size,
                                              size_t nmemb, void *userdata);
    static size_t historyCallBack(char *ptr, size_t size, size_t nmemb,
                                  void *userdata);
    static size_t listLoadedGraphsCallBack(char *ptr, size_t size, size_t nmemb,
                                           void *userdata);
    static size_t loadGraphCallBack(char *ptr, size_t size, size_t nmemb,
                                    void *userdata);
    static size_t queryCallBack(char *ptr, size_t size, size_t nmemb,
                                void *userdata);

    CurlClient& _client;
    CurlRequest* _handle {nullptr};

    std::string _url;
    std::vector<char> _buffer;

    TuringClientResult<void> _result;
};
}
