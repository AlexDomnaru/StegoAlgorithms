#pragma once
#include "BaseIncludes.h"
#include <numeric>
#include <ctime>

/*uchar chars[4];
bitset<8> character;
int i = 0;
for (i; i < 8; i++) {
	character[i % 8] = bits[i];
}
chars[0] = (uchar)character.to_ulong();
for (i; i < 16; i++) {
	character[i % 8] = bits[i];
}
chars[1] = (uchar)character.to_ulong();
for (i; i < 24; i++) {
	character[i % 8] = bits[i];
}
chars[2] = (uchar)character.to_ulong();
for (i; i < 32; i++) {
	character[i % 8] = bits[i];
}
chars[3] = (uchar)character.to_ulong();*/

int getEdgePixelCount(Mat edgeImage) {
	int pixelCount = 0;
	for (int i = 0; i < edgeImage.rows; i++) {
		for (int j = 0; j < edgeImage.cols; j++) {
			pixelCount += (int)edgeImage.at<uchar>(i, j);
		}
	}

	return pixelCount / 255;
}

float toFloat(bitset<32> bits) {
	float number;
	memcpy(&number, &bits, sizeof(float));
	return number;
}

vector<Point> getEdgeMap(Mat edgeImage) {
	vector<Point> map;

	for (int i = 0; i < edgeImage.rows; i++) {
		for (int j = 0; j < edgeImage.cols; j++) {
			if ((int)edgeImage.at<uchar>(i, j) == 255) {
				Point pixel = Point(i, j);
				map.push_back(pixel);
			}
		}
	}

	return map;
}

void edgeMapIndexesPermutation(vector<int> &map, int stegoKey) {
	for (int i = stegoKey; i < 2 * map.size() / 3; i++) {
		if (i % 2 == 0) {
			swap(map[i], map[map.size() / 3 + i - 1]);
		}
		else {
			swap(map[i], map[2 * map.size() / 3 - i - 1]);
		}
	}
}

double findThreshold(Mat blurredImage, int messageSize, int width) {
	int edgePixelCount;
	Mat edgeImage;
	double limit = messageSize * 0.1,
		maxThreshold = 1000,
		minThreshold = 0,
		t_h;
	bool set = false;

	do {
		t_h = (maxThreshold + minThreshold) / 2;
		cv::Canny(blurredImage, edgeImage, t_h, 0.4 * t_h, width);
		edgePixelCount = getEdgePixelCount(edgeImage);
		edgePixelCount *= 2;

		int difference = edgePixelCount - messageSize;

		if (difference > limit) {
			minThreshold = t_h;
		}
		else if (difference < 0) {
			maxThreshold = t_h;
		}
		else {
			set = true;
		}
	} while (set == false);

	return t_h;
}

int getStegoKey(int edgeMapSize) {
	bool correct = false;
	int key;
	while (!correct) {
		printf("Please enter a number between %d and %d: \n", edgeMapSize / 3, 2 * edgeMapSize / 3);
		cin >> key;
		if (key > edgeMapSize / 3 && key < 2 * edgeMapSize / 3) {
			correct = true;
		}
	}
	return key;
}

tuple<Mat, int> EDS(Mat coverImage, string secret, int width) {
	Mat stegoImage = coverImage.clone();/*Mat::zeros(coverImage.rows, coverImage.cols, CV_8UC1);*/
	Mat edgeImage, maxEdgeImage, maskedImage;

	vector<int> secretBits;
	int augmentedSecretSize = 4 + secret.size();
	secretBits.resize(augmentedSecretSize * 8);
	int i, j, k = 0;

	bitset<32> secretLengthBits = bitset<32>(secret.size());
	for (k = 0; k < 32; k++) {
		secretBits[k] = secretLengthBits[k];
	}
	for (i = 0; i < secret.size(), k < augmentedSecretSize * 8; i++) {
		bitset<8> bitarray = bitset<8>(secret[i]);
		for (j = 0; j < 8; j++, k++) {
			secretBits[k] = bitarray[j];
		}
	}

	//GaussianBlur(coverImage, blurredImage, Size(width, width), 0, 0);
	bitwise_and(coverImage, 252, maskedImage);
	//augmentedSecretSize * 8?
	float t_h = findThreshold(maskedImage, augmentedSecretSize * 8, width);
	Canny(maskedImage, edgeImage, t_h, t_h * 0.4, width);
	imshow("wow", edgeImage);
	//generate edgeMap
	vector<Point> edgeMap = getEdgeMap(edgeImage);
	size_t edgeMapSize = edgeMap.size();
	vector<int> edgeMapIndexes(edgeMapSize);
	iota(edgeMapIndexes.begin(), edgeMapIndexes.end(), 0);

	//generate stegokey at runtime
	int stegoKey = getStegoKey(edgeMapSize);

	//compute permutation
	edgeMapIndexesPermutation(edgeMapIndexes, stegoKey);

	int bitIndex = 0;
	uchar* pixelPtr;
	for (i = 0; i < edgeMapSize, bitIndex < secretBits.size(); i++) {
		Point pixelLocation = edgeMap[edgeMapIndexes[i]];
		pixelPtr = stegoImage.ptr<uchar>(pixelLocation.x, pixelLocation.y);
		bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

		pixelBits[0] = secretBits[bitIndex];
		pixelBits[1] = secretBits[bitIndex + 1];

		pixelPtr[0] = static_cast<uchar>(pixelBits.to_ulong());
		bitIndex += 2;
	}

	Canny(maskedImage, maxEdgeImage, 0, 0, 3);
	imshow("wow", maxEdgeImage);
	union
	{
		float input;
		int   output;
	}    data;

	data.input = t_h;
	bitset<sizeof(float) * CHAR_BIT> thresholdBits(data.output);
	cout << "th " << thresholdBits << endl;

	data.input = width;
	bitset<sizeof(float) * CHAR_BIT> widthBits(data.output);
	cout << "w " << widthBits << endl;

	int index = 0;
	for (i = 0; i < stegoImage.rows, index < 32; i++) {
		for (j = 0; j < stegoImage.cols, index < 32; j++) {
			pixelPtr = maxEdgeImage.ptr<uchar>(i, j);
			if (pixelPtr[0] == 0) {
				pixelPtr = stegoImage.ptr<uchar>(i, j);
				bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

				pixelBits[0] = thresholdBits[index];

				pixelPtr[0] = static_cast<uchar>(pixelBits.to_ulong());
				index++;
			}
		}
	}

	index = 0;
	for (i; i < stegoImage.rows, index < 32; i++) {
		for (j; j < stegoImage.cols, index < 32; j++) {
			pixelPtr = maxEdgeImage.ptr<uchar>(i, j);
			if (pixelPtr[0] == 0) {
				pixelPtr = stegoImage.ptr<uchar>(i, j);
				bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

				pixelBits[0] = widthBits[index];

				pixelPtr[0] = static_cast<uchar>(pixelBits.to_ulong());
				index++;
			}
		}
	}
	return tuple<Mat, int>(stegoImage, stegoKey);
}

void EDSdisembed(Mat stegoImage, int stegoKey) {

	Mat edgeImage, maxEdgeImage, maskedImage;

	bitwise_and(stegoImage, 252, maskedImage);
	Canny(maskedImage, maxEdgeImage, 0, 0, 3);

	bitset<32> thresholdBits, widthBits;
	uchar* pixelPtr;
	int i, j;
	int index = 0;

	for (i = 0; i < stegoImage.rows, index < 32; i++) {
		for (j = 0; j < stegoImage.cols, index < 32; j++) {
			pixelPtr = maxEdgeImage.ptr<uchar>(i, j);
			if (pixelPtr[0] == 0) {
				pixelPtr = stegoImage.ptr<uchar>(i, j);
				bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

				thresholdBits[index] = pixelBits[0];

				index++;
			}
		}
	}

	index = 0;
	for (i; i < stegoImage.rows, index < 32; i++) {
		for (j; j < stegoImage.cols, index < 32; j++) {
			pixelPtr = maxEdgeImage.ptr<uchar>(i, j);
			if (pixelPtr[0] == 0) {
				pixelPtr = stegoImage.ptr<uchar>(i, j);
				bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

				widthBits[index] = pixelBits[0];

				pixelPtr[0] = static_cast<uchar>(pixelBits.to_ulong());
				index++;
			}
		}
	}

	cout << "th " << thresholdBits << endl;
	cout << "w " << widthBits << endl;

	float t_h = toFloat(thresholdBits);
	float width = toFloat(widthBits);

	Canny(maskedImage, edgeImage, t_h, 0.4*t_h, width);

	vector<Point> edgeMap = getEdgeMap(edgeImage);
	size_t edgeMapSize = edgeMap.size();
	vector<int> edgeMapIndexes(edgeMapSize);
	iota(edgeMapIndexes.begin(), edgeMapIndexes.end(), 0);

	edgeMapIndexesPermutation(edgeMapIndexes, stegoKey);

	//disembed message length
	size_t messageLength;
	bitset<32> messageLengthBits;
	index = 0;
	for (i = 0; i < 16; i++) {
		Point pixelLocation = edgeMap[edgeMapIndexes[i]];
		pixelPtr = stegoImage.ptr<uchar>(pixelLocation.x, pixelLocation.y);
		bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

		messageLengthBits[index++] = pixelBits[0];
		messageLengthBits[index++] = pixelBits[1];
	}

	vector<int> messageBits;
	messageLength = messageLengthBits.to_ulong();
	for (i; i < (messageLength + 4) * 4; i++) {
		Point pixelLocation = edgeMap[edgeMapIndexes[i]];
		pixelPtr = stegoImage.ptr<uchar>(pixelLocation.x, pixelLocation.y);
		bitset<8> pixelBits = bitset<8>(pixelPtr[0]);

		messageBits.push_back(pixelBits[0]);
		messageBits.push_back(pixelBits[1]);
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
}