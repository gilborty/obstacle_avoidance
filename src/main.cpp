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
    bool add_remove_point = false;
    bool need_to_init = true;
    bool program_is_running = true;
    

    //Handles to frames
    cv::Mat gray;
    cv::Mat frame;
    cv::Mat resized_frame;
    cv::Mat previous_resized_frame;

    //Algorithm options and members
    std::vector<cv::Point2f> features[2];

    cv::TermCriteria term_criteria(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,20,0.03);
    cv::Size subPixWinSize(10,10);
    cv::Size winSize(31,31);

    auto max_count = 500; // Maximum amount of features to track
    auto quality_level = 0.01;
    auto min_dist = 10; //Minimum distance between two features

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
            case 'r':
            {
                need_to_init = true;
                std::cout << "ReInitalizing features to track" << std::endl;
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
            cv::resize(frame, frame, cv::Size(), 0.5, 0.5, cv::INTER_CUBIC);

            // Check if we need to initialize points
            if(need_to_init)
            {   
                cv::goodFeaturesToTrack(resized_frame, features[1], max_count, quality_level, min_dist);
                cv::cornerSubPix(resized_frame, features[1], subPixWinSize, cv::Size(-1,-1), term_criteria);
            }
            else if(!features[0].empty())
            {
                if(previous_resized_frame.empty()) 
                {
                    resized_frame.copyTo(previous_resized_frame);
                }
                
                std::vector<uchar> status;
                std::vector<float> error;

                cv::calcOpticalFlowPyrLK(previous_resized_frame, resized_frame, features[0], features[1], status, error, winSize, 3, term_criteria, 0, 0.001);

                size_t i, k;
                for( i = k = 0; i < features[1].size(); ++i)
                {
                    if(!status[i])
                    {
                        continue;
                    }
                    features[1][k++] = features[1][i];
                    cv::circle(frame, features[1][i], 3, cv::Scalar(0,255,0), -1, 8);
                }
                features[1].resize(k);
            }

            need_to_init = false;

            // Display
            cv::imshow("Input Feed", frame);
            cv::imshow("Resized Frame", resized_frame);

            handle_wait_key(static_cast<char>(cv::waitKey(33)));

            std::swap(features[1],features[0]);
            cv::swap(previous_resized_frame, resized_frame);
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