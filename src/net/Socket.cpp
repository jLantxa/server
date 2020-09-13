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

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "net/Socket.hpp"
#include "debug.hpp"

static __attribute_used__ const char* LOG_TAG = "net::Socket";

namespace server {
namespace net {

Connection::Connection(int sockfd) : m_sockfd(sockfd) {
}

Connection::~Connection() {
    Close();
}

ssize_t Connection::Send(void* buffer, std::size_t len) const {
    return send(m_sockfd, buffer, len, 0);
}

ssize_t Connection::Read(void* buffer, std::size_t len, int flags) const {
    return recv(m_sockfd, buffer, len, flags);
}

void Connection::Close() {
    setsockopt(m_sockfd ,SOL_SOCKET, SO_REUSEADDR, (const void*) true, sizeof(int));
}


Socket::Socket(Domain domain, Type type)
:   m_domain(static_cast<int>(domain)),
    m_type(static_cast<int>(type))
{
    m_sockfd = socket(m_domain, m_type, 0);
}


ServerSocket::ServerSocket(Domain domain, Type type, uint16_t port)
:   Socket(domain, type)
{
    Debug::Log::d(LOG_TAG, "%s():", __func__);

    m_port = port;
    m_address.sin_family = m_domain;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);

    int ret = bind(m_sockfd, (struct sockaddr*) &m_address, sizeof(m_address));
    if (ret < 0) {
        throw SocketException(
            SocketException::Action::BIND,
            "Error binding server socket");
    }
    Debug::Log::i(LOG_TAG, "%s(): Created server socket", __func__);
}

void ServerSocket::Listen() {
    int ret = listen(m_sockfd, 0);
    if (ret < 0) {
        throw SocketException(
            SocketException::Action::LISTEN,
            "Server socket could not listen");
    }
}

Connection ServerSocket::Accept() {
    socklen_t socklen = sizeof(m_address);
    int newSockfd;

    static constexpr unsigned MAX_RETRIES = 10;
    unsigned int numTries = 0;

    do {
        newSockfd = accept(m_sockfd, (struct sockaddr*) &m_address, &socklen);
        numTries++;

        if (newSockfd >= 0) {
            Connection connection(newSockfd);
            Debug::Log::i(LOG_TAG, "%s(): Accepted new socket", __func__);
            return connection;
        }
    } while(numTries < MAX_RETRIES);

    throw SocketException(
        SocketException::Action::ACCEPT,
        "Server socket could not accept a connection");
}


ClientSocket::ClientSocket(Domain domain, Type type, std::string address, uint16_t port)
:   Socket(domain, type)
{
    Debug::Log::d(LOG_TAG, "%s():", __func__);

    m_port = port;
    m_address.sin_family = m_domain;
    inet_pton(AF_INET, address.c_str(), &m_address.sin_addr);
    m_address.sin_port = htons(m_port);

    Debug::Log::i(LOG_TAG, "%s(): Created client socket", __func__);
}

void ClientSocket::Connect() {
    int ret = connect(m_sockfd, (struct sockaddr*) &m_address, sizeof(m_address));
    if (ret < 0) {
        throw SocketException(
            SocketException::Action::CONNECT,
            "Error connecting client socket");
    }
}

}  // namespace net
}  // namespace server
