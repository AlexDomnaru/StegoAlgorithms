#pragma once
#include "BaseIncludes.h"

float MSE(Mat coverImage, Mat stegoImage) {

	float coeff = pow((1.0 / (coverImage.rows * coverImage.cols)), 2);
	float sum = 0;
	for (int i = 0; i < coverImage.rows; i++) {
		for (int j = 0; j < coverImage.cols; j++) {
			sum += pow(coverImage.at<uchar>(i, j) - stegoImage.at<uchar>(i, j), 2);
		}
	}
	return coeff * sum;
}

float PSNR(Mat coverImage, Mat stegoImage) {
	return 10 * log10((255 * 255) / MSE(coverImage, stegoImage));
}