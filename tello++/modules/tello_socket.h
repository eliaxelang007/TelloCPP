#pragma once

#include "internals/socket_base.h"
#include "internals/util.h"

#include "tello_logger.h"

class TelloSocket 
{
    public:
        TelloSocket
        (
        const TelloLogger &tello_logger, 
        const int &tello_response_timeout_secs = 12, 
        const std::string &tello_ip = "192.168.10.1", 
        const int &tello_port = 8889, 
        const std::string &tello_client_ip = "0.0.0.0", 
        const int &tello_client_port = 9000
        ): 
        socket_base{SocketBase::get_socket_base()},
        tello_logger{tello_logger}, 
        simple_tello_address{tello_ip, tello_port},
        tello_address {AF_INET, htons(tello_port), encode_ip_address(AF_INET, tello_ip)},
        tello_client_address {AF_INET, htons(tello_client_port), encode_ip_address(AF_INET, tello_client_ip)},
        tello_client{socket(AF_INET, SOCK_DGRAM, 0)}
        {
            if (this->tello_client == INVALID_SOCKET) 
            {
                throw_winsock_error
                (
                    format_string("Sorry, we couldn't create you an '%s' socket with the protocol '%s'.", get_variable_name(AF_INET), get_variable_name(SOCK_DGRAM))
                );
            }

            int bind_result = bind(this->tello_client, (sockaddr*)&this->tello_client_address, sizeof(this->tello_client_address));


            if(bind_result == SOCKET_ERROR) 
            {
                throw_winsock_error
                (
                    format_string("Sorry, we couldn't bind your socket to '(%s, %i)'.", tello_client_ip.c_str(), tello_client_port)
                );
            }

            DWORD tello_response_timeout = (tello_response_timeout_secs * 1000);
            int set_socket_recv_timeout_result = setsockopt(tello_client, SOL_SOCKET, SO_RCVTIMEO, (char*)&tello_response_timeout, sizeof(tello_response_timeout));
            int set_socket_send_timeout_result = setsockopt(tello_client, SOL_SOCKET, SO_SNDTIMEO, (char*)&tello_response_timeout, sizeof(tello_response_timeout));


            if (set_socket_recv_timeout_result < 0 || set_socket_recv_timeout_result < 0) 
            {
                throw_winsock_error
                (
                    format_string("Sorry, we couldn't set the timeout of your socket to '%i'.", tello_response_timeout_secs)
                );
            }
        }

        std::string send_command(const std::string &to_send, const int &buffer_size = 128) 
        {
            send_data(to_send);
            return receive_data(buffer_size);
        }

        void send_data(const std::string &string_data) 
        {
            const char* raw_string_data = string_data.c_str();
            int send_result = sendto(this->tello_client, raw_string_data, strlen(raw_string_data), 0, (sockaddr*)&this->tello_address, sizeof(this->tello_address));

            if (send_result == SOCKET_ERROR) 
            {
                throw_winsock_error
                (
                    format_string("Sorry, we couldn't send '%s' to to your Tello.", raw_string_data)
                );
            }

            this->tello_logger.log_data
            (
                format_string("Sent '%s' with the size '%i' to '(%s, %i)'", raw_string_data, string_data.size(), simple_tello_address.ip_address.c_str(), simple_tello_address.port)
            );
        }

        std::string receive_data(const int &buffer_size = 200) 
        {
            sockaddr_in client_address;
            int client_byte_size = sizeof(client_address);
            ZeroMemory(&client_address, client_byte_size);

            std::unique_ptr<char[]> data_buffer = std::make_unique<char[]>(buffer_size);
            ZeroMemory(data_buffer.get(), buffer_size);

            int receive_result = recvfrom(this->tello_client, data_buffer.get(), buffer_size, 0, (sockaddr*)&client_address, &client_byte_size);

            if (receive_result == SOCKET_ERROR) 
            {
                throw_winsock_error
                (
                    "Sorry, we couldn't receive data from your Tello."
                );
            }

            std::string decoded_client_ip = decode_ip_address(AF_INET, client_address.sin_addr, 200);
            int decoded_client_port = ntohs(client_address.sin_port);

            if (decoded_client_ip != this->simple_tello_address.ip_address) 
            {
                throw std::runtime_error(format_string("Unknown connection from ip '%s'", decoded_client_ip));
            }

            std::string data_buffer_string(data_buffer.get());

            this->tello_logger.log_data
            (
                format_string("Received '%s' with the size '%i' from '(%s, %i)'", data_buffer_string.c_str(), data_buffer_string.size(), decoded_client_ip.c_str(), decoded_client_port)
            );

            return strip(data_buffer_string);
        }

    private:
        struct SocketAddress 
        {
            std::string ip_address;
            int port;
        };

        const SocketBase &socket_base;
        const TelloLogger &tello_logger;

        SocketAddress simple_tello_address;
        sockaddr_in tello_address;
        sockaddr_in tello_client_address;
        SOCKET tello_client;
};