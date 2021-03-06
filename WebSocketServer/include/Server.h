//
// Created by xuzhenhai on 2/20/22.
//

#ifndef PUPPY_SERVER_H
#define PUPPY_SERVER_H

#include "functional"
#include "folly/Synchronized.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "vector"
#include "Executor.h"
#include "folly/FBVector.h"
#include "boost/optional.hpp"
#include <chrono>

template<class Client, class Message, class Auth>
struct Server {
    typedef std::function<Message(std::string)> Decoder;
    typedef std::function<std::string(Message)> Encoder;
    typedef std::function<std::string(Message, Client)> MessageHandler;
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
        _loginCheckExecutor = std::make_shared<puppy::common::Executor>(1);
        _enableHeartBeat = true;
        _timeOutSecond = 5;
        _timeOutInterval = 1;
        checkHeartBeat();
    }

    Server() {
        _loginCheckExecutor = std::make_shared<puppy::common::Executor>(3);
        _heartBeatExecutor = std::make_shared<puppy::common::Executor>(3);
        _loginCheckExecutor = std::make_shared<puppy::common::Executor>(1);
        _enableHeartBeat = true;
        _timeOutSecond = 5;
        _timeOutInterval = 1;
        checkHeartBeat();
    }

    virtual ~Server() {

    }

    void onNewClient(Client client) {
        _openConnectClients.wlock()->push_back(client);
        checkClientLogin(client);
    }

    void onCloseClient(Client client) {
        {
            auto lock = _openConnectClients.wlock();
            auto p = std::remove_if(lock->begin(), lock->end(), [client](auto &c) {
                return client == c;
            });
            lock->erase(p);
            LOG(INFO) << "_openConnectClients size " << lock->size();
        }
        {
            auto lock = _validClients.wlock();
            auto p = std::remove_if(lock->begin(), lock->end(), [client](auto &c) {
                return std::get<1>(c) == client;
            });
            if (p) {
                lock->erase(p);
            }
            LOG(INFO) << "_validClients size " << lock->size();
        }
    }

    void addValidClient(Auth auth, Client client) {
        _validClients.wlock()->push_back(std::make_tuple(auth, client, timeSinceEpochMillisec()));
    }

    std::string onMessage(Client client, std::string message) {
        if (!checkClient(client)) {
            LOG(ERROR) << " client check fail ,the client is not login";
            auto auth = _checkAuth(message);
            if (auth.empty()) {
                std::string response = _handlers.rlock()->at("Authorize")(message, client);
                if (response.empty()) {
                    LOG(ERROR) << " dispost error for check fail";
                    closeClient(client);
                    return "";
                } else {
                    _validClients.wlock()->push_back(
                            std::make_tuple(_decoder(response), client, timeSinceEpochMillisec()));
                    return response;
                }
            } else {
                LOG(INFO) << " check user pass add new login user";
                _validClients.wlock()->push_back(std::make_tuple(auth, client, timeSinceEpochMillisec()));
            }
        }

        Message msg = _decoder(message);
//        try {
        std::string type = msg.at("type").template get<std::string>();
        return _handlers.rlock()->at(type)(message, client);
//        } catch (...) {
//            closeClient(client);
//        }
        return "";
    }

    Auth getAuth(Client client) {
        auto lock = _validClients.rlock();
        std::find_if(lock->begin(), lock->end(), [&, client](auto &token) {
            return std::get<2>(token) == client;
        });
    }

    Client getClient(Auth auth) {
        auto lock = _validClients.rlock();
        std::find_if(lock->begin(), lock->end(), [&, auth](auto &token) {
            return std::get<1>(token) == auth;
        });
    }

    void addHandler(std::string type, MessageHandler messageHandler) {
        _handlers.wlock()->insert({type, messageHandler});
    }

private:

    void closeClient(Client client) {
        _heartBeatExecutor->postTask([&, client]() {
            if (_disposeClient) {
                _disposeClient(client);
            }
        });
    }

    uint64_t timeSinceEpochMillisec() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    bool checkClient(Client client) {
//        if (client) {
        auto lock = _validClients.rlock();
        return std::find_if(lock->begin(), lock->end(), [client](auto &c) {
            return std::get<1>(c) == client;
        }) != lock->end();
//        }
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
        LOG(INFO) << " start checkHeartBeat";
        _heartBeatExecutor->postTimerTaskSecond([&]() {
            auto lock = _validClients.wlock();
            auto current = timeSinceEpochMillisec();
            size_t n = lock->size();
            for (int i = 0; i < n; i++) {
                auto &item = lock->at(i);
                long temp = current - std::get<2>(item);
                if (temp > _timeOutSecond * 1000) {
                    if (_enableHeartBeat) {
                        LOG(ERROR) << " close client for timeout";
                        _disposeClient(std::get<1>(item));
                    }
                }
//                else {
//                    std::get<2>(item) = current;
//                }
            }
            _heartBeatExecutor->postTimerTaskSecond([&]() {
                checkHeartBeat();
            });
        }, _timeOutInterval);
    }

public:
    bool _enableHeartBeat;
    int _timeOutSecond;
    int _timeOutInterval;
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
