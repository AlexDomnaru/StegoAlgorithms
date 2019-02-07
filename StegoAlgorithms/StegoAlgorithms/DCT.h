#pragma once
#include "BaseIncludes.h"
#include <iomanip>

int getLastMultipleOf8(int number) {
	int multiple = 0;
	while (multiple < number) {
		multiple += 8;
	}
	return multiple - 8;
}

void goodCout(Mat img) {
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			cout << setprecision(10) << img.at<float>(i, j) << " ";
		}
		cout << endl;
	}
}

ofstream fout("helper.txt");
ofstream fout2("helper2.txt");
void goodFout(Mat img) {
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			fout << setprecision(10) << img.at<float>(i, j) << " ";
		}
		fout << endl;
	}
}

Mat DCT(Mat coverImage, string secret) {
	Mat stegoImage, convertImage;

	coverImage.convertTo(convertImage, CV_32FC1, 1.0 / 255);

	//pad image if needed
	int leftEdge = getLastMultipleOf8(coverImage.cols);
	int botEdge = getLastMultipleOf8(coverImage.rows);

	stegoImage = convertImage.clone();

	//create secret bits array and augment message with message size
	vector<int> secretBits;
	secretBits.resize(secret.size() * 8);
	int i, j, k = 0, l = 0;

	for (i = 0; i < secret.size(), k < secret.size() * 8; i++) {
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
	for (i = 0; i < leftEdge && bitIndex < secretBits.size(); i += 8) {
		for (j = 0; j < botEdge && bitIndex < secretBits.size(); j += 8) {
			Mat block = stegoImage(Rect(i, j, 8, 8));
			Mat dctBlock;
			dct(block, dctBlock);
			for (k = 0; k < 8 & bitIndex < secretBits.size(); k++) {
				for (l = 7 - k; l < 8 & bitIndex < secretBits.size(); l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 15; ind < 22 & bitIndex < secretBits.size(); ind++) {
							pixelBits[ind] = secretBits[bitIndex++];
						}
						float interm;
						memcpy(&interm, &pixelBits, sizeof(sizeof(float) * CHAR_BIT));
						dctBlock.at<float>(k, l) = interm;
					}
				}
			}
			dct(dctBlock, block, DCT_INVERSE);
			block.copyTo(stegoImage(Rect(i, j, 8, 8)));
		}
	}

	return stegoImage;
}

void DCTdisembed(Mat stegoImage, size_t size) {

	Mat block, dctBlock;
	int i, j, k, l, index = 0;
	size_t messageLength;
	bitset<32> messageLengthBits;
	vector<int> messageBits;

	union
	{
		float input;
		int   output;
	}    pixelVal;

	//for (i = 0; i < 1; i += 8) {
	//	for (j = 0; j < 32; j += 8) {
	//		block = stegoImage(Rect(i, j, 8, 8));
	//		dct(block, dctBlock);
	//		pixelVal.input = dctBlock.at<float>(0, 0);
	//		bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);
	//		fout2 << setprecision(10) << pixelBits << " " << pixelVal.input << endl;

	//		for (int ind = 0; ind < 8; ind++) {
	//			messageLengthBits[index++] = pixelBits[ind];
	//		}
	//	}
	//}

	messageLength = size * 8;
	index = 0;
	//for (j = 32; j < stegoImage.cols && index < messageLength; j += 8) {
	//	block = stegoImage(Rect(0, j, 8, 8));
	//	dct(block, dctBlock);
	//	pixelVal.input = dctBlock.at<float>(0, 0);
	//	bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);
	//	fout2 << setprecision(10) << pixelBits << " " << pixelVal.input << endl;

	//	for (int ind = 0; ind < 8 && index < messageLength; ind++) {
	//		messageBits.push_back(pixelBits[ind]);
	//		index++;
	//	}
	//}

	for (i = 0; i < stegoImage.rows && index < messageLength; i += 8) {
		for (j = 0; j < stegoImage.cols && index < messageLength; j += 8) {
			block = stegoImage(Rect(i, j, 8, 8));
			dct(block, dctBlock);
			//goodCout(dctBlock);
			for (k = 0; k < 8 && index < messageLength; k++) {
				for (l = 7 - k; l < 8 && index < messageLength; l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);
						//fout2 << setprecision(10) << pixelBits << " " << pixelVal.input << endl;

						for (int ind = 15; ind < 22 && index < messageLength; ind++) {
							messageBits.push_back(pixelBits[ind]);
							index++;
						}
					}
				}
			}
		}
	}
	//cout << endl;
	i = 0;
	bitset<8> letter = bitset<8>(0);
	while (i < messageBits.size()) {
		for (j = 0; j < 8; j++, i++) {
			letter[j] = messageBits[i];
		}
		cout << (uchar)letter.to_ulong();
		letter = bitset<8>(0);
	}
}

Mat DCT(Mat coverImage, Mat secret) {
	Mat stegoImage, convertImage;

	coverImage.convertTo(convertImage, CV_32FC1, 1.0 / 255);

	int leftEdge = getLastMultipleOf8(coverImage.cols);
	int botEdge = getLastMultipleOf8(coverImage.rows);

	stegoImage = convertImage.clone();

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
	for (i = 0; i < leftEdge && bitIndex < secretBits.size(); i += 8) {
		for (j = 0; j < botEdge && bitIndex < secretBits.size(); j += 8) {
			Mat block = stegoImage(Rect(i, j, 8, 8));
			Mat dctBlock;
			dct(block, dctBlock);
			for (k = 0; k < 8 & bitIndex < secretBits.size(); k++) {
				for (l = 7 - k; l < 8 & bitIndex < secretBits.size(); l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 15; ind < 22 & bitIndex < secretBits.size(); ind++) {
							pixelBits[ind] = secretBits[bitIndex++];
						}

						float interm;
						memcpy(&interm, &pixelBits, sizeof(sizeof(float) * CHAR_BIT));
						dctBlock.at<float>(k, l) = interm;
					}
				}
			}

			dct(dctBlock, block, DCT_INVERSE);
			block.copyTo(stegoImage(Rect(i, j, 8, 8)));
		}
	}

	return stegoImage;
}

void DCTdisembed(Mat stegoImage, int rows, int cols) {

	Mat block, dctBlock;
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

	for (i = 0; i < leftEdge && index < messageLength; i += 8) {
		for (j = 0; j < botEdge && index < messageLength; j += 8) {
			block = stegoImage(Rect(i, j, 8, 8));
			dct(block, dctBlock);
			for (k = 0; k < 8 && index < messageLength; k++) {
				for (l = 7 - k; l < 8 && index < messageLength; l++) {
					pixelVal.input = dctBlock.at<float>(k, l);
					if (pixelVal.input != 0) {
						bitset<sizeof(float) * CHAR_BIT> pixelBits(pixelVal.output);

						for (int ind = 15; ind < 22 && index < messageLength; ind++) {
							messageBits.push_back(pixelBits[ind]);
							index++;
						}
					}
				}
			}

		}
	}

	bitset<8> letter = bitset<8>(0);
	index = 0;
	Mat receivedImage(rows, cols, CV_8UC1);
	for (i = 0; i < rows && index < messageBits.size(); i++) {
		for (j = 0; j < cols && index < messageBits.size(); j++) {
			for (k = 0; k < 8 && index < messageBits.size(); k++) {
				letter[k] = messageBits[index++];
			}

			receivedImage.at<uchar>(i, j) = static_cast<uchar>(letter.to_ulong());
			letter = bitset<8>(0);
		}
	}
	imshow("wow1", receivedImage);
}