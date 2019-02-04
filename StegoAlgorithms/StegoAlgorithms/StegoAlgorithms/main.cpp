#include "Algorithms.h"

//void edgeMapPermutation(vector<Point> map, int stegoKey) {
//	for (int i = 0; i < stegoKey; i++) {
//		next_permutation(map.begin(), map.end(), [map](const Point &a, const Point &b)
//		{
//			return distance(map.begin(), find(map.begin(), map.end(), a)) <
//				distance(map.begin(), find(map.begin(), map.end(), b));
//		});
//		for (int j = 0; j < map.size(); ++j) {
//			cout << map[j].x << ' ';
//		}
//		cout << endl;
//	}
//}
Mat img;
Mat stegoImg;

void onMouse(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		uchar* p = img.ptr<uchar>(y, x);
		printf("location: %d %d, value: %d \n", y, x, p[0]);
	}
}

void onMouse2(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		uchar* p = stegoImg.ptr<uchar>(y, x);
		printf("location: %d %d, stego value: %d \n", y, x, p[0]);
	}
}

int main(int argc, char** argv) {
	img = imread(argv[3], IMREAD_GRAYSCALE);
	Mat imgtohide = imread(argv[1], IMREAD_GRAYSCALE);
	imshow("aaa", imgtohide);
	if (img.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	cv::resize(img, img, cv::Size(), 0.3, 0.3);
	ifstream file(argv[2]);

	stringstream secret;
	secret << file.rdbuf();
	file.close();
	//tuple<Mat, int>result = PVDembed(image, secret.str());

	//imwrite("stegoimage.jpg", get<0>(result));

	//Mat image2;
	//image2 = imread("stegoimage.jpg", IMREAD_GRAYSCALE);
	//if (image2.empty()) // Check for invalid input
	//{
	//	cout << "Could not open or find the image" << std::endl;
	//	return -1;
	//}

	//PVDdisembed(image2, get<1>(result));
	/*tuple<Mat, int> res = EDS(image, secret.str(), 3);
	EDSdisembed(get<0>(res), get<1>(res));*/
	namedWindow("kek1");
	namedWindow("kek2");

	imshow("kek1", img);
	stegoImg = PVDembed(img, secret.str());
	imshow("kek2", stegoImg);/*
	setMouseCallback("kek1", onMouse, 0);
	setMouseCallback("kek2", onMouse2, 0);*/

	PVDdisembed(stegoImg);
	waitKey(0);
	return 0;
}