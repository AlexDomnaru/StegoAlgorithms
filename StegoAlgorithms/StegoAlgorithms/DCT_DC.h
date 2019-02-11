#pragma once
#include "BaseIncludes.h"

int getLastMultipleOf8(int number) {
	int multiple = 0;
	while (multiple < number) {
		multiple += 8;
	}
	return multiple - 8;
}

cv::Mat DCT_DC(cv::Mat coverImage, string secret) {
	cv::Mat stegoImage;

	//pad image if needed
	int leftEdge = getLastMultipleOf8(coverImage.cols);
	int botEdge = getLastMultipleOf8(coverImage.rows);

	stegoImage = coverImage.clone();

	//create secret bits array and augment message with message size
	vector<int> secretBits;
	secretBits.resize(secret.size() * 8);
	int i, j, k = 0, l = 0;

	for (i = 0; i < secret.size() && k < secret.size() * 8; i++) {
		bitset<8> bitarray = bitset<8>(secret[i]);
		for (j = 0; j < 8; j++, k++) {
			secretBits[k] = bitarray[j];
		}
	}
	//split into 8x8 blocks
	union
	{
		float input;
		int   output;
	}    pixelVal;

	int bitIndex = 0;
	for (i = 0; i < botEdge && bitIndex < secretBits.size(); i += 8) {
		for (j = 0; j < leftEdge && bitIndex < secretBits.size(); j += 8) {
			cv::Mat block = stegoImage(cv::Rect(j, i, 8, 8));
			cv::Mat dctBlock;
			cv::dct(block, dctBlock);
			l = 1;
			for (k = 0; k < 8 && bitIndex < secretBits.size(); k++) {
				for (l; l < 8 - k && bitIndex < secretBits.size(); l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 14; ind < 18 & bitIndex < secretBits.size(); ind++) {
							pixelBits[ind] = secretBits[bitIndex++];
						}

						float interm;
						memcpy(&interm, &pixelBits, sizeof(sizeof(float) * CHAR_BIT));
						dctBlock.at<float>(k, l) = interm;
					}
				}
				l = 0;
			}

			cv::dct(dctBlock, block, cv::DCT_INVERSE);
			block.copyTo(stegoImage(cv::Rect(j, i, 8, 8)));
		}
	}

	return stegoImage;
}

void DCT_DCDisembed(cv::Mat stegoImage, size_t size) {

	cv::Mat block, dctBlock;
	int i, j, k, l, index = 0;
	size_t messageLength;
	bitset<32> messageLengthBits;
	vector<int> messageBits;

	int leftEdge = getLastMultipleOf8(stegoImage.cols);
	int botEdge = getLastMultipleOf8(stegoImage.rows);

	union
	{
		float input;
		int   output;
	}    pixelVal;

	messageLength = size * 8;
	index = 0;

	for (i = 0; i < botEdge && index < messageLength; i += 8) {
		for (j = 0; j < leftEdge && index < messageLength; j += 8) {
			block = stegoImage(cv::Rect(j, i, 8, 8));
			cv::dct(block, dctBlock);
			l = 1;
			for (k = 0; k < 8 && index < messageLength; k++) {
				for (l; l < 8 - k && index < messageLength; l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 14; ind < 18 && index < messageLength; ind++) {
							messageBits.push_back(pixelBits[ind]);
							index++;
						}
					}
				}
				l = 0;
			}
		}
	}

	i = 0;
	bitset<8> letter = bitset<8>(0);
	while (i < messageBits.size()) {
		for (j = 0; j < 8; j++, i++) {
			letter[j] = messageBits[i];
		}
		cout << (uchar)letter.to_ulong();
		letter = bitset<8>(0);
	}
	cout << endl;
}

cv::Mat DCT_DC(cv::Mat coverImage, cv::Mat secret) {
	cv::Mat stegoImage;

	int leftEdge = getLastMultipleOf8(coverImage.cols);
	int botEdge = getLastMultipleOf8(coverImage.rows);

	stegoImage = coverImage.clone();

	//create secret bits array and augment message with message size
	vector<int> secretBits;
	int augmentedSecretSize = secret.rows * secret.cols * 8;
	secretBits.resize(augmentedSecretSize);
	int i, j, k = 0, l = 0;
	uchar* pixelPtr;

	for (i = 0; i < secret.rows && k < augmentedSecretSize; i++) {
		for (j = 0; j < secret.cols && k < augmentedSecretSize; j++) {
			pixelPtr = secret.ptr<uchar>(i, j);
			bitset<8> bitarray = bitset<8>(pixelPtr[0]);
			for (l = 0; l < 8; l++, k++) {
				secretBits[k] = bitarray[l];
			}
		}
	}
	
	//split into 8x8 blocks
	union
	{
		float input;
		int   output;
	}    pixelVal;

	int bitIndex = 0;
	for (i = 0; i < botEdge && bitIndex < secretBits.size(); i += 8) {
		for (j = 0; j < leftEdge && bitIndex < secretBits.size(); j += 8) {
			cv::Mat block = stegoImage(cv::Rect(j, i, 8, 8));
			cv::Mat dctBlock;
			cv::dct(block, dctBlock);
			l = 1;
			for (k = 0; k < 8 & bitIndex < secretBits.size(); k++) {
				for (l; l < 8 - k & bitIndex < secretBits.size(); l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 14; ind < 18 & bitIndex < secretBits.size(); ind++) {
							pixelBits[ind] = secretBits[bitIndex++];
						}

						float interm;
						memcpy(&interm, &pixelBits, sizeof(sizeof(float) * CHAR_BIT));
						dctBlock.at<float>(k, l) = interm;
					}
				}
				l = 0;
			}

			cv::dct(dctBlock, block, cv::DCT_INVERSE);
			block.copyTo(stegoImage(cv::Rect(j, i, 8, 8)));
		}
	}

	return stegoImage;
}

void DCT_DCDisembed(cv::Mat stegoImage, int rows, int cols) {
	cv::Mat block, dctBlock;
	int i, j, k = 0, l, index = 0;
	size_t messageLength;
	bitset<64> messageLengthBits;
	vector<int> messageBits;
	uchar* pixelPtr;

	int leftEdge = getLastMultipleOf8(stegoImage.cols);
	int botEdge = getLastMultipleOf8(stegoImage.rows);

	union
	{
		float input;
		int   output;
	}    pixelVal;

	messageLength = rows * cols;
	messageLength *= 8;
	index = 0;

	for (i = 0; i < botEdge && index < messageLength; i += 8) {
		for (j = 0; j < leftEdge && index < messageLength; j += 8) {
			block = stegoImage(cv::Rect(j, i, 8, 8));
			cv::dct(block, dctBlock);
			l = 1;
			for (k = 0; k < 8 && index < messageLength; k++) {
				for (l; l < 8 - k && index < messageLength; l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 14; ind < 18 && index < messageLength; ind++) {
							messageBits.push_back(pixelBits[ind]);
							index++;
						}
					}
				}
				l = 0;
			}
		}
	}

	bitset<8> letter = bitset<8>(0);
	index = 0;
	cv::Mat receivedImage(rows, cols, CV_8UC1);
	for (i = 0; i < rows && index < messageBits.size(); i++) {
		for (j = 0; j < cols && index < messageBits.size(); j++) {
			for (k = 0; k < 8 && index < messageBits.size(); k++) {
				letter[k] = messageBits[index++];
			}

			receivedImage.at<uchar>(i, j) = static_cast<uchar>(letter.to_ulong());
			letter = bitset<8>(0);
		}
	}
	cv::namedWindow("received image");
	cv::imshow("received image", receivedImage);
}