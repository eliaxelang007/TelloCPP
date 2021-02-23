#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class TelloLogger
{
    public:
        std::string log;

        void tello_logger_initializer(const bool& tello_logging) 
        {
            this->tello_logging = tello_logging;
        }

        void write_log(const std::string &log_location) 
        {
            std::ofstream log_file;
            log_file.open(log_location);
            log_file << log;
            log_file.close();
        }

        void log_data(const std::string &log_string)
        {
            if (tello_logging)
            {
                std::string log_string_n = log_string + '\n';
                this->log += (log_string_n);
                std::cout << log_string_n;
            }
        }

        TelloLogger() = default;
    
    private:
        bool tello_logging;
};