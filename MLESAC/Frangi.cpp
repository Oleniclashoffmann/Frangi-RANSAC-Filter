#include "Frangi.h"
#define PI 3.1415926535897932384626433

// Constructor for Frangi class, initializing source matrix
Frangi::Frangi(const cv::Mat& src)
{
	m_src = src;

}

Frangi::~Frangi()
{

}

// Calculate the 2D Hessian matrix components for an image given a Gaussian filter sigma
void Frangi::Hessian2D(const cv::Mat src, cv::Mat& Dxx, cv::Mat& Dxy, cv::Mat& Dyy, float sigma)
{
	// Calculate kernel size for x and y
	int n_kern_x = 2 * round(3 * sigma) + 1;
	int n_kern_y = n_kern_x;

	float* kern_xx_f = new float[n_kern_x * n_kern_y]();
	float* kern_xy_f = new float[n_kern_x * n_kern_y]();
	float* kern_yy_f = new float[n_kern_x * n_kern_y]();
	int i = 0, j = 0;
	float pointer1, pointer2, pointer3, pointer4;

	// Construct kernels for Hessian components
	for (int x = -round(3 * sigma); x <= round(3 * sigma); x++)
	{
		j = 0;
		for (int y = -round(3 * sigma); y <= round(3 * sigma); y++)
		{
			// Construct the xx component of the Hessian
			pointer1 = 1.0f / (2.0f * PI * sigma * sigma * sigma * sigma);
			pointer2 = (x * x / (sigma * sigma) - 1);
			pointer3 = exp(-(x * x + y * y) / (2.0f * sigma * sigma));
			kern_xx_f[i * n_kern_y + j] = pointer1 * pointer2 * pointer3;

			// Construct the xy component of the Hessian
			pointer1 = 1.0f / (2.0f * PI * sigma * sigma * sigma * sigma * sigma * sigma);
			pointer2 = (x * y);
			pointer3 = exp(-(x * x + y * y) / (2.0f * sigma * sigma));
			kern_xy_f[i * n_kern_y + j] = pointer1 * pointer2 * pointer3;
			j++;
		}
		i++;
	}

	// The yy component can be derived from the xx component due to symmetry
	for (int j = 0; j < n_kern_y; j++)
	{
		for (int i = 0; i < n_kern_x; i++)
		{
			kern_yy_f[j * n_kern_x + i] = kern_xx_f[i * n_kern_x + j];
		}
	}

	// Convert kernel arrays to cv::Mat and flip for convolution
	cv::Mat kern_xx;
	cv::flip(cv::Mat(n_kern_y, n_kern_x, CV_32FC1, kern_xx_f), kern_xx, -1);

	cv::Mat kern_xy;
	cv::flip(cv::Mat(n_kern_y, n_kern_x, CV_32FC1, kern_xy_f), kern_xy, -1);

	cv::Mat kern_yy;
	cv::flip(cv::Mat(n_kern_y, n_kern_x, CV_32FC1, kern_yy_f), kern_yy, -1);

	//specify anchor since we are to perform a convolution, not a correlation
	cv::Point anchor(n_kern_x - n_kern_x / 2 - 1, n_kern_y - n_kern_y / 2 - 1);

	//run image filter
	filter2D(src, Dxx, -1, kern_xx, anchor);
	filter2D(src, Dxy, -1, kern_xy, anchor);
	filter2D(src, Dyy, -1, kern_yy, anchor);

	delete[] kern_xx_f;
	delete[] kern_xy_f;
	delete[] kern_yy_f;
}

void Frangi::eig2image(const cv::Mat& Dxx, const cv::Mat& Dxy, const cv::Mat& Dyy, cv::Mat& lambda1, cv::Mat& lambda2, cv::Mat& Ix, cv::Mat& Iy)
{
	cv::Mat tmp, tmp2;
	tmp2 = Dxx - Dyy;
	sqrt(tmp2.mul(tmp2) + 4 * Dxy.mul(Dxy), tmp);
	 
	cv::Mat v2x = 2 * Dxy;
	cv::Mat v2y = Dyy - Dxx + tmp;

	//normalize
	cv::Mat mag;
	sqrt((v2x.mul(v2x) + v2y.mul(v2y)), mag);
	cv::Mat v2xtmp = v2x.mul(1.0f / mag);
	v2xtmp.copyTo(v2x, mag != 0);
	cv::Mat v2ytmp = v2y.mul(1.0f / mag);
	v2ytmp.copyTo(v2y, mag != 0);

	//eigenvectors are orthogonal
	cv::Mat v1x, v1y;
	v2y.copyTo(v1x);
	v1x = -1 * v1x;
	v2x.copyTo(v1y);

	//compute eigenvalues
	cv::Mat mu1 = 0.5 * (Dxx + Dyy + tmp);
	cv::Mat mu2 = 0.5 * (Dxx + Dyy - tmp);

	//sort eigenvalues by absolute value abs(Lambda1) < abs(Lamda2)
	cv::Mat check = abs(mu1) > abs(mu2);
	mu1.copyTo(lambda1); mu2.copyTo(lambda1, check);
	mu2.copyTo(lambda2); mu1.copyTo(lambda2, check);

	v1x.copyTo(Ix); v2x.copyTo(Ix, check);
	v1y.copyTo(Iy); v2y.copyTo(Iy, check);
}

void Frangi::frangi2D(cv::Mat& src, cv::Mat& maxVals, cv::Mat& whatScale, cv::Mat& outAngles)
{
	std::vector<cv::Mat> ALLfilterd;
	std::vector<cv::Mat> ALLangles;

	float bet = 2 * BetaOne * BetaOne;
	float c_1 = 2 * BetaTwo * BetaTwo;

	for (float sigma = sigma_start; sigma <= sigma_end; sigma += sigma_step)
	{
		//create 2D hessians
		cv::Mat Dxx, Dyy, Dxy;
		Hessian2D(src, Dxx, Dxy, Dyy, sigma);

		Dxx = Dxx * sigma * sigma;
		Dyy = Dyy * sigma * sigma;
		Dxy = Dxy * sigma * sigma;

		//calculate Eigenvalues 
		cv::Mat lambda1, lambda2, Ix, Iy;
		eig2image(Dxx, Dxy, Dyy, lambda1, lambda2, Ix, Iy);

		//Compute direction of minor eigenvector 
		cv::Mat angles;
		cv::phase(Ix, Iy, angles);
		ALLangles.push_back(angles);

		//compute some siilarity measures
		lambda2.setTo(nextafterf(0, 1), lambda2 == 0);
		cv::Mat Rb = lambda1.mul(1.0 / lambda2);
		Rb = Rb.mul(Rb);
		cv::Mat S2 = lambda1.mul(lambda1) + lambda2.mul(lambda2);

		//compute output image 
		cv::Mat tmp1, tmp2;
		exp(-Rb / BetaOne, tmp1);
		exp(-S2 / BetaTwo, tmp2);
		cv::Mat Ifiltered = tmp1.mul(cv::Mat::ones(src.rows, src.cols, src.type()) - tmp2);
		if (false) {
			Ifiltered.setTo(0, lambda2 < 0);
		}
		else {
			Ifiltered.setTo(0, lambda2 > 0);
		}
		ALLfilterd.push_back(Ifiltered);
	}

	float sigma = sigma_start;
	ALLfilterd[0].copyTo(maxVals);
	ALLfilterd[0].copyTo(whatScale);
	ALLfilterd[0].copyTo(outAngles);
	whatScale.setTo(sigma);

	for (int i = 1; i < ALLfilterd.size(); i++) {
		maxVals = max(maxVals, ALLfilterd[i]);
		whatScale.setTo(sigma, ALLfilterd[i] == maxVals);
		ALLangles[i].copyTo(outAngles, ALLfilterd[i] == maxVals);
		sigma += sigma_step;
	}
}