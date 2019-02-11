#pragma once
#include "BaseIncludes.h"
#include <map>

std::map<int, int> quantizationTable;

void initializeQuantizationTable(int quantTableFlag) {
	if (quantTableFlag == 1) {
		quantizationTable = {
			{0, 7},
			{8, 15},
			{16, 31},
			{32, 63},
			{64, 127},
			{128, 255}
		};
	}
	else if (quantTableFlag == 2) {
		quantizationTable = {
			{0, 15},
			{16, 23},
			{24, 31},
			{32, 47},
			{48, 63},
			{64, 79},
			{80, 95},
			{96, 103},
			{104, 111},
			{112, 119},
			{120, 127},
			{128, 135},
			{136, 143},
			{144, 151},
			{152, 159},
			{160, 191},
			{192, 255}
		};
	}
}

int getFirstGreaterKey(map<int, int> map, int val) {

	std::map<int, int>::reverse_iterator it;
	for (it = map.rbegin(); it != map.rend(); it++) {
		if (it->first <= val) {
			return it->first;
		}
	}
}

int getLastMultipleOf2(int number) {
	int multiple = 0;
	while (multiple < number) {
		multiple += 2;
	}
	return multiple - 2;
}


int getBitCount(int lower, int upper) {
	return log2(upper - lower + 1);
}

std::vector<bool> toBinary(int value, int bitCount) {
	std::vector<bool> bits;
	bits.resize(bitCount, 0);
	bitCount--;
	while (value > 0) {
		bits[bitCount] = value % 2;
		bitCount--;
		value /= 2;
	}
	return bits;
}

tuple<int, int> getPixelValues(int g_i1, int g_i2, double m, bool isOdd) {
	int g_i1_prime, g_i2_prime;
	int ceiling_m = ceil(m / 2);
	int floor_m = floor(m / 2);
	if (isOdd) {
		g_i1_prime = g_i1 - ceiling_m;
		g_i2_prime = g_i2 + floor_m;
	}
	else {
		g_i1_prime = g_i1 - floor_m;
		g_i2_prime = g_i2 + ceiling_m;
	}
	return tuple<int, int>(g_i1_prime, g_i2_prime);
}

cv::Mat PVDembed(cv::Mat coverImage, string secret, int quantTableFlag) {
	initializeQuantizationTable(quantTableFlag);
	cv::Mat stegoImage = coverImage.clone();

	size_t rows = getLastMultipleOf2(coverImage.rows);
	size_t cols = getLastMultipleOf2(coverImage.cols);

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

	uchar* pixelPtr;
	int pixelCount = 0;
	map<int, int>::iterator firstGreater;
	for (i = 0; i < rows && secretBits.size() > 0; i++) {
		for (j = 0; j < cols && secretBits.size() > 0; j += 2) {
			pixelPtr = stegoImage.ptr<uchar>(i, j);
			int differenceValue = pixelPtr[0] - pixelPtr[1];

			firstGreater = quantizationTable.find(getFirstGreaterKey(quantizationTable, abs(differenceValue)));

			double m;
			m = differenceValue - firstGreater->second;
			tuple<int, int> boundaryCheckValues = getPixelValues(pixelPtr[0], pixelPtr[1], m, differenceValue % 2);
			if (get<0>(boundaryCheckValues) < 0 || get<0>(boundaryCheckValues) > 255 ||
				get<1>(boundaryCheckValues) < 0 || get<1>(boundaryCheckValues) > 255)
			{
				break;
			}
			int bitCount = getBitCount(firstGreater->first, firstGreater->second);
			int bitsToInt = 0;
			if (bitCount > secretBits.size()) {
				bitCount = secretBits.size();
			}
			while (bitCount > 0) {
				bitsToInt = bitsToInt + secretBits[0] * pow(2, bitCount - 1);
				bitCount--;
				secretBits.erase(secretBits.begin());
			}
			int newDifferenceValue = differenceValue >= 0 ? firstGreater->first + bitsToInt : -1 * (firstGreater->first + bitsToInt);

			//embed
			m = differenceValue - newDifferenceValue;
			tuple<int, int> newPixelValues = getPixelValues(pixelPtr[0], pixelPtr[1], m, differenceValue % 2);

			pixelPtr[0] = static_cast<uchar>(get<0>(newPixelValues));
			pixelPtr[1] = static_cast<uchar>(get<1>(newPixelValues));
		}
	}

	return stegoImage;
}

void PVDdisembed(cv::Mat stegoImage, int quantTableFlag) {
	initializeQuantizationTable(quantTableFlag);
	size_t rows = getLastMultipleOf2(stegoImage.rows);
	size_t cols = getLastMultipleOf2(stegoImage.cols);

	vector<int> secretBits;
	bitset<32> messageLengthBits;

	int i = 0, j = 0, index = 0;
	uchar* pixelPtr;
	map<int, int>::iterator firstGreater;

	for (i; i < rows; i++) {
		for (j; j < cols; j += 2) {
			pixelPtr = stegoImage.ptr<uchar>(i, j);
			int differenceValue = pixelPtr[0] - pixelPtr[1];
			//outfile << "difference val: " << differenceValue << " | bits: ";
			firstGreater = quantizationTable.find(getFirstGreaterKey(quantizationTable, abs(differenceValue)));

			double m;
			//check boundary falloff
			m = differenceValue - firstGreater->second;
			tuple<int, int> boundaryCheckValues = getPixelValues(pixelPtr[0], pixelPtr[1], m, differenceValue % 2);
			if (get<0>(boundaryCheckValues) < 0 || get<0>(boundaryCheckValues) > 255 ||
				get<1>(boundaryCheckValues) < 0 || get<1>(boundaryCheckValues) > 255)
			{
				break;
			}

			int bitCount = getBitCount(firstGreater->first, firstGreater->second);

			int embeddedValue = differenceValue >= 0 ? differenceValue - firstGreater->first : -1 * differenceValue - firstGreater->first;
			std::vector<bool> bitarray = toBinary(embeddedValue, bitCount);
			for (int k = 0; k < bitarray.size(); k++) {
				if (index >= 32) {
					secretBits.push_back(bitarray[k]);
				}
				else {
					messageLengthBits[index++] = bitarray[k];
				}
			}
			if (index >= 32) {
				j += 2;
				break;
			}
		}
		if (index >= 32) {
			break;
		}
	}
	size_t messageLength = messageLengthBits.to_ulong();
	messageLength *= 8;
	messageLength -= secretBits.size();
	int pixelCount = 0;

	for (i; i < rows && pixelCount < messageLength; ++i) {
		for (j; j < cols && pixelCount < messageLength; j += 2) {
			pixelPtr = stegoImage.ptr<uchar>(i, j);
			int differenceValue = pixelPtr[0] - pixelPtr[1];
			firstGreater = quantizationTable.find(getFirstGreaterKey(quantizationTable, abs(differenceValue)));

			double m;
			//check boundary falloff
			m = differenceValue - firstGreater->second;
			tuple<int, int> boundaryCheckValues = getPixelValues(pixelPtr[0], pixelPtr[1], m, differenceValue % 2);
			if (get<0>(boundaryCheckValues) < 0 || get<0>(boundaryCheckValues) > 255 ||
				get<1>(boundaryCheckValues) < 0 || get<1>(boundaryCheckValues) > 255)
			{
				break;
			}

			int bitCount = getBitCount(firstGreater->first, firstGreater->second);

			int embeddedValue = differenceValue >= 0 ? differenceValue - firstGreater->first : -1 * differenceValue - firstGreater->first;
			std::vector<bool> bitarray = toBinary(embeddedValue, bitCount);
			for (int k = 0; k < bitarray.size() && pixelCount < messageLength; k++) {
				secretBits.push_back(bitarray[k]);
				pixelCount++;

			}
		}
		j = 0;
	}

	i = 0;
	bool letterIsComplete = false;
	bitset<8> letter = bitset<8>(0);
	while (i < secretBits.size()) {
		for (j = 0; j < 8 && i < secretBits.size(); j++, i++) {
			letter[j] = secretBits[i];
		}
		cout << (uchar)letter.to_ulong();
		letter = bitset<8>(0);
	}
	cout << endl;
}