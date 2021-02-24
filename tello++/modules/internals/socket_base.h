#pragma once

#undef NTDDI_VERSION
#undef WINVER
#undef _WIN32_WINNT

#define NTDDI_VERSION NTDDI_VISTA
#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

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