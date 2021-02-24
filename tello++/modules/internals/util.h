#pragma once

#define get_variable_name(name) #name

#include "socket_base.h"
#include <string>
#include <memory>
#include <regex>

static const std::string whitespace = "";

template<typename ... Args>
std::string format_string(const std::string &to_be_formatted, Args ...arguments)
{
    const char *raw_string_data = to_be_formatted.c_str();

    int format_string_size = snprintf(nullptr, 0, raw_string_data, arguments...) + 1; 

    if(format_string_size <= 0)
    { 
        throw std::runtime_error("Sorry, we couldn't format your string."); 
    }

    std::unique_ptr<char[]> string_buffer = std::make_unique<char[]>(format_string_size); 
    snprintf(string_buffer.get(), format_string_size, raw_string_data, arguments...);

    return std::string(string_buffer.get()); 
}

std::string left_strip(const std::string &to_strip) 
{
    return std::regex_replace(to_strip, std::regex(R"(^\s+)"), whitespace);
}

std::string right_strip(const std::string &to_strip) 
{
    return std::regex_replace(to_strip, std::regex(R"(\s+$)"), whitespace);
}

std::string strip(const std::string &to_strip) 
{
    return left_strip(right_strip(to_strip));
}

void throw_winsock_error(std::string error_message) 
{
    throw std::runtime_error(format_string("%s Winsock Error Code: '%i'", error_message.c_str(), WSAGetLastError()));
}

IN_ADDR encode_ip_address(const INT &ip_address_family, const std::string &ip_address) 
{    
    IN_ADDR converted_ip_address;
    inet_pton(ip_address_family, ip_address.c_str(), &converted_ip_address);
    return converted_ip_address;
}

std::string decode_ip_address(const INT &ip_address_family, IN_ADDR &to_decode, const int &buffer_size) 
{
    std::unique_ptr<char[]> decoded_client_ip = std::make_unique<char[]>(buffer_size);
    inet_ntop(ip_address_family, &to_decode, decoded_client_ip.get(), buffer_size);
    return decoded_client_ip.get(); 
}