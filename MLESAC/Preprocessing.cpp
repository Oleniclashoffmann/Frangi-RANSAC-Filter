#include "Preprocessing.h"

Preprocessing::Preprocessing(cv::Mat src, int angle)
{
	m_src = src; 
	m_src_hugh = src; 
	original = src.clone(); 
	m_angle = angle;
}

Preprocessing::~Preprocessing()
{

}

//Preprocessing function -> Gaussian blur, and rotation function
cv::Mat Preprocessing::process()
{

	cv::cvtColor(m_src, m_src, cv::COLOR_BGR2GRAY);
	m_src.convertTo(m_src, CV_32FC1);
	cv::GaussianBlur(m_src, m_src, cv::Size(7, 7), 0, 0);
	cv::threshold(m_src, m_src, m_threshold_value, m_max_binary_value, m_threshold_type);

	cv::Point pt(m_src.cols / 2, m_src.rows / 2); 
	
	cv::Mat rotation; 
	rotation = cv::getRotationMatrix2D(pt, m_angle, 1.0); 
	cv::warpAffine(m_src, m_src, rotation, cv::Size(m_src.cols, m_src.rows));
	m_src_hugh = m_src; 
	return m_src;
}


