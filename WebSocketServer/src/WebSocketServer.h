////
//// Created by xuzhenhai on 2/21/22.
////
//
//#ifndef PUPPY_WEBSOCKETSERVER_H
//#define PUPPY_WEBSOCKETSERVER_H
//
//#include "Server.h"
//#include "folly/dynamic.h"
//#include "folly/json.h"
//#include <websocketpp/config/asio_no_tls.hpp>
//
//#include <websocketpp/server.hpp>
//
//#include <iostream>
//#include "glog/logging.h"
//#include "nlohmann/json.hpp"
//
//class WebSocketServer : public Server<websocketpp::connection_hdl, nlohmann::json, nlohmann::json> {
//public:
//    WebSocketServer(const Decoder &decoder, const Encoder &encoder, const CheckAuth &checkAuth,
//                    const DisposeClient &disposeClient);
//
//    WebSocketServer();
//
//private:
//    void init();
//
//    nlohmann::json decoder(std::string text);
//
//    std::string encoder(nlohmann::json msg);
//
//    nlohmann::json checkAuth(std::string text);
//
//    void disposeClient(websocketpp::connection_hdl client);
//};
//
//
//#endif //PUPPY_WEBSOCKETSERVER_H
