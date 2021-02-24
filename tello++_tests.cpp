#include <string>
#include <iostream>
#include "tello++/tello.h"

int main() 
{
    Tello tello{true, true};

    tello.takeoff();
    Tello::TelloState tello_state = tello.get_tello_state();
    std::cout << tello_state.imu_attitude.pitch << '\n';
    std::cout << tello_state.imu_attitude.roll << '\n';
    std::cout << tello_state.imu_attitude.yaw << '\n';
    std::cout << tello_state.imu_velocity.x_velocity << '\n';
    std::cout << tello_state.imu_velocity.y_velocity << '\n';
    std::cout << tello_state.imu_velocity.z_velocity << '\n';
    std::cout << tello_state.average_temprature << '\n';
    std::cout << tello_state.distance_from_takeoff << '\n';
    std::cout << tello_state.height << '\n';
    std::cout << tello_state.battery << '\n';
    std::cout << tello_state.barometer_reading << '\n';
    std::cout << tello_state.flight_time << '\n';
    std::cout << tello_state.imu_acceleration.x_acceleration << '\n';
    std::cout << tello_state.imu_acceleration.y_acceleration << '\n';
    std::cout << tello_state.imu_acceleration.z_acceleration << "\n\n";
    std::cout << tello.get_speed() << '\n';
    std::cout << tello.get_battery() << '\n';
    std::cout << tello.get_flight_time() << '\n';
    std::cout << tello.get_height() << '\n';
    std::cout << tello.get_average_temprature() << '\n';
    std::cout << tello.get_imu_attitude().pitch << '\n';
    std::cout << tello.get_barometer_reading() << '\n';
    std::cout << tello.get_imu_acceleration().x_acceleration << '\n';
    std::cout << tello.get_distance_from_takeoff() << '\n';
    std::cout << tello.get_wifi_snr() << '\n';
}