//
// Created by ubuntu on 2/21/22.
//

#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(const Server<websocketpp::connection_hdl, Message, User>::Decoder &decoder,
                                 const Server<websocketpp::connection_hdl, Message, User>::Encoder &encoder,
                                 const Server<websocketpp::connection_hdl, Message, User>::CheckAuth &checkAuth,
                                 const Server<websocketpp::connection_hdl, Message, User>::DisposeClient &disposeClient)
        : Server(decoder, encoder, checkAuth, disposeClient) {}

WebSocketServer::WebSocketServer() {

}

void WebSocketServer::init() {
    _decoder = [&](std::string text){
        return decoder(text);
    };
    _encoder = [&](Message msg){
        return _encoder(msg);
    };
    _checkAuth = [&](std::string text){
        return checkAuth(text);
    };
    _disposeClient = [&](auto client){
        disposeClient(client);
    };
}

Message WebSocketServer::decoder(std::string text) {
    auto dynamic = folly::parseJson(text);
    return {dynamic["type"].asString(),dynamic["content"].asString()};
}

std::string WebSocketServer::encoder(Message msg) {
    folly::dynamic dymiac;
    dymiac["type"]=msg.type;
    dymiac["content"]=msg.content;
    return folly::toJson(dymiac);
}

User WebSocketServer::checkAuth(std::string text) {
    auto msg = decoder(text);
    return {msg.type,msg.content};
}

void WebSocketServer::disposeClient(websocketpp::connection_hdl client) {

}
