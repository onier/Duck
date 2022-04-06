//
// Created by xuzhenhai on 2022/4/6.
//

#include "WebClientAuthorizeHandler.h"

WebClientAuthorizeHandler::~WebClientAuthorizeHandler() {

}

std::string WebClientAuthorizeHandler::getType() {
    return "Authorize";
}

void WebClientAuthorizeHandler::socketOpen() {

}

std::string WebClientAuthorizeHandler::handMessage(nlohmann::json json, std::shared_ptr<void> client) {
    return "";
}
