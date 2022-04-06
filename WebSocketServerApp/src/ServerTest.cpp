#include <websocketpp/config/asio_no_tls.hpp>
#include "nlohmann/json.hpp"
#include <websocketpp/server.hpp>

#include <iostream>
#include "glog/logging.h"
#include "chrono"
#include "Server.h"
#include "WebSocketServer.h"
#include "Library.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const &e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

int main() {
    using namespace Duck::WebSocketServer;
    puppy::common::library::loadDefaultLibrary();
    std::shared_ptr<WebSocketServer> webSocketServer = std::make_shared<WebSocketServer>(9002);
    auto values = puppy::common::library::get<WebSocketHandler>("WebSocketServerHandler");
    LOG(INFO) << values.size();
    auto handlers = puppy::common::library::get<WebSocketHandler>("WebSocketServerHandler");
    for (auto &hand : handlers) {
        webSocketServer->addWebSocketHandler(hand);
    }
    webSocketServer->start();
}

int main1() {
    std::shared_ptr<server> echo_server = std::make_shared<server>();
    auto encoder = [](nlohmann::json msg) {
        return msg.dump();
    };
    auto decoder = [](std::string msg) -> nlohmann::json {
        return nlohmann::json::parse(msg);
    };

    auto messageHandler = [](nlohmann::json msg, std::shared_ptr<void> client) {
        return msg.dump();
    };

    auto checkAuth = [](std::string msg) -> nlohmann::json {
        return nlohmann::json::parse(msg);
    };

    auto closeClient = [echo_server](std::shared_ptr<void> hdl) {
        websocketpp::lib::error_code ec;
        echo_server->close(hdl, websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection ";
        }
    };

    typedef Server<std::shared_ptr<void>, nlohmann::json, nlohmann::json> MyServer;
    std::shared_ptr<MyServer> myserver = std::make_shared<MyServer>(decoder, encoder, checkAuth, closeClient);
    myserver->addHandler("Test", messageHandler);
//    try {
    // Set logging settings
    echo_server->set_access_channels(websocketpp::log::alevel::none);
    echo_server->clear_access_channels(websocketpp::log::alevel::frame_payload);

    echo_server->set_reuse_addr(true);
    // Initialize Asio
    echo_server->init_asio();
    // Register our message handler
    echo_server->set_open_handler([myserver](websocketpp::connection_hdl client) {
        LOG(INFO) << " open new client ";
        myserver->onNewClient(client.lock());
    });
    echo_server->set_message_handler([myserver, echo_server](websocketpp::connection_hdl hdl, message_ptr msg) {
        LOG(INFO) << " on message " << msg->get_payload();
        std::string response = myserver->onMessage(hdl.lock(), msg->get_payload());
        if (!response.empty()) {
            msg->set_payload(response);
            try {
                echo_server->send(hdl, msg);
            } catch (...) {
                websocketpp::lib::error_code ec;
                echo_server->close(hdl, websocketpp::close::status::going_away, "", ec);
                if (ec) {
                    std::cout << "> Error closing connection ";
                }
            }

        }
    });
    echo_server->set_close_handler([myserver](websocketpp::connection_hdl client) {
        LOG(INFO) << " close client ";
        myserver->onCloseClient(client.lock());
    });
    // Listen on port 9002
    echo_server->listen(9002);

    // Start the server accept loop
    echo_server->start_accept();

    // Start the ASIO io_service run loop
    echo_server->run();
//    } catch (std::exception const &e) {
//        std::cout << e.what() << std::endl;
//    } catch (...) {
//        std::cout << "other exception" << std::endl;
//    }
}