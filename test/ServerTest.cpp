/*
 * Copyright (C) 2020  Javier Lancha Vázquez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <chrono>
#include <deque>
#include <thread>

#include "Server.hpp"
#include "net/Socket.hpp"

class StressTestServer : public server::Server {
public:
    StressTestServer() : Server("ServerTest", 4000) {

    }

    bool authenticate(const char* token) override {
        return strcmp(token, "fail") != 0;
    }

    void onLogin(Client& client) override {
        (void) client;
    }

    void onMessageReceived(Client& client, const server::comm::Message& msg) override {
        (void) client;
        (void) msg;
    }
};

void CreateSocket(std::deque<server::net::ClientSocket>* socketList) throw() {
    try {
        server::net::ClientSocket newSocket(
            server::net::Socket::Domain::IPv4,
            server::net::Socket::Type::STREAM,
            "127.0.0.1", 4000
        );

        newSocket.Connect();

        if (socketList != nullptr) {
            socketList->push_back(newSocket);
        }
    }
    catch (server::net::SocketException& exception) {
        throw exception;
    }
}

TEST(ServerTest, StressServer) {
    const int64_t start = std::chrono::duration_cast<std::chrono::seconds>(
                                        std::chrono::system_clock::now()
                                            .time_since_epoch())
                                            .count();
    int64_t now = start;

    StressTestServer stressServer;
    std::thread serverThread(&StressTestServer::run, stressServer);
    serverThread.detach();

    // Let's stress the server
    std::deque<server::net::ClientSocket> logFailSockets;
    std::deque<server::net::ClientSocket> loggedOneSockets;
    std::deque<server::net::ClientSocket> loggedTwoSockets;
    std::deque<server::net::ClientSocket> loggedThreeSockets;

    const char failToken[]  = "fail";
    const char oneToken[]   = "one";
    const char twoToken[]   = "two";
    const char threeToken[] = "three";
    uint8_t failBuffer[16];
    uint8_t oneBuffer[16];
    uint8_t twoBuffer[16];
    uint8_t threeBuffer[16];
    {
    server::comm::Message failLogin(
        server::comm::ServerMessageTypes::LOGIN,
        (uint8_t*) failToken, sizeof(failToken));
    server::comm::Message oneLogin(
        server::comm::ServerMessageTypes::LOGIN,
        (uint8_t*) oneToken, sizeof(oneToken));
    server::comm::Message twoLogin(
        server::comm::ServerMessageTypes::LOGIN,
        (uint8_t*) twoToken, sizeof(twoToken));
    server::comm::Message threeLogin(
        server::comm::ServerMessageTypes::LOGIN,
        (uint8_t*) threeToken, sizeof(threeToken));
    failLogin.serialize(failBuffer, sizeof(failBuffer));
    oneLogin.serialize(oneBuffer, sizeof(oneBuffer));
    twoLogin.serialize(twoBuffer, sizeof(twoBuffer));
    threeLogin.serialize(threeBuffer, sizeof(threeBuffer));
    }

    unsigned int cycles = 0;
    do {
        cycles++;
        std::cout << cycles << " test cycles" << std::endl;
        // Create idle socket

        try {
            CreateSocket(nullptr);
            CreateSocket(&logFailSockets);
            CreateSocket(&loggedOneSockets);
            CreateSocket(&loggedTwoSockets);
            CreateSocket(&loggedThreeSockets);
        }
        catch (server::net::SocketException& exception) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        for (auto& socket : logFailSockets) {
            socket.Send((void*) failBuffer, sizeof(failBuffer));
        }

        for (auto& socket : loggedOneSockets) {
            socket.Send((void*) oneBuffer, sizeof(oneBuffer));
        }

        for (auto& socket : loggedTwoSockets) {
            socket.Send((void*) twoBuffer, sizeof(twoBuffer));
        }

        for (auto& socket : loggedThreeSockets) {
            socket.Send((void*) threeBuffer, sizeof(threeBuffer));
        }

        if (logFailSockets.size() > 50) {
            logFailSockets.front().Close();
            logFailSockets.pop_front();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if (loggedOneSockets.size() > 50) {
            loggedOneSockets.front().Close();
            loggedOneSockets.pop_front();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if (loggedTwoSockets.size() > 50) {
            loggedTwoSockets.front().Close();
            loggedTwoSockets.pop_front();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if (loggedThreeSockets.size() > 50) {
            loggedThreeSockets.front().Close();
            loggedThreeSockets.pop_front();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        now = std::chrono::duration_cast<std::chrono::seconds>(
                                        std::chrono::system_clock::now()
                                            .time_since_epoch())
                                            .count();

    } while((cycles < 100) || ((now - start) < 90));
}
