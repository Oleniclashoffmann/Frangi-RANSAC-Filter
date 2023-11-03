#include "tip_estimation.h"

tip_estimation::tip_estimation(cv::Mat source, int y_intercept, float slope)
{
	m_src = source.clone(); 
	
	m_slope = slope; 
	m_y_intercept = y_intercept; 


}

tip_estimation::~tip_estimation()
{

}

void tip_estimation::probability_func()
{
	//Variable initialization
	std::vector<int> probabilities;
	int white_unt_point = 0; 
	int black_unt_point = 0; 
	int white_aft_point = 0; 
	int black_aft_point = 0; 
	float prob_1 = 0; 
	float prob_2 = 0; 
	float prob_3 = 0;
	float prob_4 = 0; 
	float prob_N_n_x_y = 0;
	float prob_plus_n_x_y = 0;
	float prob_N_plus_n_x_y = 0;
	float prob_T = 0; 
	std::vector<PointProbability> points;
	int value = 0; 

	//Iteration begin
	if (m_y_intercept != 0)
	{
		//Iterate through whole ROI (+/- 10 pixels as the ROI)
		for (int y = m_y_intercept - 10; y < m_y_intercept + 10; y++)
		{
			for (int x = 0; x < m_src.cols; x++)
			{
				//Since ROI might be angled the real y-coordinate is calculated
				int current_y = 1;
				current_y = -(m_slope * x - y);
				if (current_y >= 0)
				{

					white_unt_point = 0;
					white_aft_point = 0;
					black_aft_point = 0;
					black_unt_point = 0;

					//Second iterations that iterate from 0 to m_src.col to add up pixels
					for (int z = 0; z < m_src.cols; z++)
					{
						int current_z = 1;
						current_z = -(m_slope * z - y);
						
						if(current_z >= 0 && current_z < m_src.rows)
							value = m_src.at<int>(current_z, z);
						if (value > 0)
						{
							//White pixels until point are added up
							if (z < x)
							{
								white_unt_point = white_unt_point + 1;
							}
							//white pixels after point are added up
							else {
								white_aft_point = white_aft_point + 1;
							}
						}
						else {
							//black pixels until point are added up
							if (z < x)
							{
								black_unt_point = black_unt_point + 1;
							}
							//black pixels after point are added up
							else {
								black_aft_point = black_aft_point + 1;
							}
						}

					}

					//probability functions described in BAchelor thesis
					if (white_unt_point + black_unt_point != 0)
						prob_1 = static_cast<float>(white_unt_point) / (white_unt_point + black_unt_point);

					if (white_unt_point + black_unt_point != 0)
						prob_2 = static_cast<float>(black_unt_point) / (white_unt_point + black_unt_point);

					if (white_aft_point + black_aft_point != 0)
						prob_3 = static_cast<float>(white_aft_point) / (white_aft_point + black_aft_point);

					if (white_aft_point + black_aft_point != 0)
						prob_4 = static_cast<float>(black_aft_point) / (white_aft_point + black_aft_point);

					prob_N_n_x_y = (prob_1 + prob_3) * prob_1 + (prob_2 + prob_4) * prob_4;
					prob_plus_n_x_y = (prob_1 + prob_3) * prob_1 + (prob_2 + prob_4) * prob_2;

					if (prob_plus_n_x_y != 0)
						prob_N_plus_n_x_y = (prob_N_n_x_y * prob_1) / prob_plus_n_x_y;

					PointProbability point;
					point.x = x;
					point.y = current_y;
					point.probability = prob_N_plus_n_x_y;

					points.push_back(point);
				}
			}
		}
	}

	//probabilities are sorted 
	std::sort(points.begin(), points.end(), compareByProbability); 
	float propability_value = 0;

	//final value with highest probability is chosen 
	if (!points.empty())
	{
		 const PointProbability& highprob = points[0]; 
		 high_x = highprob.x;
		 high_y = highprob.y; 
		 propability_value = highprob.probability;
	}

	points.clear(); 
}
