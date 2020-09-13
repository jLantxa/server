#include <gtest/gtest.h>

#include <cstdint>

#include <chrono>
#include <thread>

#include "debug.hpp"
#include "net/Socket.hpp"
#include "util/TextUtils.hpp"

using namespace std::chrono_literals;
const std::string localhost = "127.0.0.1";

TEST(SocketTest, Connect) {
    const uint16_t port = 9999;

    server::net::ServerSocket server(
            server::net::Socket::Domain::IPv4,
            server::net::Socket::Type::STREAM,
            port);

    server::net::ClientSocket client0(
        server::net::Socket::Domain::IPv4,
        server::net::Socket::Type::STREAM,
        localhost, port);

    server::net::ClientSocket client1(
        server::net::Socket::Domain::IPv4,
        server::net::Socket::Type::STREAM,
        localhost, port);

    std::thread serverThread([&server](){
        // Expect two clients
        server.Listen();
        server::net::Connection cn0 = server.Accept();
        server::net::Connection cn1 = server.Accept();

        // Send message to clients
        cn0.Send((void*) "Hello client 0!", 16);
        cn1.Send((void*) "Hello client 1!", 16);

        // Read from clients
        std::this_thread::sleep_for(1s);
        char buffer[64];
        cn0.Read((uint8_t*) buffer, 64);
        const std::string msg0(buffer);
        cn1.Read((uint8_t*) buffer, 64);
        const std::string msg1(buffer);
        EXPECT_TRUE(TextUtils::Equals(msg0, "Hello server 0!"));
        EXPECT_TRUE(TextUtils::Equals(msg1, "Hello server 1!"));
    });
    serverThread.detach();

    // Connect clients to server and send message
    client0.Connect();
    client1.Connect();
    client0.Send((void*) "Hello server 0!", 16);
    client1.Send((void*) "Hello server 1!", 16);

    // Read from server
    std::this_thread::sleep_for(2s);
    char buffer[64];
    client0.Read((uint8_t*) buffer, 64);
    const std::string msg0(buffer);
    client1.Read((uint8_t*) buffer, 64);
    const std::string msg1(buffer);
    EXPECT_TRUE(TextUtils::Equals(msg0, "Hello client 0!"));
    EXPECT_TRUE(TextUtils::Equals(msg1, "Hello client 1!"));
}
