#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include "nlohmann/json.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;


    websocketpp::lib::error_code ec;
    nlohmann::json json;
    json["type"] = "Test";
    json["content"] = "        // Note that connect here only requests a connection. No network messages are";
    msg->set_payload(json.dump());
    c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}

#include "glog/logging.h"
#include "Client.h"

int main(int argc, char *argv[]) {
    // Create a client endpoint
    client c;

    std::string uri = "ws://localhost:9002";

    Client client1(uri);
    client1.addHandler("Test", [](std::shared_ptr<void> client, nlohmann::json json) -> std::string {
        LOG(INFO)<<" recive "<<json.dump();
        nlohmann::json msg;
        msg["type"] = "Test";
        msg["content"] = "        // Note that connect here only requests a connection. No network messages are";
        return msg.dump();
    });
     sleep(10000);
    if (argc == 2) {
        uri = argv[1];
    }

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::none);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);
        std::thread t([con]() {
            sleep(3);
            nlohmann::json msg;
            msg["type"] = "Test";
            msg["content"] = "        // Note that connect here only requests a connection. No network messages are";
            con->send(msg.dump());
        });
        t.detach();
        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch (websocketpp::exception const &e) {
        std::cout << e.what() << std::endl;
    }
}