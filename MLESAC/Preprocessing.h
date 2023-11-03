#pragma once
#include "opencv2/opencv.hpp"

class Preprocessing
{
public: 
	Preprocessing(cv::Mat src, int angle); 
	~Preprocessing(); 
	cv::Mat process(); 
	void linedetection(); 

	int y_begin_func() { return y_begin; }; 
	int y_end_func() { return y_end; }; 
	int x_end_func() { return x_end; };
	float slope() { return m_slope;  }
	

private: 

	cv::Mat m_src; 
	cv::Mat dst; 
	cv::Mat m_src_hugh; 
	cv::Mat original; 

	int m_angle; 

	int m_threshold_value = 180;
	int m_threshold_type = 3;
	int const m_max_binary_value = 190;

	int y_begin, y_end, x_end; 
	float m_slope; 
};

