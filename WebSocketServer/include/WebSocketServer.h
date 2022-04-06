//
// Created by xuzhenhai on 2022/4/2.
//

#ifndef PUPPY_WEBSOCKETSERVER_H
#define PUPPY_WEBSOCKETSERVER_H

#include "Server.h"
#include "nlohmann/json.hpp"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

namespace Duck {
    namespace WebSocketServer {
        typedef websocketpp::server<websocketpp::config::asio> InternalServer;

        class WebSocketServer;

        struct WebSocketHandler {
            virtual ~WebSocketHandler();

            virtual std::string getType() {};

//    <std::string(Message, Client)>
            virtual std::string handMessage(nlohmann::json json, std::shared_ptr<void> client) {};

            std::weak_ptr<WebSocketServer> _webSocketServer;
        };

    class WebSocketServer : public Server<std::shared_ptr<void>, nlohmann::json, nlohmann::json>, public std::enable_shared_from_this<WebSocketServer> {
        public:
            WebSocketServer(int port);

            void start();

            void addWebSocketHandler(std::shared_ptr<WebSocketHandler> wsh);

        private:
            int _port;
            std::shared_ptr<InternalServer> _websocketServer;
            std::vector<std::shared_ptr<WebSocketHandler>> _webSocketHandlers;
        };
    }
}

#endif //PUPPY_WEBSOCKETSERVER_H
