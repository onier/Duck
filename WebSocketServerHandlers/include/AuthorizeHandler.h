//
// Created by xuzhenhai on 2022/4/2.
//

#ifndef PUPPY_AUTHORIZEHANDLER_H
#define PUPPY_AUTHORIZEHANDLER_H

#include "WebSocketServer.h"

class AuthorizeHandler : public Duck::WebSocketServer::WebSocketHandler {
public:
    std::string getType() override;

    std::string handMessage(nlohmann::json json, std::shared_ptr<void> client) override;
};


#endif //PUPPY_AUTHORIZEHANDLER_H
