#pragma once

#include<fstream>
#include<iostream>

class TelloLogger
{
    public:
        TelloLogger(bool &tello_logging) : tello_logging{tello_logging} {}

        void write_log(const std::string &log_location) const
        {
            std::ofstream log_file;
            log_file.open(log_location);
            log_file << log;
            log_file.close();
        }

        void log_data(const std::string &to_log) const
        {
            if(this->tello_logging) 
            {
                std::string log_string_newline = to_log + '\n';
                this->log += log_string_newline;
                std::cout << log_string_newline;
            }
        }

    private:
        bool &tello_logging;
        mutable std::string log;
};