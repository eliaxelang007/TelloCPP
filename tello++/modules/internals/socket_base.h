#pragma once

#include <WS2tcpip.h>
#include <stdexcept>

class SocketBase 
{
    public:
        static SocketBase& get_socket_base() 
        {
            static SocketBase socket_base{};
            return socket_base;
        }

        SocketBase(SocketBase const&) = delete;
        void operator = (SocketBase const&) = delete;

    private:
        SocketBase() 
        {
            WSADATA winsock_data;
            WORD winsock_version = MAKEWORD(2, 2);

            int winsock_initialization_state = WSAStartup(winsock_version, &winsock_data);

            if (winsock_initialization_state != 0) 
            {
                throw std::runtime_error("Sorry, we couldn't initialize Winsock");
            }
        }

        ~SocketBase() 
        {
            WSACleanup();
        }
};