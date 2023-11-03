#pragma once
#include "opencv2/opencv.hpp"
#include <opencv2/core/cvdef.h>

class Kalman
{
public:

	Kalman(float kp, float A, float B, float lambda, float phi);
	cv::Point2f predict();
	cv::Point2f correct(const cv::Point2f& measuredPosition);
	void setinitvalue(int x, int y);

private:


	cv::Mat F;
	cv::Mat H;

	cv::KalmanFilter kalmanFilter;


};


