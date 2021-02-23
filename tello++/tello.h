#pragma once

#include "modules/tello_socket.h"
#include "modules/tello_logger.h"
#include "modules/internals/util.h"

#include <string>
#include <regex>
#include <memory>

class Tello 
{
    public:
        struct IMUAttitude 
        {
                int pitch;
                int roll;
                int yaw;
        };

        struct IMUAcceleration 
        {
                float x_acceleration;
                float y_acceleration;
                float z_acceleration;
        };

        Tello(  
              const bool &land_on_exit = false, 
              const int &tello_response_timeout_secs = 12,
              const std::string &tello_ip = "192.168.10.1", 
              const int &tello_port = 8889,
              const std::string &tello_client_ip = "0.0.0.0", 
              const int &tello_client_port = 9000,
              const std::string &tello_state_receiver_ip = "0.0.0.0", 
              const int &tello_state_receiver_port = 8890,
              const std::string &tello_video_receiver_ip = "0.0.0.0",
              const int &tello_video_receiver_port = 11111
             )
        {
                this->land_on_exit = land_on_exit;
                this->tello_client = TelloSocket(tello_logger, tello_response_timeout_secs, tello_ip, tello_port, tello_client_ip, tello_client_port);
                this->tello_state_receiver = TelloSocket(tello_logger, tello_response_timeout_secs, tello_ip, tello_port, tello_state_receiver_ip, tello_state_receiver_port);
                this->tello_video_receiver = TelloSocket(tello_logger, tello_response_timeout_secs, tello_ip, tello_port, tello_video_receiver_ip, tello_video_receiver_port);
                
                this->tello_client.send_command("command");

                if (this->land_on_exit) 
                {
                        this->start_flight_time = this->get_flight_time();
                }

                this->start_video_stream();
        }

        std::string takeoff() 
        {
                return  this->tello_client.send_command("takeoff");
        }

        std::string land() 
        {
                return this->tello_client.send_command("land");
        }

        std::string emergency_shutdown() 
        {
                return this->tello_client.send_command("emergency");
        }

        std::string fly_forward(const int &forward_cm) 
        {
                return this->tello_client.send_command(format_string("forward %i", forward_cm));
        }

        std::string fly_backward(const int &backward_cm) 
        {
                return this->tello_client.send_command(format_string("back %i", backward_cm));
        }

        std::string fly_up(const int &up_cm) 
        {
                return this->tello_client.send_command(format_string("up %i", up_cm));
        }

        std::string fly_down(const int &down_cm) 
        {
                return this->tello_client.send_command(format_string("down %i", down_cm));
        }

        std::string fly_left(const int &left_cm) 
        {
                return this->tello_client.send_command(format_string("left %i", left_cm));
        }

        std::string fly_right(const int &right_cm) 
        {
                return this->tello_client.send_command(format_string("right %i", right_cm));
        }

        std::string rotate_clockwise(const int &angle) 
        {
                return this->tello_client.send_command(format_string("cw %i", angle));
        }

        std::string rotate_counterclockwise(const int &angle) 
        {
                return this->tello_client.send_command(format_string("ccw %i", angle));
        }

        std::string front_flip() 
        {
                return this->tello_client.send_command("flip f");
        }

        std::string back_flip() 
        {
                return this->tello_client.send_command("flip b");
        }

        std::string left_flip() 
        {
                return this->tello_client.send_command("flip l");
        }

        std::string right_flip() 
        {
                return this->tello_client.send_command("flip r");
        }

        std::string fly_to_position(const int &x_position, const int &y_position, const int &z_position, const int &speed_cm_per_sec) 
        {
                return this->tello_client.send_command(format_string("go %i %i %i %i", x_position, y_position, z_position, speed_cm_per_sec));
        }

        std::string fly_to_position(const int &x1_position, const int &y1_position, const int &z1_position, int x2_position, const int &y2_position, const int &z2_position, const int &speed_cm_per_sec) 
        {
                return this->tello_client.send_command(format_string("curve %i %i %i %i %i %i %i", x1_position, y1_position, z1_position, x2_position, y2_position, z2_position, speed_cm_per_sec));
        }

        std::string set_speed(const int &speed_cm_per_sec) 
        {
                return this->tello_client.send_command(format_string("speed %i", speed_cm_per_sec));
        }

        std::string set_remote_control(const int &roll, const int &pitch, const int &up_down, const int &yaw) 
        {
                return this->tello_client.send_command(format_string("rc %i %i %i %i", roll, pitch, up_down, yaw));
        }

        std::string change_tello_wifi_info(const std::string &new_tello_wifi_name, const std::string &new_tello_wifi_password) 
        {
                return this->tello_client.send_command(format_string("wifi %s %s", new_tello_wifi_name.c_str(), new_tello_wifi_password.c_str()));
        }

        std::string get_tello_state() 
        {
                return this->tello_state_receiver.receive_data();
        }

        float get_speed() 
        {
                return stof(this->tello_client.send_command("speed?"));
        }

        int get_battery() 
        {
                return stoi(this->tello_client.send_command("battery?"));
        }

        int get_flight_time() 
        {
                const std::regex flight_time_regex(R"((\d+)(\w+))");
                std::smatch matches;

                std::string flight_time_response = this->tello_client.send_command("time?");

                std::regex_search(flight_time_response, matches, flight_time_regex);

                this->tello_logger.log_data(format_string("Tello Flight Time Units: '%s'", matches[2].str().c_str()));

                return std::stoi(matches[1].str());
        }

        int get_height() 
        {
                const std::regex height_regex(R"((\d+)(\w+))");
                std::smatch matches;

                std::string height_response = this->tello_client.send_command("height?");

                regex_search(height_response, matches, height_regex);

                this->tello_logger.log_data(format_string("Tello Height Units: '%s'", matches[2].str().c_str()));

                return std::stoi(matches[1].str());
        }

        float get_average_temprature() 
        {
                const std::regex temprature_regex(R"((\d+)(~)(\d+)(\w+))");
                std::smatch matches;

                std::string temprature_response = this->tello_client.send_command("temp?");

                regex_search(temprature_response, matches, temprature_regex);

                this->tello_logger.log_data(format_string("Tello Temprature Unit: '%s'", matches[4].str().c_str()));

                return (std::stof(matches[1].str()) + std::stof(matches[3].str())) / 2;
        }

        const IMUAttitude get_imu_attitude() 
        {
                IMUAttitude imu_attitude;

                const std::regex imu_attitude_regex(R"((pitch:)(.+)(;roll:)(.+)(;yaw:)(.+)(;))");
                std::smatch matches;

                std::string imu_attitude_reponse = this->tello_client.send_command("attitude?");

                regex_search(imu_attitude_reponse, matches, imu_attitude_regex);

                this->tello_logger.log_data("Tello IMU Attitude Units: Pitch, Roll, Yaw");

                imu_attitude.pitch = std::stoi(matches[2].str());
                imu_attitude.roll = std::stoi(matches[4].str());
                imu_attitude.yaw = std::stoi(matches[6].str());

                return imu_attitude;
        }

        float get_barometer_reading() 
        {
                return std::stof(this->tello_client.send_command("baro?"));
        }

        IMUAcceleration get_imu_acceleration() 
        {
                IMUAcceleration imu_acceleration;

                const std::regex imu_acceleration_regex(R"((agx:)(.+)(;agy:)(.+)(;agz:)(.+)(;))");
                std::smatch matches;

                std::string imu_acceleration_reponse = this->tello_client.send_command("acceleration?");

                std::regex_search(imu_acceleration_reponse, matches, imu_acceleration_regex);

                this->tello_logger.log_data("Tello IMU Acceleration Units: X Acceleration, Y Acceleration, Z Acceleration");

                imu_acceleration.x_acceleration = std::stof(matches[2].str());
                imu_acceleration.y_acceleration = stof(matches[4].str());
                imu_acceleration.z_acceleration = stof(matches[6].str());

                return imu_acceleration;
        }

        float get_distance_from_takeoff() 
        {
                const std::regex distance_from_takeoff_regex(R"((\d+|.)(\w+))");
                std::smatch matches;

                std::string distance_from_takeoff_response = this->tello_client.send_command("tof?");

                std::regex_search(distance_from_takeoff_response, matches, distance_from_takeoff_regex);

                this->tello_logger.log_data(format_string("Tello Distance Form Takeoff Units: '%s'", matches[2].str().c_str()));

                return std::stof(matches[1].str());
        }

        int get_wifi_snr() 
        {
                return std::stoi(this->tello_client.send_command("wifi?"));
        }

        std::string get_video_frame(const int &buffer_size) 
        {
                return this->tello_video_receiver.receive_data(buffer_size);
        }

        ~Tello() 
        {
                try
                {
                        if (this->land_on_exit && this->get_flight_time() > this->start_flight_time) 
                        {
                                this->land();
                        }

                        this->end_video_stream();
                }
                catch(...)
                {

                }
        }

        private:
                int start_flight_time;
                bool land_on_exit;
                TelloLogger tello_logger;
                TelloSocket tello_client;
                TelloSocket tello_state_receiver;
                TelloSocket tello_video_receiver;

                std::string end_video_stream() 
                {         
                        return this->tello_client.send_command("streamoff");
                }

                std::string start_video_stream() 
                {
                        return this->tello_client.send_command("streamon");
                }
};