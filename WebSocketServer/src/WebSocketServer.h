//
// Created by xuzhenhai on 2/21/22.
//

#ifndef PUPPY_WEBSOCKETSERVER_H
#define PUPPY_WEBSOCKETSERVER_H

#include "Server.h"
#include "folly/dynamic.h"
#include "folly/json.h"
#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include "glog/logging.h"

struct Message {
    std::string type;
    std::string content;

    Message(const std::string &type, const std::string &content) : type(type), content(content) {}

    Message() {}

    std::string getType() {
        return type;
    }
};

struct User {
    std::string _userName;
    std::string _level;

    User(const std::string &userName, const std::string &level) : _userName(userName), _level(level) {}

    User() {};

    bool operator!() {
        return _userName.empty();
    }
};

class WebSocketServer : public Server<websocketpp::connection_hdl, Message, User> {
public:
    WebSocketServer(const Decoder &decoder, const Encoder &encoder, const CheckAuth &checkAuth,
                    const DisposeClient &disposeClient);

    WebSocketServer();

private:
    void init();

    Message decoder(std::string text);

    std::string encoder(Message msg);

    User checkAuth(std::string text);

    void disposeClient(websocketpp::connection_hdl client);
};


#endif //PUPPY_WEBSOCKETSERVER_H
