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
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "net/Socket.hpp"
#include "debug.hpp"

static __attribute_used__ const char* LOG_TAG = "net::Socket";

namespace server::net {

Connection::Connection(int sockfd) : m_sockfd(sockfd) {
}

void Connection::Send(uint8_t* buffer, std::size_t len) const {
    send(m_sockfd, buffer, len, 0);
}

std::size_t Connection::Read(uint8_t* buffer, std::size_t len) const {
    return read(m_sockfd, buffer, len);
}


Socket::Socket(Domain domain, Type type)
:   m_domain(static_cast<int>(domain)),
    m_type(static_cast<int>(type))
{
    m_sockfd = socket(m_domain, m_type, 0);
}

Socket::~Socket() {
    setsockopt(m_sockfd ,SOL_SOCKET, SO_REUSEADDR, (const void*) true, sizeof(int));
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
        throw SocketException("Error binding server socket");
    }
    Debug::Log::i(LOG_TAG, "%s(): Created server socket", __func__);
}

void ServerSocket::Listen() {
    int ret = listen(m_sockfd, 0);
    if (ret < 0) {
        throw SocketException("Server socket could not listen");
    }
}

Connection ServerSocket::Accept() {
    socklen_t socklen = sizeof(m_address);
    int newSockfd = accept(m_sockfd, (struct sockaddr*) &m_address, &socklen);

    if (newSockfd < 0) {
        throw SocketException("Server socket could not accept a connection");
    } else {
        Connection connection(newSockfd);
        Debug::Log::i(LOG_TAG, "%s(): Accepted new socket", __func__);
        return connection;
    }
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
        throw SocketException("Error connecting client socket");
    }
}

}  // namespace server::net
