#pragma once

#include <string>

namespace turingsdk {

class TuringClientConfig {
public:
    TuringClientConfig() = default;
    ~TuringClientConfig() = default;

    void setHostURL(const std::string& hostURL) { _hostURL = hostURL; }
    void setBearerToken(const std::string& bearerToken) { _bearerToken = bearerToken; }
    void setInstanceID(const std::string& instanceID) { _instanceID = instanceID; }

    const std::string& getHostURL() const { return _hostURL; }
    const std::string& getBearerToken() const { return _bearerToken; }
    const std::string& getInstanceID() const { return _instanceID; }

private:
    std::string _hostURL;
    std::string _bearerToken;
    std::string _instanceID;
};

}