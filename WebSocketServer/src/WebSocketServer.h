//
// Created by xuzhenhai on 2022/4/2.
//

#ifndef PUPPY_WEBSOCKETSERVER_H
#define PUPPY_WEBSOCKETSERVER_H

#include "Server.h"
#include "nlohmann/json.hpp"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

typedef websocketpp::server<websocketpp::config::asio> InternalServer;

class WebSocketServer : public Server<std::shared_ptr<void>, nlohmann::json, nlohmann::json> {
public:
    WebSocketServer(int port);

    void start();

private:
    int _port;
    std::shared_ptr<InternalServer> _websocketServer;
};


#endif //PUPPY_WEBSOCKETSERVER_H
