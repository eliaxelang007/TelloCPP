#pragma once

#include <SdkDdkver.h>

#undef NTDDI_VERSION
#undef WINVER
#undef _WIN32_WINNT

#define NTDDI_VERSION NTDDI_VISTA
#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include "internals/socket_base.h"
#include "internals/util.h"
#include "tello_logger.h"

#include <WS2tcpip.h>
#include <stdexcept>
#include <string>
#include <memory>

class TelloSocket 
{
    public:
        TelloSocket(
                    const TelloLogger& tello_logger, 
                    const int &tello_response_timeout_secs = 12, 
                    const std::string &tello_ip = "192.168.10.1", 
                    const int &tello_port = 8889, 
                    const std::string &tello_client_ip = "0.0.0.0", 
                    const int &tello_client_port = 9000 
                    )
        {
            const SocketBase &socket_base = SocketBase::get_socket_base();
            this->tello_logger = &tello_logger;

            this->simple_tello_address.ip_address = tello_ip;
            this->simple_tello_address.port = tello_port;

            this->tello_address.sin_family = AF_INET;
            inet_pton(tello_address.sin_family, tello_ip.c_str(), &this->tello_address.sin_addr);
            this->tello_address.sin_port = htons(tello_port);

            this->tello_client_address.sin_family = AF_INET;
            inet_pton(tello_client_address.sin_family, tello_client_ip.c_str(), &this->tello_client_address.sin_addr);
            this->tello_client_address.sin_port = htons(tello_client_port);

            this->tello_client = socket(AF_INET, SOCK_DGRAM, 0);

            if (this->tello_client == INVALID_SOCKET) 
            {
                throw std::runtime_error
                    (
                        format_string("Sorry, we couldn't create you an '%s' socket with the protocol '%s'. Winsock Error Code: %i", get_variable_name(AF_INET), get_variable_name(SOCK_DGRAM), WSAGetLastError())
                    );
            }

            int bind_result = bind(this->tello_client, (sockaddr*)&this->tello_client_address, sizeof(this->tello_client_address));

            if(bind_result == SOCKET_ERROR) 
            {
                throw std::runtime_error
                    (
                        format_string("Sorry, we couldn't bind your socket to '(%s, %i)'. Winsock Error Code: %i", tello_client_ip.c_str(), tello_client_port, WSAGetLastError())
                    );
            }

            DWORD tello_response_timeout = (tello_response_timeout_secs * 1000);
            int set_socket_recv_timeout_result = setsockopt(tello_client, SOL_SOCKET, SO_RCVTIMEO, (char*)&tello_response_timeout, sizeof(tello_response_timeout));
            int set_socket_send_timeout_result = setsockopt(tello_client, SOL_SOCKET, SO_SNDTIMEO, (char*)&tello_response_timeout, sizeof(tello_response_timeout));

            if (set_socket_recv_timeout_result < 0 || set_socket_recv_timeout_result < 0) 
            {
                throw std::runtime_error
                    (
                        format_string("Sorry, we couldn't set the timeout of your socket to '%i'. Winsock Error Code: ", tello_response_timeout_secs, WSAGetLastError())
                    );
            }
        }

        std::string send_command(const std::string &command_string) 
        {
            this->send_data(command_string);
            return this->receive_data();
        }

        void send_data(const std::string &string_data) 
        {
            const char* raw_string_data = string_data.c_str();
            int send_result = sendto(this->tello_client, raw_string_data, strlen(raw_string_data), 0, (sockaddr*)&this->tello_address, sizeof(this->tello_address));

            if (send_result == SOCKET_ERROR) 
            {
                throw std::runtime_error
                    (
                        format_string("Sorry, we couldn't send '%s' to to your Tello. Winsock Error Code: %i", raw_string_data, WSAGetLastError())
                    );
            }

            this->*tello_logger.log_data
            (
                format_string("Sent '%s' with the size '%i' to '(%s, %i)'", raw_string_data, string_data.size(), simple_tello_address.ip_address.c_str(), simple_tello_address.port)
            );
        }

        std::string receive_data(const int &buffer_size = 200) 
        {
            sockaddr_in client_address;
            int client_bit_size = sizeof(client_address);
            ZeroMemory(&client_address, client_bit_size);

            std::unique_ptr<char[]> data_buffer = std::make_unique<char[]>(buffer_size);
            ZeroMemory(data_buffer.get(), buffer_size);

            int receive_result = recvfrom(this->tello_client, data_buffer.get(), buffer_size, 0, (sockaddr*)&client_address, &client_bit_size);

            if (receive_result == SOCKET_ERROR) 
            {
                throw std::runtime_error
                    (
                        format_string("Sorry, we couldn't receive data from your Tello. Winsock Error Code: %i", WSAGetLastError())
                    );
            }

            static const int ip_address_string_buffer_size = 200;
            char decoded_client_ip_address[ip_address_string_buffer_size];
            inet_ntop(AF_INET, &client_address.sin_addr, decoded_client_ip_address, ip_address_string_buffer_size);

            int decoded_client_port = ntohs(client_address.sin_port);

            if (decoded_client_ip_address != this->simple_tello_address.ip_address) 
            {
                throw std::runtime_error(format_string("Unknown connection from port '%s'", decoded_client_ip_address));
            }

            std::string data_buffer_string(data_buffer.get());

            data_buffer_string = strip(data_buffer_string);

            this->*tello_logger.log_data
            (
                format_string("Received '%s' with the size '%i' from '(%s, %i)'", data_buffer_string.c_str(), data_buffer_string.size(), this->simple_tello_address.ip_address.c_str(), this->simple_tello_address.port)
            );

            return data_buffer_string;
        }

        ~TelloSocket() 
        {
            closesocket(this->tello_client);
        }

    private:
        struct SocketAddress 
        {
            std::string ip_address;
            int port;
        };

        const TelloLogger *tello_logger;

        SOCKET tello_client;
        SocketAddress simple_tello_address;
        sockaddr_in tello_client_address;
        sockaddr_in tello_address;
};