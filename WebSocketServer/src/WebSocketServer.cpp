//
// Created by ubuntu on 2/21/22.
//

#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(const Server<websocketpp::connection_hdl, Message, User>::Decoder &decoder,
                                 const Server<websocketpp::connection_hdl, Message, User>::Encoder &encoder,
                                 const Server<websocketpp::connection_hdl, Message, User>::CheckAuth &checkAuth,
                                 const Server<websocketpp::connection_hdl, Message, User>::DisposeClient &disposeClient)
        : Server(decoder, encoder, checkAuth, disposeClient) {}
