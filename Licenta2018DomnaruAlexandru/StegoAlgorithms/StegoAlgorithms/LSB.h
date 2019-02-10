#pragma once
#include "BaseIncludes.h"
#include <tuple>

Mat LSBembed(Mat coverImage, string secret, int lsbNum) {

	Mat stegoImage = coverImage.clone();

	size_t rows = coverImage.rows;
	size_t cols = coverImage.cols;

	vector<int> secretBits;
	int augmentedSecretSize = 4 + secret.size();
	secretBits.resize(augmentedSecretSize * 8);
	int i, j, k = 0;

	bitset<32> secretLengthBits = bitset<32>(secret.size());
	for (k = 0; k < 32; k++) {
		secretBits[k] = secretLengthBits[k];
	}
	for (i = 0; i < secret.size() && k < augmentedSecretSize * 8; i++) {
		bitset<8> bitarray = bitset<8>(secret[i]);
		for (j = 0; j < 8; j++, k++) {
			secretBits[k] = bitarray[j];
		}
	}

	int bitIndex = 0;
	uchar* pixelPtr;
	for (i = 0; i < rows && bitIndex < secretBits.size(); i++) {
		for (j = 0; j < cols && bitIndex < secretBits.size(); j++) {
			pixelPtr = stegoImage.ptr<uchar>(i, j);
			bitset<8> pixelBits = bitset<8>(pixelPtr[0]);
			int ind = 0;
			while (ind < lsbNum && bitIndex < secretBits.size()) {
				pixelBits[ind++] = secretBits[bitIndex++];
			}
			pixelPtr[0] = static_cast<uchar>(pixelBits.to_ulong());
		}
	}

	return stegoImage;
}

void LSBdisembed(cv::Mat stegoImage, int lsbNum) {

	size_t messageLength;
	size_t rows = stegoImage.rows;
	size_t cols = stegoImage.cols;

	int i, j;
	uchar* pixelPtr;
	int bitIndex = 0;
	bitset<32> messageLenBits = bitset<32>(0);
	for (j = 0; j < 32/lsbNum; j++) {
		pixelPtr = stegoImage.ptr<uchar>(0, j);
		bitset<8> pixelBits = bitset<8>(pixelPtr[0]);
		int ind = 0;
		while (ind < lsbNum && bitIndex < 32) {
			messageLenBits[bitIndex++] = pixelBits[ind++];
		}
	}

	messageLength = messageLenBits.to_ulong();
	messageLength *= 8;
	vector<int> messageBits;

	int index = 0;
	for (i = 0; i < rows && index < messageLength; i++) {
		for (j; j < cols && index < messageLength; j++) {
			pixelPtr = stegoImage.ptr<uchar>(i, j);
			bitset<8> pixelBits = bitset<8>(pixelPtr[0]);
			int ind = 0;
			while (ind < lsbNum && index < messageLength) {
				messageBits.push_back(pixelBits[ind++]);
				index++;
			}
		}
		j = 0;
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