#pragma once
#include "BaseIncludes.h"

float MSE(cv::Mat coverImage, cv::Mat stegoImage) {

	float coeff = pow((1.0 / (coverImage.rows * coverImage.cols)), 2);
	float sum = 0;
	for (int i = 0; i < coverImage.rows; i++) {
		for (int j = 0; j < coverImage.cols; j++) {
			uchar a = coverImage.at<uchar>(i, j);
			uchar b = stegoImage.at<uchar>(i, j);
			sum += pow(a - b, 2);
		}
	}
	return coeff * sum;
}

float MSE(cv::Mat coverImage, cv::Mat stegoImage, int flag) {

	float coeff = pow((1.0 / (coverImage.rows * coverImage.cols)), 2);
	float sum = 0;
	for (int i = 0; i < coverImage.rows; i++) {
		for (int j = 0; j < coverImage.cols; j++) {
			float a = coverImage.at<float>(i, j);
			float b = stegoImage.at<float>(i, j);
			sum += pow(a - b, 2);
		}
	}
	return coeff * sum;
}

float PSNR(cv::Mat coverImage, cv::Mat stegoImage) {
	return 10 * log10((255 * 255) / MSE(coverImage, stegoImage));
}


float PSNR(cv::Mat coverImage, cv::Mat stegoImage, int flag) {
	return 10 * log10((255 * 255) / MSE(coverImage, stegoImage, flag));
}