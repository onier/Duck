//
// Created by xuzhenhai on 2022/4/6.
//

#ifndef PUPPY_HEARTBEATHANDLER_H
#define PUPPY_HEARTBEATHANDLER_H

#include "WebSocketServer.h"

class HeartBeatHandler: public Duck::WebSocketServer::WebSocketHandler {
public:
    ~HeartBeatHandler() override;

    std::string getType() override;

    std::string handMessage(nlohmann::json json, std::shared_ptr<void> client) override;
};


#endif //PUPPY_HEARTBEATHANDLER_H
