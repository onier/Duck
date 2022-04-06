//
// Created by xuzhenhai on 2022/4/6.
//

#include "HeartBeatHandler.h"

HeartBeatHandler::~HeartBeatHandler() {

}

std::string HeartBeatHandler::getType() {
    return "HeartBeat";
}

std::string HeartBeatHandler::handMessage(nlohmann::json json, std::shared_ptr<void> client) {
    auto  lock = _webSocketServer.lock();
    if(lock){
        auto wlock = lock->_validClients.wlock();
        int n = wlock->size();
        for(int i =0;i<n;i++){
            if(std::get<1>(wlock->at(i))==client){
                using namespace std::chrono;
                std::get<2>(wlock->at(i))=  duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                LOG(INFO)<<"update heatbeat "<< std::get<2>(wlock->at(i));
                return "";
            }
        }
    }
}
