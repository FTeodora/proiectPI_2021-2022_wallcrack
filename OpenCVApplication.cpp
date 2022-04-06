// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

#include "func.h"


#define THRESH 127
#define THINNESS_THRESH 0.25
#define AREA_THRESH_LOW 10
#define AREA_THRESH_HIGH 200


//Tip de vecinatate
enum {
	N4, N8
};

Mat_<uchar> process(Mat_<uchar> src) {

	Vec3b bg = Vec3b(0, 0, 0);
	Mat_<uchar> finalImg(src.rows, src.cols);

	//Pas 1 : avem imaginea binarizata

	//Pas 2 : facem dilatarea
	int dilation_size = 1; //dim elementului structural

	Mat elementD = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));

	Mat_<uchar> dilatedImg;
	dilate(src, dilatedImg, elementD, Point(-1, -1), 4); //ult param e nr de iteratii


	//Pas 3 : facem eroziunea
	int erosion_size = 3;

	Mat elementE = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	Mat_<uchar> erodedImg;
	erode(dilatedImg, erodedImg, elementE, Point(-1, -1), 1);


	//Pas 4 : etichetarea imaginii
	Mat_<uchar> labels = labelBFS(erodedImg, 255, N8);
	Mat_<Vec3b> labelledImg = colourForLabels(Vec3b(0, 0, 0), labels);

	//functia pentru aflarea proprietatilor geometrice
	//processObject(labelledImg);

	//Pas 5 + final : filtrarea obiectelor
	//obiecte cu thinness mai mic decat thresh
	//filterObjectsByThinness(&labelledImg, bg, THINNESS_THRESH);
	//filterObjectsByArea(&labelledImg, bg, AREA_THRESH_LOW, AREA_THRESH_HIGH);


	Mat_<uchar>filteredImg(src.rows, src.cols);
	binarizeLabelled(filteredImg, labelledImg);

	Mat_<uchar> finalImg(src.rows, src.cols);
	finalImg = erode(filteredImg, 4);

	finalImg = filteredImg.clone();

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
		Mat_<uchar> src = getBinary();
		Mat_<uchar> finalImg(src.rows, src.cols);
		finalImg = process(src);
		imshow("imagine finala", finalImg);
		waitKey();
	}

	return 0;
}