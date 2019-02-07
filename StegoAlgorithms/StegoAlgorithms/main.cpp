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
Mat coverImage;
Mat stegoImage;
Mat readStegoImage;

void onMouse(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		uchar* p = coverImage.ptr<uchar>(y, x);
		printf("location: %d %d, value: %d \n", y, x, p[0]);
	}
}

void onMouse2(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		uchar* p = stegoImage.ptr<uchar>(y, x);
		printf("location: %d %d, stego value: %d \n", y, x, p[0]);
	}
}

int main(int argc, char** argv) {
	coverImage = imread(argv[3], IMREAD_GRAYSCALE);
	namedWindow("cover");
	namedWindow("stego");
	imshow("cover", coverImage);

	Mat imgtohide = imread(argv[1], IMREAD_GRAYSCALE);
	if (coverImage.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	ifstream file(argv[2]);

	stringstream secret;
	secret << file.rdbuf();
	file.close();
	
	tuple<Mat, int> res = EDS(coverImage, secret.str(), 3);
	stegoImage = get<0>(res);

	//stegoImage = PVDembed(coverImage, secret.str());
	imwrite("stegoImage.png", stegoImage);
	imshow("stego", stegoImage);

	readStegoImage = imread("stegoImage.png", IMREAD_GRAYSCALE);
	EDSdisembed(readStegoImage, get<1>(res));
	//PVDdisembed(readStegoImage);
	
	/*imwrite("stegocrop.png", stegoImage(Rect(9, 49, 70, 70)));*/
	cout << setprecision(20) << endl << fixed << MSE(coverImage, readStegoImage) << " " << PSNR(coverImage, readStegoImage);
	waitKey(0);
	return 0;
}