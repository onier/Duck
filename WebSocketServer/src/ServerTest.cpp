#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include "glog/logging.h"
#include "chrono"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

// Define a callback to handle incoming messages
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const &e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}



int main() {
    auto decoder = [](Message msg) {
        folly::dynamic md;
        md["type"] = msg.type;
        md["content"] = msg.content;
        return folly::toJson(md);
    };
    auto encoder = [](std::string msg) -> Message {
        folly::dynamic data = folly::parseJson(msg);
        return {data["type"].asString(), data["content"].asString()};
    };

    auto messageHandler = [](Message msg, websocketpp::connection_hdl client) {
        LOG(INFO) << msg.type << "  " << msg.content;
    };

    auto checkAuth = [](std::string msg) -> User {
        return {"A","B"};
    };


    std::shared_ptr<MyServe> myServe;
}

int main1() {
    using namespace std::chrono;
    long millisec = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    sleep(3);
    long millisec1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    // Create a server endpoint
    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_open_handler([](websocketpp::connection_hdl client) {
            LOG(INFO) << " socket is open ";
        });
        echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));
        echo_server.set_close_handler([](websocketpp::connection_hdl client) {
            LOG(INFO) << " socket is close ";
        });
        // Listen on port 9002
        echo_server.listen(9002);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}