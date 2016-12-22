#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

//Boost includes
#include "boost/program_options.hpp"

//OpenCV Include
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"

namespace 
{
    //Members
    bool need_to_init = true;
    bool program_is_running = true;
    

    //Handles to frames
    cv::Mat gray;
    cv::Mat frame;
    cv::Mat resized_frame;

    //Algorithm options and members
    std::vector<cv::Point> features_prev;
    std::vector<cv::Point> features_next;

    //Methods

    //Helper to handle wait key events
    void handle_wait_key(const char& key)
    {
        if(key == 27)
        {
            program_is_running = false;
        }

        switch(key)
        {
            case 'q': 
            {
                program_is_running = false;
                break;
            }
        }
    }

    //Helper to convert enum classes to its underlying integral type
    template<typename E>
    constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type 
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }

    //Errors
    enum class ReturnCodes : std::int8_t {
        SUCCESS = 0,
        ERROR_COMMAND_LINE = 1,
        ERROR_UNHANDLED_EXCEPTION = 2,
        ERROR_COULD_NOT_OPEN_VIDEO = 3,
        ERROR_COULD_NOT_GET_NEW_FRAME = 4
    };

} //namespace


int main(int argc, char** argv)
{
    // Define and parse the program options
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "Print the help messages")
        ("video", po::value<std::string>(), "The path to the video file");
    
    po::variables_map vm;
    try 
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        // --help option
        if(vm.count("help"))
        {
            std::cout << "Optical flow for obstacle avoidance" << std::endl;
            std::cout << desc << std::endl;
            return to_integral(ReturnCodes::SUCCESS);
        }

        po::notify(vm); //This throws on an error
    }
    catch(po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return to_integral(ReturnCodes::ERROR_COMMAND_LINE);
    }

    // Start OpenCV code
    try
    {
        // Default to /dev/video0 (usually a webcam)
        cv::VideoCapture capture(0);

        //Check to see if a video file was passed to the command line
        if(vm.count("video"))
        {
            auto video_file = vm["video"].as<std::string>();
            std::cout << "Using video file: " << video_file << std::endl;

            capture = cv::VideoCapture(video_file);
        }

        // Try to open the video
        if( !capture.isOpened() )
        {
            std::cerr << "Could not open capture device. Exiting." << std::endl;
            return to_integral(ReturnCodes::ERROR_COULD_NOT_OPEN_VIDEO);
        }

        // Open up a window to view the frames
        cv::namedWindow("Input Feed", cv::WINDOW_NORMAL);
        cv::namedWindow("Resized Frame", cv::WINDOW_NORMAL);


        while(program_is_running)
        {
            // Get a frame from the camera
            capture >> frame;
            if(frame.empty())
            {
                std::cerr << "Could not get a new frame from the camera. Exiting." << std::endl;
                program_is_running = false;

                return to_integral(ReturnCodes::ERROR_COULD_NOT_GET_NEW_FRAME);
            }

            // Convert to grayscale
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            // Downsample
            // Use a bicubic interpolation and downsample by half (0.5)
            cv::resize(gray, resized_frame, cv::Size(), 0.5, 0.5, cv::INTER_CUBIC);

            // Check if we need to initialize points
            if(need_to_init)
            {   
                cv::
            }

            // Display
            cv::imshow("Input Feed", frame);
            cv::imshow("Resized Frame", resized_frame);

            handle_wait_key(static_cast<char>(cv::waitKey(33)));
        }
    }
   
    catch(std::exception& e)
    {
        std::cerr << "Unhandeled exception reached the top of main: " << e.what() << std::endl;
        std::cerr << "Exiting." << std::endl;
        return to_integral(ReturnCodes::ERROR_UNHANDLED_EXCEPTION);
    }
    
    return to_integral(ReturnCodes::SUCCESS);
}// main