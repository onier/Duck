//
// Created by xuzhenhai on 2022/4/1.
//

#include "Client.h"
#include "glog/logging.h"
#include "folly/futures/Future.h"
#include <chrono>
#include <thread>

Client::Client(std::string url) {
    _url = url;
    _executor = std::make_shared<puppy::common::Executor>(1);
    _sendExecutor = std::make_shared<puppy::common::Executor>(1);
    _executor->postTask([&]() {
        initClient();
    });
    using namespace std::chrono_literals;
    while (!_isConnected) {
        std::this_thread::sleep_for(200ms);
    }
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
            _executor->postTask([&]() {
                initClient();
            });
        });
        _client->set_open_handler([&](websocketpp::connection_hdl hdl) {
            _isConnected = true;
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
    } catch (...) {

    }
}