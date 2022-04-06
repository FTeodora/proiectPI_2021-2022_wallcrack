// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

#include "func.h"


#define THRESH 127
#define THINNESS_THRESH 0.3
#define AREA_THRESH 25000


void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}
void testThresholding() {
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname, IMREAD_GRAYSCALE);
		Mat th2 = Mat(src.rows, src.cols, CV_8UC1);
		Mat th3 = Mat(src.rows, src.cols, CV_8UC1);
		Mat th4 = Mat(src.rows, src.cols, CV_8UC1);
		double C = 0.0f;
		adaptiveThreshold(src, th2, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 2);
		adaptiveThreshold(src, th3, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 7);
		adaptiveThreshold(src, th4, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 12);

		imshow("ResultC=2", th2);
		imshow("ResultC=7", th3);
		imshow("ResultC=12", th4);
		waitKey();
	}
}

Mat getBinary(Mat_<uchar> src) {
	/*char fname[MAX_PATH];
	if (openFileDlg(fname))
	{*/
		//Mat src;
		//src = imread(fname, IMREAD_GRAYSCALE);
		Mat th2 = Mat(src.rows, src.cols, CV_8UC1);
		Mat th3 = Mat(src.rows, src.cols, CV_8UC1);
		Mat th4 = Mat(src.rows, src.cols, CV_8UC1);
		double C = 0.0f;
		adaptiveThreshold(src, th2, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 15);
		adaptiveThreshold(src, th3, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 7);
		adaptiveThreshold(src, th4, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 12);

		/*
		imshow("ResultC=2", th2);
		imshow("ResultC=7", th3);
		imshow("ResultC=12", th4);
		waitKey();*/

		return th4;
	//}

}
void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", CV_LOAD_IMAGE_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, CV_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	printf("Press any key to continue ...\n");
	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Asa se acceseaaza pixelii individuali pt. o imagine cu 8 biti/pixel
		// Varianta ineficienta (lenta)
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar neg = 255 - val;
				dst.at<uchar>(i, j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testParcurgereSimplaDiblookStyle()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// the fastest approach using the �diblook style�
		uchar* lpSrc = src.data;
		uchar* lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i * w + j];
				lpDst[i * w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		// Asa se acceseaaza pixelii individuali pt. o imagine RGB 24 biti/pixel
		// Varianta ineficienta (lenta)
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (r + g + b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// Componentele d eculoare ale modelului HSV
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// definire pointeri la matricele (8 biti/pixeli) folosite la afisarea componentelor individuale H,S,V
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, CV_BGR2HSV);

		// definire pointer la matricea (24 biti/pixeli) a imaginii HSV
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;		// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, dst, gauss;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int)k * pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss, dst, pL, pH, 3);
		imshow("input image", src);
		imshow("canny", dst);
		waitKey();
	}
}

void testVideoSequence()
{
	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, CV_BGR2GRAY);
		Canny(grayFrame, edges, 40, 100, 3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = cvWaitKey(0);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}

void testSnap()
{
	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;

		imshow(WIN_SRC, frame);

		c = cvWaitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115) { //'s' pressed - snapp the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess)
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / PI)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}



//Tip de vecinatate
enum {
	N4, N8
};

Mat_<uchar> process(Mat_<uchar> src) {
	Vec3b bg = Vec3b(0, 0, 0);
	//Mat_<uchar> src = getBinary();

	//Erode

	Mat_<uchar> dilatedImg = dilate(src, 20);
	Mat_<uchar> erodedImg = erode(dilatedImg, 15);


	Mat_<uchar> labels = labelBFS(erodedImg, 255, N8);
	Mat_<Vec3b> labelledImg = colourForLabels(Vec3b(0, 0, 0), labels);

	//saveImage(labelledImg, "imagine_etichetata");

	processObject(labelledImg);

	//imshow("imagine etichetata", labelledImg);

	filterObjectsByArea(&labelledImg, bg, AREA_THRESH);
	filterObjectsByThinness(&labelledImg, bg, THINNESS_THRESH);

	Mat_<uchar>filteredImg(src.rows, src.cols);
	binarizeLabelled(filteredImg, labelledImg);

	Mat_<uchar> finalImg(src.rows, src.cols); //// nu mai trebuie 
	finalImg = erode(filteredImg, 4);


	//imshow("imagine filtrata", filteredImg);
	//imshow("imagine etichetata", labelledImg);
	//imshow("imagine finala", finalImg);
	return finalImg;
}

Mat_<uchar> processNTimes(int n, Mat_<uchar> src) {
	for (int i = 0; i < n; i++)
		src = process(src);
	return src;

}

Mat_<uchar> show_mask(Mat_<uchar> imgSrc, Mat_<uchar> img) {

	Mat_<uchar> maskImg(img.rows, img.cols);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			uchar pixel = img.at<uchar>(i, j);
			if (pixel==255) {
				maskImg.at<uchar>(i, j) = imgSrc.at<uchar>(i, j);
			}
			else {
				/*Vec3b newPixel;
				newPixel[0] = 0;
				newPixel[1] = 0;
				newPixel[2] = 0;*/
				maskImg.at<uchar>(i, j) = 0;
			}
		}
	}
	return maskImg;
}

int main()
{
	while (true) {
		system("cls");
		destroyAllWindows();
		//testThresholding();
		/*Vec3b bg = Vec3b(0, 0, 0);
		Mat_<uchar> src = getBinary();

		//Erode

		Mat_<uchar> dilatedImg = dilate(src, 20);
		Mat_<uchar> erodedImg = erode(dilatedImg, 15);


		Mat_<uchar> labels = labelBFS(erodedImg, 255, N8);
		Mat_<Vec3b> labelledImg = colourForLabels(Vec3b(0, 0, 0), labels);

		//saveImage(labelledImg, "imagine_etichetata");

		processObject(labelledImg);

		imshow("imagine etichetata", labelledImg);

		filterObjectsByArea(&labelledImg, bg, AREA_THRESH);
		filterObjectsByThinness(&labelledImg, bg, THINNESS_THRESH);

		Mat_<uchar>filteredImg(src.rows, src.cols);
		binarizeLabelled(filteredImg, labelledImg);

		Mat_<uchar> finalImg(src.rows, src.cols);
		finalImg = erode(filteredImg, 4);


		imshow("imagine filtrata", filteredImg);
		//imshow("imagine etichetata", labelledImg);
		imshow("imagine finala", finalImg);

		//imshow("imagine erodata", erodedImg);
		//imshow("imagine dilatata", dilatedImg);*/
		int n;
		std::cin >> n;
		Mat src;
		// Read image from file 
		char fname[MAX_PATH];
		while (openFileDlg(fname))
		{
			Mat_<uchar> src = imread(fname,CV_LOAD_IMAGE_GRAYSCALE);
			Mat_<uchar> srcBinary= getBinary(src);
			Mat_<uchar> src1 = srcBinary;
			Mat_<uchar> finalImg(src.rows, src.cols);
			Mat_<uchar> finalImg1(src.rows, src.cols);
			finalImg = processNTimes(1, srcBinary);
			finalImg1 = processNTimes(n, srcBinary);
			imshow("imagine procesata 1 data", finalImg);
			imshow("imagine procesata n ori", finalImg1);
			Mat_<uchar> maskImg = show_mask(src, finalImg);
			imshow("original", src);
			imshow("mask", maskImg);
			waitKey();
		}
	}

	return 0;
}