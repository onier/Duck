//
// Created by xuzhenhai on 2022/4/1.
//

#include "Client.h"
#include "glog/logging.h"
#include <chrono>
#include <thread>

using namespace Duck::WebSocketClient;

Client::Client(std::string url) {
    _url = url;
    _executor = std::make_shared<puppy::common::Executor>(1);
    _sendExecutor = std::make_shared<puppy::common::Executor>(1);
}

void Client::addHandler(std::string type, MessageHandler function) {
    _messageHandler.insert({type, function});
}

bool Client::sendMessage(std::string msg) {
    if (_currentConnection && _isConnected) {
        try {
            const std::error_code &ret = _currentConnection->send(msg);
            if (ret) {
                return true;
            }
        } catch (...) {
            LOG(ERROR) << " send message fail " << msg;
        }
    }
    return false;
}

void Client::setAuthorize(Authorize authorize) {
    _authorize = authorize;
}

void Client::setHeartbeat(Heartbeat heartbeat) {
    _heartBeat = heartbeat;
}

WebSocketHandler::~WebSocketHandler() {

}

void Client::connect() {
    _executor->postTask([&]() {
        initClient();
    });
    using namespace std::chrono_literals;
    while (!_isConnected) {
        std::this_thread::sleep_for(200ms);
    }
}

void Client::addWebSocketHandler(std::shared_ptr<WebSocketHandler> webSocketHandler) {
    webSocketHandler->_webSocketServer = shared_from_this();
    addHandler(webSocketHandler->getType(),
               [webSocketHandler](std::shared_ptr<void> client, nlohmann::json json) -> std::string {
                   webSocketHandler->handMessage(json, client);
               });
}

void Client::sendHeartBeat() {
    _sendExecutor->postTask([&]() {
        if (_heartBeat && _isConnected) {
            sendMessage(_heartBeat());
        }
        _sendExecutor->postTimerTaskSecond([&]() {
            sendHeartBeat();
        }, 1);
    });
}

void Client::initClient() {
    try {
        _isConnected = false;
        _client = std::make_shared<client>();
        // Set logging to be pretty verbose (everything except message payloads)
        _client->set_access_channels(websocketpp::log::alevel::none);
        _client->clear_access_channels(websocketpp::log::alevel::frame_payload);

        _client->init_asio();
        _client->set_reuse_addr(true);
        // Register our message handler
        _client->set_fail_handler([&](websocketpp::connection_hdl hdl) {
            _isConnected = false;
        });
        _client->set_open_handler([&](websocketpp::connection_hdl hdl) {
            _isConnected = true;
            if (_authorize) {
                sendMessage(_authorize());
            }
            sendHeartBeat();
            LOG(INFO) << " socket open";
        });
        _client->set_message_handler([&](websocketpp::connection_hdl hdl, message_ptr msg) {
            auto json = nlohmann::json::parse(msg->get_payload());
            auto res = _messageHandler[json.at("type").get<std::string>()](hdl.lock(), json);
            if (!res.empty()) {
                sendMessage(res);
            }
        });

        websocketpp::lib::error_code ec;
        _currentConnection = _client->get_connection(_url, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        _client->connect(_currentConnection);
        _client->run();
        LOG(ERROR) << "connect fail";
        _executor->postTimerTaskSecond([&]() {
            initClient();
        }, 1);
    } catch (...) {

    }
}
