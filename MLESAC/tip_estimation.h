#pragma once
#include "opencv2/opencv.hpp"
#include <fstream>

class tip_estimation
{
public: 
	tip_estimation(cv::Mat source, int y_intercept, float slope);
	~tip_estimation(); 

	void probability_func(); 
	int cor_x() {
		return high_x;
	};
	int cor_y() {
		return high_y;
	};
private: 
	cv::Mat m_src; 
	
	double m_y_intercept; 
	float m_slope; 
	int high_x = 0;
	int high_y = 0; 


	struct PointProbability {
		int x; 
		int y; 
		float probability; 
	};

	static bool compareByProbability(const PointProbability& a, const PointProbability& b)
	{
		return a.probability > b.probability; 
	}

};

