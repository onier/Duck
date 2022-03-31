#include <websocketpp/config/asio_no_tls.hpp>
#include "nlohmann/json.hpp"
#include <websocketpp/server.hpp>

#include <iostream>
#include "glog/logging.h"
#include "chrono"
#include "Server.h"

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
    std::shared_ptr<server> echo_server = std::make_shared<server>();
    auto decoder = [](nlohmann::json msg) {
        return msg.dump();
    };
    auto encoder = [](std::string msg) -> nlohmann::json {
        return nlohmann::json::parse(msg);
    };

    auto messageHandler = [](nlohmann::json msg,std::shared_ptr<void> client) {
        LOG(INFO) << msg.dump();
    };

    auto checkAuth = [](std::string msg) -> nlohmann::json {
        return nlohmann::json::parse(msg);
    };

    auto closeClient = [echo_server](std::shared_ptr<void> hdl){
        websocketpp::lib::error_code ec;
        echo_server->close(hdl, websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection " ;
        }
    };

    typedef Server<std::shared_ptr<void>, nlohmann::json, nlohmann::json> MyServer;
    std::shared_ptr<MyServer> myserver = std::make_shared<MyServer>(decoder,encoder,checkAuth,closeClient);
    myserver->addHandler("Test",messageHandler);

    try {
        // Set logging settings
        echo_server->set_access_channels(websocketpp::log::alevel::all);
        echo_server->clear_access_channels(websocketpp::log::alevel::frame_payload);

        echo_server->set_reuse_addr(true);
        // Initialize Asio
        echo_server->init_asio();
        // Register our message handler
        echo_server->set_open_handler([myserver](websocketpp::connection_hdl client) {
            myserver->onNewClient(client.lock());
        });
        echo_server->set_message_handler([myserver]( websocketpp::connection_hdl hdl, message_ptr msg){
            myserver->onMessage(hdl.lock(),msg->get_payload());
        });
        echo_server->set_close_handler([myserver](websocketpp::connection_hdl client) {
            myserver->onCloseClient(client.lock());
        });
        // Listen on port 9002
        echo_server->listen(9002);

        // Start the server accept loop
        echo_server->start_accept();

        // Start the ASIO io_service run loop
        echo_server->run();
    } catch (websocketpp::exception const &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
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