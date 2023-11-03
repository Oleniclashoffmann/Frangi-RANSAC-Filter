#include "Kalman.h"
#include <cmath>

Kalman::Kalman(float kp, float A, float B, float lambda, float phi)
{
    // Initialize the dynamic model matrix F
    F = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);

    // Initialize the measurement matrix H
    H = (cv::Mat_<float>(2, 4) << 1, 0, 0, 0, 0, 1, 0, 0);

    // Initialize the process noise covariance matrix Q
    cv::Mat Q = (cv::Mat_<float>(4, 4) << kp, 0, 0, 0, 0, kp, 0, 0, 0, 0, kp, 0, 0, 0, 0, kp);

    // Initialize the measurement noise covariance matrix R
    cv::Mat R = cv::Mat::eye(2, 2, CV_32F) * std::pow(A + B * (1 - phi), lambda);

    // Initialize the Kalman filter object
    kalmanFilter = cv::KalmanFilter(4, 2, 0);
    kalmanFilter.transitionMatrix = F;
    kalmanFilter.measurementMatrix = H;
    kalmanFilter.processNoiseCov = Q;
    kalmanFilter.measurementNoiseCov = R;


}

cv::Point2f Kalman::predict()
{
    // Perform the prediction step
    cv::Mat predictedState = kalmanFilter.predict();

    // Extract the predicted position (px, py)
    float predictedPx = predictedState.at<float>(0, 0);
    float predictedPy = predictedState.at<float>(1, 0);

    return cv::Point2f(predictedPx, predictedPy);
}

cv::Point2f Kalman::correct(const cv::Point2f& measuredPosition)
{
    // Perform the correction step using the measured position
    cv::Mat measurement(2, 1, CV_32F);
    measurement.at<float>(0, 0) = measuredPosition.x;
    measurement.at<float>(1, 0) = measuredPosition.y;
    cv::Mat correctedState = kalmanFilter.correct(measurement);

    // Extract the corrected position (px, py)
    float correctedPx = correctedState.at<float>(0, 0);
    float correctedPy = correctedState.at<float>(1, 0);

    return cv::Point2f(correctedPx, correctedPy);
}

void Kalman::setinitvalue(int x, int y)
{
    cv::Mat initialState(4, 1, CV_32F);

    initialState.at<float>(0, 0) = x;
    initialState.at<float>(1, 0) = y;
    initialState.at<float>(2, 0) = 0;
    initialState.at<float>(3, 0) = 0;

    kalmanFilter.statePre = initialState;
    kalmanFilter.statePost = initialState;
}


