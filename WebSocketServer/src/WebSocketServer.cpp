//
// Created by xuzhenhai on 2022/4/2.
//

#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(int port) {
    _port = port;
}

void WebSocketServer::start() {
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
    _websocketServer = std::make_shared<InternalServer>();
    auto closeClient = [&](std::shared_ptr<void> hdl) {
        websocketpp::lib::error_code ec;
        _websocketServer->close(hdl, websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection ";
        }
    };

    typedef Server<std::shared_ptr<void>, nlohmann::json, nlohmann::json> MyServer;
//    try {
    // Set logging settings
    _websocketServer->set_access_channels(websocketpp::log::alevel::none);
    _websocketServer->clear_access_channels(websocketpp::log::alevel::frame_payload);

    _websocketServer->set_reuse_addr(true);
    // Initialize Asio
    _websocketServer->init_asio();
    // Register our message handler
    _websocketServer->set_open_handler([&](websocketpp::connection_hdl client) {
        LOG(INFO) << " open new client ";
        onNewClient(client.lock());
    });
    _websocketServer->set_message_handler([&](websocketpp::connection_hdl hdl, auto msg) {
        LOG(INFO) << " on message " << msg->get_payload();
        std::string response = onMessage(hdl.lock(), msg->get_payload());
        if (!response.empty()) {
            msg->set_payload(response);
            try {
                _websocketServer->send(hdl, msg);
            } catch (...) {
                websocketpp::lib::error_code ec;
                _websocketServer->close(hdl, websocketpp::close::status::going_away, "", ec);
                if (ec) {
                    std::cout << "> Error closing connection ";
                }
            }

        }
    });
    _websocketServer->set_close_handler([&](websocketpp::connection_hdl client) {
        LOG(INFO) << " close client ";
        onCloseClient(client.lock());
    });
    // Listen on port 9002
    _websocketServer->listen(9002);

    // Start the server accept loop
    _websocketServer->start_accept();

    // Start the ASIO io_service run loop
    _websocketServer->run();

}

