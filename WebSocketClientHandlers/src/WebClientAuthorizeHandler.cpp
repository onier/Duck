//
// Created by xuzhenhai on 2022/4/6.
//

#include "WebClientAuthorizeHandler.h"

WebClientAuthorizeHandler::WebClientAuthorizeHandler() {
    _executor = std::make_shared<puppy::common::Executor>(1);
    _timesSecond = 1;
}

WebClientAuthorizeHandler::~WebClientAuthorizeHandler() {

}

std::string WebClientAuthorizeHandler::getType() {
    return "Authorize";
}

void WebClientAuthorizeHandler::socketOpen() {
    nlohmann::json json;
    json["type"] = "Authorize";
    json["content"] = "";
    auto lock = _webSocketServer.lock();
    if (lock) {
        lock->sendMessage(json.dump());
    }
}

std::string WebClientAuthorizeHandler::handMessage(nlohmann::json json, std::shared_ptr<void> client) {
    sendHeartBeat();
    return "";
}

void WebClientAuthorizeHandler::sendHeartBeat() {
    _executor->postTimerTaskSecond([&]() {
        nlohmann::json hb;
        hb["type"] = "HeartBeat";
        hb["content"] = "";
        auto lock = _webSocketServer.lock();
        if (lock) {
            lock->sendMessage(hb.dump());
        } else {
            return;
        }
        _executor->postTask([&]() {
            sendHeartBeat();
        });
    }, _timesSecond);

}
