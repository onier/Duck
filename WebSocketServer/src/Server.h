//
// Created by ubuntu on 2/20/22.
//

#ifndef PUPPY_SERVER_H
#define PUPPY_SERVER_H

#include "functional"
#include "folly/Synchronized.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "folly/futures/Future.h"
#include "vector"
#include "Executor.h"
#include "folly/FBVector.h"
#include "boost/optional.hpp"
#include <chrono>

template<class Client, class Message, class Auth>
struct Server {
    typedef std::function<Message(std::string)> Decoder;
    typedef std::function<std::string(Message)> Encoder;
    typedef std::function<void(Message, Client)> MessageHandler;
    typedef std::function<Auth(std::string)> CheckAuth;
    typedef std::function<void(Client)> DisposeClient;
    typedef std::tuple<Auth, Client, uint64_t> Token;

    Server(Decoder decoder, Encoder encoder, CheckAuth checkAuth, DisposeClient disposeClient) : _decoder(decoder),
                                                                                                 _encoder(encoder),
                                                                                                 _checkAuth(checkAuth),
                                                                                                 _disposeClient(
                                                                                                         disposeClient) {
        _loginCheckExecutor = std::make_shared<puppy::common::Executor>(3);
        _heartBeatExecutor = std::make_shared<puppy::common::Executor>(3);
    }

    void onNewClient(Client client) {
        _openConnectClients->wlock()->push_back(client);
        checkClientLogin(client);
    }

    void onCloseClient(Client client) {
        {
            auto lock = _openConnectClients->wlock();
            auto p = std::remove_if(lock->begin(), lock->end(), client);
            lock->erase(p);
        }
        {
            auto lock = _validClients->wlock();
            auto p = std::remove_if(lock->begin(), lock->end(), client);
            lock->erase(p);
        }
    }

    void onMessage(Client client, std::string message) {
        if (!checkClient()) {
            LOG(ERROR) << " client check fail ,the client is not login";
            auto auth = _checkAuth(message);
            if (!auth) {
                LOG(ERROR) << " dispost error for check fail";
                closeClient(client);
                return;
            } else {
                LOG(INFO) << " check user pass add new login user";
                _validClients->wlock()->push_back(std::make_tuple(auth, client, timeSinceEpochMillisec()));
            }
        }

        Message msg = _decoder(message);
        try {
            _handlers->rlock()->operator[](msg->getType)(message, client);
        } catch (...) {
            closeClient(client);
        }
    }

    Auth getAuth(Client client){
        auto lock = _validClients->rlock();
        std::find_if(lock->begin(),lock->end(),[&,client](auto & token){
            return token.get<2>()==client;
        });
    }

    Client getClient(Auth auth){
        auto lock = _validClients->rlock();
        std::find_if(lock->begin(),lock->end(),[&,auth](auto & token){
            return token.get<1>()==auth;
        });
    }

private:

    void closeClient(Client client){
        _heartBeatExecutor->postTask([&,client](){
            _disposeClient(client);
        });
    }

    uint64_t timeSinceEpochMillisec() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    bool checkClient(Client client) {
        if (client) {
            auto lock = _validClients->rlock();
            return std::find(lock->begin(), lock->end(), client) != lock->end()
        }
    }

    void checkClientLogin(Client client) {
        _loginCheckExecutor->postTimerTaskSecond([&, client]() {
            //not int  _validClients after 3 second will be dispose
            if (!checkClient(client)) {
                closeClient(client);
            }
        }, 3);
    }

    void checkHeartBeat() {
        _heartBeatExecutor->postTimerTaskWithFixRate([&]() {
            auto lock = _validClients->rlock();
            auto current = timeSinceEpochMillisec();
            size_t n = lock->size();
            for (int i = 0; i < n; i++) {
                if (current - lock->at(i).get<3>() > 3000) {
                    _disposeClient(current - lock->at(i).get<2>());
                }
            }
        }, 1)
    }

    Decoder _decoder;
    Encoder _encoder;
    CheckAuth _checkAuth;
    DisposeClient _disposeClient;
    //open but not check auth client
    folly::Synchronized<folly::fbvector<Client>> _openConnectClients;
    //auth pass clients
    folly::Synchronized<folly::fbvector<Token>> _validClients;
    std::shared_ptr<puppy::common::Executor> _loginCheckExecutor;
    std::shared_ptr<puppy::common::Executor> _heartBeatExecutor;
    folly::Synchronized<std::map<std::string, MessageHandler>> _handlers;

};

#endif //PUPPY_SERVER_H
