//
// Created by xuzhenhai on 2022/4/6.
//

#ifndef PUPPY_WEBCLIENTAUTHORIZEHANDLER_H
#define PUPPY_WEBCLIENTAUTHORIZEHANDLER_H

#include "Client.h"

class WebClientAuthorizeHandler : public Duck::WebSocketClient::WebSocketHandler {
public:

    ~WebClientAuthorizeHandler() override;

    std::string getType() override;

    std::string handMessage(nlohmann::json json, std::shared_ptr<void> client) override;

    void socketOpen() override;

};


#endif //PUPPY_WEBCLIENTAUTHORIZEHANDLER_H
