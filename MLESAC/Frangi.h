#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

class Frangi
{
public: 
	Frangi(const cv::Mat& src);
	~Frangi(); 
	void Hessian2D(const cv::Mat src, cv::Mat& Dxx, cv::Mat& Dxy, cv::Mat& Dyy, float sigma);
	void eig2image(const cv::Mat& Dxx, const cv::Mat& Dxy, const cv::Mat& Dyy, cv::Mat& lambda1, cv::Mat& lambda2, cv::Mat& Ix, cv::Mat& Iy);
	void frangi2D(cv::Mat& src, cv::Mat& maxVals, cv::Mat& whatScale, cv::Mat& outAngles);

private:

	cv::Mat m_src; 

	// Initialization values for sigma (used for scale-space exploration). Sigma defines the scale of structures you're looking for in the image.
	// These variables dictate the starting, ending, and the step increments for sigma during scale-space processing.
	float sigma_start = 1;
	float sigma_end = 2;
	float sigma_step = 1;

	// BetaOne and BetaTwo are constants used in the vesselness measure. They influence the sensitivity to blob-like structures and background noise respectively.
	float BetaOne = 0.3; 
	float BetaTwo = 5;
};