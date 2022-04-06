#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include "nlohmann/json.hpp"

#include "glog/logging.h"
#include "Client.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using namespace Duck::WebSocketClient;
// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

#include "Library.h"

int main(int argc, char *argv[]) {
    puppy::common::library::loadDefaultLibrary();
    std::shared_ptr<Client> webSocketClient = std::make_shared<Client>("ws://localhost:9002");
    auto handlers = puppy::common::library::get<WebSocketHandler>("WebSocketClientHandler");
    for (auto &hand : handlers) {
        webSocketClient->addWebSocketHandler(hand);
    }
    webSocketClient->connect();
    sleep(10000);

}