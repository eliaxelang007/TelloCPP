#include "tello++/tello.h"
#include <iostream>

int main() 
{
    Tello tello(true);

    for (int i = 0; i < 200; i++) 
    {
        tello.get_video_frame(2048);
    }
}