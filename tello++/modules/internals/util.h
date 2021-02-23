#pragma once

#define get_variable_name(name) #name

#include <stdexcept>
#include <string>
#include <memory>
#include <regex>

static const std::string whitespace = "";

template<typename ... Args>
std::string format_string(const std::string &to_be_formatted, Args ...arguments)
{
    int format_string_size = snprintf(nullptr, 0, to_be_formatted.c_str(), arguments...) + 1; 

    if(format_string_size <= 0)
    { 
        throw std::runtime_error("Sorry, we couldn't format your string."); 
    }

    std::unique_ptr<char[]> string_buffer = std::make_unique<char[]>(format_string_size); 
    snprintf(string_buffer.get(), format_string_size, to_be_formatted.c_str(), arguments...);

    return std::string(string_buffer.get(), string_buffer.get() + format_string_size - 1); 
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