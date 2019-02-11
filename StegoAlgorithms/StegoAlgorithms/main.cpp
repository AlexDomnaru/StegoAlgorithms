#include "Algorithms.h"
#include <windows.h>
#include <iomanip>

cv::Mat coverImage;
cv::Mat stegoImage;

int main(int argc, char** argv) {

	string input;
	while (true) {
		cout << "Please type the algorithm you'd like to run (1 - LSB, 2 - PVD, 3 - EDS, 4 - DCT):" << endl;
		cin >> input;
		if (input == "1") {
			char filename[MAX_PATH];

			OPENFILENAME ofn;
			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Images\0*.png\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select an image to embed in";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&ofn))
			{
				coverImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);
				if (coverImage.empty())
				{
					cout << "Could not open or find the image" << std::endl;
					return -1;
				}

				cv::namedWindow("cover");
				cv::imshow("cover", coverImage);
			}

			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Text Files\0*.txt\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select the message you want to embed";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
			stringstream secret;
			if (GetOpenFileNameA(&ofn))
			{
				ifstream file(filename);

				secret << file.rdbuf();
				file.close();
			}
			cout << "How many bits do you want to embed? (1, 2, or 4)" << endl;
			cin >> input;
			stegoImage = LSBembed(coverImage, secret.str(), stoi(input));
			LSBdisembed(stegoImage, stoi(input));
			cv::namedWindow("stego");
			cv::imshow("stego", stegoImage);
			cv::imwrite("stegoImage.png", stegoImage);
			cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage) << " " << PSNR(coverImage, stegoImage) << endl;
			cv::waitKey(0);
			cv::destroyAllWindows();
		}
		else if (input == "2") {
			char filename[MAX_PATH];

			OPENFILENAME ofn;
			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Images\0*.png\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select an image to embed in";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&ofn))
			{
				coverImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);
				if (coverImage.empty())
				{
					cout << "Could not open or find the image" << std::endl;
					return -1;
				}
				cv::namedWindow("cover");
				cv::imshow("cover", coverImage);
			}

			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Text Files\0*.txt\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select the message you want to embed";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
			stringstream secret;
			if (GetOpenFileNameA(&ofn))
			{
				ifstream file(filename);

				secret << file.rdbuf();
				file.close();
			}
			cout << "Which quantization table do you want to use? (1 - low capacity, 2 - high capacity)" << endl;
			cin >> input;
			stegoImage = PVDembed(coverImage, secret.str(), stoi(input));
			PVDdisembed(stegoImage, stoi(input));
			cv::namedWindow("stego");
			cv::imshow("stego", stegoImage);
			cv::imwrite("stegoImage.png", stegoImage);
			cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage) << " " << PSNR(coverImage, stegoImage) << endl;
			cv::waitKey(0);
			cv::destroyAllWindows();
		}
		else if (input == "3") {
			char filename[MAX_PATH];

			OPENFILENAME ofn;
			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Images\0*.png\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select an image to embed in";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&ofn))
			{
				coverImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);
				if (coverImage.empty())
				{
					cout << "Could not open or find the image" << std::endl;
					return -1;
				}
				cv::namedWindow("cover");
				cv::imshow("cover", coverImage);
			}

			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Text Files\0*.txt\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select the message you want to embed";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
			stringstream secret;
			if (GetOpenFileNameA(&ofn))
			{
				ifstream file(filename);

				secret << file.rdbuf();
				file.close();
			}
			cout << "What width do you want to use? (odd number)" << endl;
			cin >> input;
			tuple<cv::Mat, int> result = EDS(coverImage, secret.str(), stoi(input));
			stegoImage = get<0>(result);
			int stegoKey = get<1>(result);
			EDSdisembed(stegoImage, stegoKey);
			cv::namedWindow("stego");
			cv::imshow("stego", stegoImage);
			cv::imwrite("stegoImage.png", stegoImage);
			cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage) << " " << PSNR(coverImage, stegoImage) << endl;
			cv::waitKey(0);
			cv::destroyAllWindows();
		}
		else if (input == "4") {
			char filename[MAX_PATH];

			OPENFILENAME ofn;
			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Images\0*.png\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select an image to embed in";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&ofn))
			{
				coverImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);
				if (coverImage.empty())
				{
					cout << "Could not open or find the image" << std::endl;
					return -1;
				}
				cv::namedWindow("cover");
				cv::imshow("cover", coverImage);
			}

			coverImage.convertTo(coverImage, CV_32FC1, 1.0 / 255);
			cout << "Which variant of DCT algorithm do you want? (1 - AC, 2 - DC)" << endl;
			cin >> input;
			if (input == "1") {
				cout << "Embed text or image? (1 - text, 2 - image)" << endl;
				cin >> input;
				if (input == "1") {
					ZeroMemory(&filename, sizeof(filename));
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
					ofn.lpstrFilter = "Text Files\0*.txt\0Any File\0*.*\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrTitle = "Select the message you want to embed";
					ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
					stringstream secret;
					if (GetOpenFileNameA(&ofn))
					{
						ifstream file(filename);

						secret << file.rdbuf();
						file.close();
					}
					stegoImage = DCT_AC(coverImage, secret.str());
					DCT_ACDisembed(stegoImage, secret.str().size());
					cv::namedWindow("stego");
					cv::imshow("stego", stegoImage);
					cv::Mat temp;
					stegoImage.convertTo(temp, CV_8UC1, 255);
					cv::imwrite("stegoImage.png", temp);
					cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage, 0) << " " << PSNR(coverImage, stegoImage, 0) << endl;
					cv::waitKey(0);
					cv::destroyAllWindows();
				}
				else if (input == "2") {
					ZeroMemory(&filename, sizeof(filename));
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
					ofn.lpstrFilter = "Images\0*.png\0Any File\0*.*\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrTitle = "Select the image you want to embed";
					ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn))
					{
						cv::Mat imageToHide = cv::imread(filename, cv::IMREAD_GRAYSCALE);
						if (imageToHide.empty())
						{
							cout << "Could not open or find the image" << std::endl;
							return -1;
						}
						cv::namedWindow("to hide");
						cv::imshow("to hide", imageToHide);
						stegoImage = DCT_AC(coverImage, imageToHide);
						cv::imshow("stego", stegoImage);
						DCT_ACDisembed(stegoImage, imageToHide.rows, imageToHide.cols);
						cv::namedWindow("stego");
						cv::imshow("stego", stegoImage);
						cv::Mat temp;
						stegoImage.convertTo(temp, CV_8UC1, 255);
						cv::imwrite("stegoImage.png", temp);
						cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage, 0) << " " << PSNR(coverImage, stegoImage, 0) << endl;
						cv::waitKey(0);
						cv::destroyAllWindows();
					}
				}
			}
			else if (input == "2") {
				cout << "Embed text or image? (1 - text, 2 - image)" << endl;
				cin >> input;
				if (input == "1") {
					ZeroMemory(&filename, sizeof(filename));
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
					ofn.lpstrFilter = "Text Files\0*.txt\0Any File\0*.*\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrTitle = "Select the message you want to embed";
					ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
					stringstream secret;
					if (GetOpenFileNameA(&ofn))
					{
						ifstream file(filename);

						secret << file.rdbuf();
						file.close();
					}
					stegoImage = DCT_DC(coverImage, secret.str());
					DCT_DCDisembed(stegoImage, secret.str().size());
					cv::namedWindow("stego");
					cv::imshow("stego", stegoImage);
					cv::Mat temp;
					stegoImage.convertTo(temp, CV_8UC1, 255);
					cv::imwrite("stegoImage.png", temp);
					cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage, 0) << " " << PSNR(coverImage, stegoImage, 0) << endl;
					cv::waitKey(0);
					cv::destroyAllWindows();
				}
				else if (input == "2") {
					ZeroMemory(&filename, sizeof(filename));
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
					ofn.lpstrFilter = "Images\0*.png\0Any File\0*.*\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrTitle = "Select the image you want to embed";
					ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn))
					{
						cv::Mat imageToHide = cv::imread(filename, cv::IMREAD_GRAYSCALE);
						if (imageToHide.empty())
						{
							cout << "Could not open or find the image" << std::endl;
							return -1;
						}
						cv::namedWindow("to hide");
						cv::imshow("to hide", imageToHide);
						stegoImage = DCT_DC(coverImage, imageToHide);
						cv::imshow("stego", stegoImage);
						DCT_DCDisembed(stegoImage, imageToHide.rows, imageToHide.cols);
						cv::namedWindow("stego");
						cv::imshow("stego", stegoImage);
						cv::Mat temp;
						stegoImage.convertTo(temp, CV_8UC1, 255);
						cv::imwrite("stegoImage.png", temp);
						cout << setprecision(20) << endl << fixed << MSE(coverImage, stegoImage, 0) << " " << PSNR(coverImage, stegoImage, 0) << endl;
						cv::waitKey(0);
						cv::destroyAllWindows();
					}
				}
			}

		}
	}

	return 0;
}