//
// Created by xuzhenhai on 2022/4/2.
//

#include "AuthorizeHandler.h"

std::string AuthorizeHandler::getType() {
    return "Authorize";
}

std::string AuthorizeHandler::handMessage(nlohmann::json json, std::shared_ptr<void> client) {
    nlohmann::json jj;
    jj["type"]="Authorize";
    jj["content"]="";
    return jj.dump();
}

