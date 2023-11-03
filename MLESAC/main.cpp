#include "opencv2/opencv.hpp"
#include <iostream>
#include "Frangi.h"
#include "Preprocessing.h"
#include "RANSAC.h"
#include "tip_estimation.h"
#include "Kalman.h"
#include "save_file.h"


int main()
{
    //Image matrix initialization
    cv::Mat original;
    cv::Mat maxVals, whatScale, outAngles, input;
    cv::Mat preprocess; 

    //Variable initialization
    int i = 0; 
    int y_begin; 
    int y_end; 
    int x_end; 
    float slope; 
    float ROI_intercept = 0; 
    float ROI_slope = 0; 
    bool ROI = false; 
    double est_angle = 0;
    int est_x = 0;
    int est_y = 0;

    //rotation angle 
    int angle = 100; 

    //Kalman variable initialization
    float kp = 0.5;
    float A = 1.0;
    float B = 1.0;
    float lambda = 1.0;
    float phi = 1.0;
    Kalman kalman(kp, A, B, lambda, phi);

    bool newinsertion = true;

    while (i < 100)
    {
        //In the used datasets a new insertion starts every 25 images -> therefore the kalman filter has to be reset after 25 images
        if (i % 25 == 0)
        {
            ROI_intercept = 0; 
            ROI_slope = 0; 
            newinsertion = true; 
            ROI = false;

        }
        else
        {
            newinsertion = false;
        }
        if (i == 0)
        {
            newinsertion = true; 
        }

        //Start timer 
        auto start_time = std::chrono::high_resolution_clock::now();

        //initialize image paths 
        std::string path = "C:/Users/ohoff/Documents/Bachelorarbeit/Datensets/Pictures/Liver/liv01_pos01_oth01/";
        path += "name";
        path += std::to_string(i);
        path = path + ".jpg"; 
        const cv::Mat image = cv::imread(path);
        original = image.clone();

        std::cout << "name" << i << std::endl; 

        /////////Preprocessing/////////
        Preprocessing preprocessing(image, angle); 
        preprocess = preprocessing.process(); 
       

        if(ROI = false)
        {
            y_begin = 0;
            slope = 0;
        }
        else {
            y_begin = ROI_intercept; 
            slope = ROI_slope; 
        }

        /////////Frangi Filter/////////
        Frangi neueKlasse(preprocess);
        neueKlasse.frangi2D(preprocess, maxVals, whatScale, outAngles);
        cv::imshow("maxVals", maxVals);
        
        /////////RANSAC/////////
        RANSAC algorithm(maxVals, y_begin, slope , original, angle); //switched since y_end is the smallest value 
        algorithm.algorithm();
        float est_slope = algorithm.ret_est_slope(); 
        float est_intercept = algorithm.ret_est_intercept();
        ROI_intercept = est_intercept; 
        ROI_slope = est_slope; 
        est_angle = atan(est_slope); 
        est_angle = est_angle * (180 / CV_PI); 
        ROI = true; 
 
        //////////Probability Mapping/////////
        tip_estimation new_object(maxVals, est_intercept, -est_slope);// the minus in est_slope depends on insertion angle (- at 5 degrees, + at -5 degrees)
        new_object.probability_func(); 
        est_x = new_object.cor_x();
        est_y = new_object.cor_y(); 

        //Drawing the estimated Point 
        cv::Point center(est_x, est_y); 
        int radius = 5; 
        cv::Point pt(image.cols / 2, image.rows / 2);
        cv::Mat rotation;
        rotation = cv::getRotationMatrix2D(pt, angle, 1.0);
        cv::warpAffine(image, image, rotation, cv::Size(image.cols, image.rows));
        cv::circle(image, center, radius, cv::Scalar(255, 120, 0), 2);
        std::cout << center << std::endl; 

        //Kalman filter
        if (newinsertion == true)
        {
            kalman.setinitvalue(center.x, center.y);
            std::cout << " In newinsertion" << std::endl;
        }
        newinsertion = false;
        cv::Point2f predictedpos = kalman.predict();
        cv::Point2f correctedpos;
        correctedpos = kalman.correct(center);

        //Draw Kalman Point
        cv::circle(image, correctedpos, radius, cv::Scalar(0, 255, 0), 2);
        cv::circle(image, predictedpos, radius, cv::Scalar(0, 0, 255), 2);
        cv::line(image, cv::Point2f(0, est_intercept), cv::Point2f(correctedpos.x, est_slope * correctedpos.x + est_intercept), cv::Scalar(250, 0, 0), 4, 0);
        cv::imshow("original", image);
        
        //end timer
        auto end_time = std::chrono::high_resolution_clock::now(); 

        std::chrono::duration<double> elapsed_time = end_time - start_time;
        std::cout << elapsed_time.count() << std::endl; 
        std::cout << est_angle << std::endl; 
        std::cout << "--------------" << std::endl; 

        //save data
        save_file file;
        file.save("name" + std::to_string(i), correctedpos, elapsed_time, est_angle);
        cv::waitKey(1);
        i++;
    }
    return EXIT_SUCCESS;
}