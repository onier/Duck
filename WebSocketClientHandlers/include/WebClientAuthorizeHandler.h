//
// Created by xuzhenhai on 2022/4/6.
//

#ifndef PUPPY_WEBCLIENTAUTHORIZEHANDLER_H
#define PUPPY_WEBCLIENTAUTHORIZEHANDLER_H

#include "Client.h"
#include "Executor.h"

class WebClientAuthorizeHandler : public Duck::WebSocketClient::WebSocketHandler {
public:
    WebClientAuthorizeHandler();

    ~WebClientAuthorizeHandler() override;

    std::string getType() override;

    std::string handMessage(nlohmann::json json, std::shared_ptr<void> client) override;

    void socketOpen() override;

    void sendHeartBeat();

private:
    std::shared_ptr<puppy::common::Executor> _executor;
    int _timesSecond;
};


#endif //PUPPY_WEBCLIENTAUTHORIZEHANDLER_H
