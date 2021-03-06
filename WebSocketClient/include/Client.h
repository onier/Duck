//
// Created by xuzhenhai on 2022/4/1.
//

#ifndef PUPPY_CLIENT_H
#define PUPPY_CLIENT_H

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include "Executor.h"
#include <iostream>
#include "nlohmann/json.hpp"
#include "atomic"

namespace Duck {
    namespace WebSocketClient {
        typedef websocketpp::client<websocketpp::config::asio_client> client;

        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;
        using websocketpp::lib::bind;

// pull out the type of messages sent by our config
        typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
        typedef std::function<std::string(std::shared_ptr<void>, nlohmann::json)> MessageHandler;

        typedef std::function<std::string()> Authorize;
        typedef std::function<std::string()> Heartbeat;

        class Client;

        struct WebSocketHandler {
            virtual ~WebSocketHandler();

            virtual std::string getType() {return "";};

//    <std::string(Message, Client)>
            virtual std::string handMessage(nlohmann::json json, std::shared_ptr<void> client) {return "";};

            virtual void socketOpen() {};

            std::weak_ptr<Client> _webSocketServer;
        };

        class Client : public std::enable_shared_from_this<Client> {
        public:
            Client(std::string url);

            void addHandler(std::string type, MessageHandler function);

            bool sendMessage(std::string msg);

            void setAuthorize(Authorize authorize);

            void setHeartbeat(Heartbeat heartbeat);

            void addWebSocketHandler(std::shared_ptr<WebSocketHandler> webSocketHandlers);

            void connect();

            bool isConnected();
        private:
            void initClient();

            void sendHeartBeat();

        private:
            std::shared_ptr<client> _client;
            std::string _url;
            std::vector<std::shared_ptr<WebSocketHandler>> _handlers;
            std::map<std::string, MessageHandler> _messageHandler;
            std::shared_ptr<puppy::common::Executor> _executor;
            std::shared_ptr<puppy::common::Executor> _sendExecutor;
            std::atomic_bool _isConnected;
            client::connection_ptr _currentConnection;
            Authorize _authorize;
            Heartbeat _heartBeat;
        };
    }
}

#endif //PUPPY_CLIENT_H
