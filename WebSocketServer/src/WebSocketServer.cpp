////
//// Created by ubuntu on 2/21/22.
////
//
//#include "WebSocketServer.h"
//
//WebSocketServer::WebSocketServer(const Server<websocketpp::connection_hdl, nlohmann::json , nlohmann::json >::Decoder &decoder,
//                                 const Server<websocketpp::connection_hdl, nlohmann::json , nlohmann::json >::Encoder &encoder,
//                                 const Server<websocketpp::connection_hdl, nlohmann::json , nlohmann::json >::CheckAuth &checkAuth,
//                                 const Server<websocketpp::connection_hdl, nlohmann::json , nlohmann::json >::DisposeClient &disposeClient)
//        : Server(decoder, encoder, checkAuth, disposeClient) {}
//
//WebSocketServer::WebSocketServer() {
//
//}
//
//void WebSocketServer::init() {
//    _decoder = [&](std::string text){
//        return decoder(text);
//    };
//    _encoder = [&](nlohmann::json  msg){
//        return _encoder(msg);
//    };
//    _checkAuth = [&](std::string text){
//        return checkAuth(text);
//    };
//    _disposeClient = [&](auto client){
//        disposeClient(client);
//    };
//}
//
//nlohmann::json  WebSocketServer::decoder(std::string text) {
//    return nlohmann::json::parse(text);
//}
//
//std::string WebSocketServer::encoder(nlohmann::json  msg) {
//    return msg.dump();
//}
//
//nlohmann::json  WebSocketServer::checkAuth(std::string text) {
//    nlohmann::json::parse(text);
//}
//
//void WebSocketServer::disposeClient(websocketpp::connection_hdl client) {
//
//}
