#include "RANSAC.h"

RANSAC::RANSAC(const cv::Mat src, float y_intercept, float slope, const cv::Mat original, int angle)
{
	m_src = src; 
    m_original = original; 
    m_y_intercept = y_intercept; 
    m_slope = slope; 
    m_angle = angle; 
}

RANSAC::~RANSAC()
{

}

//Main algorithm
void RANSAC::algorithm()
{
    //create dataset
    this->create_data(); 

    //initialize variables
    int ran_numx, ran_numy;
    int iterations = 0; 
    int max_inliers = 0; 
    int pt3_x, pt3_y;
    float a, b, abs;
    estimatedSlope = 0; 
    estimatedIntercept = 0; 
    int confirmed_inlier = 0;
    col_image = m_src;

    

    cv::cvtColor(col_image, col_image, cv::COLOR_GRAY2BGR);
    if (data_x.size() != 0)
    {
        while (iterations < m_k)
        {
            //Create Random dataset
            for (int i = 0; i < 2; i++)
            {
                ran_numx = rand() % data_x.size();
                ran_numy = rand() % data_y.size();
                ran_datax.push_back(data_x[ran_numx]);
                ran_datay.push_back(data_y[ran_numy]);
            }
            //fit random data to a line
            this->linear_regression(a, b);
            cv::Point starting(0, a);
            cv::Point ending(460, 460 * b + a);
            cv::Scalar line_color(255, 0, 0);
            cv::line(col_image, starting, ending, line_color, 2);
            
            //Calculate inliers
            confirmed_inlier = 0;
            for (int j = 0; j < data_x.size(); j++)
            {
                pt3_x = data_x[j];
                pt3_y = data_y[j];
                abs = (std::abs(b * pt3_x - pt3_y + a)) / (sqrt(b * b + 1));
                if (abs <= m_t)
                {
                    confirmed_inlier = confirmed_inlier + 1;
                    confirmed_inlierx.push_back(pt3_x);
                    confirmed_inliery.push_back(pt3_y);
                }
            }
            if (confirmed_inlier > max_inliers)
            {
                max_inliers = confirmed_inlier;
                if (estimatedx.size() != 0)
                {
                    estimatedx.clear();
                    estimatedy.clear();
                }
                estimatedx = confirmed_inlierx;
                estimatedy = confirmed_inliery;
                estimatedIntercept = a;
                estimatedSlope = b;
            }
            confirmed_inlierx.clear();
            confirmed_inliery.clear();
            ran_datax.clear();
            ran_datay.clear();

            iterations++;
        }

    }
    
    //Define final axis
    starting.x = 0; 
    starting.y = estimatedIntercept; 
    ending.x = 300; 
    ending.y = 300 * estimatedSlope + estimatedIntercept;
    cv::Scalar line_color(0, 0, 255);
    cv::line(col_image, starting, ending, line_color, 2);
    
    //rotate original image and draw line
    cv::Point pt(m_src.cols / 2, m_src.rows / 2);
    cv::Mat rotation;
    rotation = cv::getRotationMatrix2D(pt, m_angle, 1.0);
    cv::warpAffine(m_original, m_original, rotation, cv::Size(m_src.cols, m_src.rows));
    cv::line(m_original, starting, ending, line_color, 2);

    cv::imshow("RANSAC algorithm 2.0", col_image); 
    cv::imshow("Line in original", m_original); 
  
    //delete vectors
    confirmed_inlierx.clear(); 
    confirmed_inliery.clear(); 
    ran_datax.clear(); 
    ran_datay.clear(); 
    estimatedx.clear(); 
    estimatedy.clear(); 
    data_x.clear(); 
    data_y.clear(); 
}

//Create dataset
void RANSAC::create_data()
{
    if (m_slope != 0 && m_y_intercept != 0) {
        for (int x = 0; x < m_src.cols; x++)
        {
            for (int y = 0; y < m_src.rows; y++)
            {
                int current_y = 1;
                current_y = -(m_slope * x - y);
                if (current_y >= 0 && current_y< m_src.rows)
                {
                    if (m_src.at<float>(current_y, x) > 0)
                    {
                        data_x.push_back(x);
                        data_y.push_back(current_y);
                    }
                }
            }
        }
    }
    else {
        std::cout << "-----------------------In 460 schleife!!------------------- " << std::endl;
        for (int x = 0; x < m_src.cols; x++)
        {
            for (int y = 0; y < m_src.rows; y++)
            {
                if (y >= 0 && y < m_src.rows)
                {
                    if (m_src.at<float>(y, x) > 0)
                    {
                        data_x.push_back(x);
                        data_y.push_back(y); 
                    }
                }
            }
        }
    }
    
}

//Linear regression function
void RANSAC::linear_regression(float &a, float &b)
{
    float sum_x = 0, sum_x2 = 0, sum_y = 0, sum_xy = 0; 
    int n = ran_datax.size(); 

    for (int i = 0; i < ran_datax.size(); i++)
    {
        sum_x = sum_x + ran_datax[i]; 
        sum_x2 = sum_x2 + (ran_datax[i] * ran_datax[i]);
        sum_y = sum_y + ran_datay[i];
        sum_xy = sum_xy + (ran_datax[i] * ran_datay[i]);
    }

    //Calculating a and b
    float meanX = sum_x / n; 
    float meanY = sum_y / n; 

    b = (sum_xy - (n * meanX * meanY)) / (sum_x2 - n * meanX * meanX); 
    a = meanY - (b * meanX);
}