#include <iostream>

// For getopt
#include <unistd.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"

namespace 
{
    //Members

    //Methods
    void printHelp()
    {
        std::cout << "Obstacle avoidance using optical flow" << std::endl;
        std::cout << "USAGE: ./obstacle_avoidance -v [Path to video]" << std::endl; 
    }
}

int main(int argc, char** argv)
{
    // Grab a video feed, default to /dev/video0
    cv::VideoCapture capture(0);

    //Check to see if a video was passed to us
    printHelp();
    return 0;
}