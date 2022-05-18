#include "impfunc.h"
#include "func.h"
#include "stdafx.h"
#include "common.h"
#include<iostream>
#include <queue>
#include <random>
#include <algorithm>
#include <vector>
#include <iterator>
#include <math.h>
#include <vector>

using namespace std;

#define THRESH 127
#define THINNESS_THRESH 0.40
#define AREA_THRESH_LOW 1
#define AREA_THRESH_HIGH 200


//Tip de vecinatate
enum {
	N4, N8
};

typedef struct {
	Vec3b label;
	uchar grayLevel;
	long double area = -1;
	int perim = -1;
	float orientationPhi = -1.0f;
	float thinness = -1.0f;
}ObjectData;

Mat firstBinary(Mat src, int th) {

	Mat_<uchar> dst(src.rows, src.cols);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<uchar>(i, j) <= th) {
				dst(i, j) = 255;
			}
			else {
				dst(i, j) = 0;
			}
		}
	}

	return dst;
}


void buildObjectVectors(Mat_<uchar> startImg, Mat_<uchar> endImg, vector<ObjectData>* remained, vector<ObjectData>* removed) {
	Vec3b bg = Vec3b(0, 0, 0);
	Mat_<Vec3b> labelledStartImg;
	vector<Vec3b> visitedLabels;
	for (int i = 0; i < labelledStartImg.rows; i++) {
		for (int j = 0; j < labelledStartImg.cols; j++) {
			Vec3b label = labelledStartImg(i, j);
			if ((labelledStartImg(i, j) != bg) && (endImg(i, j) == 0) && (isLabelInVector(visitedLabels, label) == false)) {
				//create object
				ObjectData obj = createObjectFromLabel(&labelledStartImg, label);
				removed->push_back(obj);
			}
		}
	}
}

char* buildWindowTitle(int iteration, int step, char s[]) {
	char buffer[100];
	sprintf(buffer, "%d.%d - ", iteration, step);

	strcat(buffer, s);

	return buffer;
}

Mat_<uchar> process(Mat_<uchar> src, int iteration) {
	Vec3b bg = Vec3b(0, 0, 0);
	Mat_<uchar> finalImg(src.rows, src.cols);

	//Pas 1.1 : avem imaginea binarizata
	char* buffer = buildWindowTitle(1, 1, "Binary Image");
	imshow(buffer, src);
	free(buffer);

	//Pas 2 : facem dilatarea
	int dilation_size = 1; //dim elementului structural

	Mat elementD = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));

	Mat_<uchar> dilatedImg;
	dilate(src, dilatedImg, elementD, Point(-1, -1), 4); //ult param e nr de iteratii

	char* buffer = buildWindowTitle(iteration, 2, "Dilated Image");
	imshow(buffer, src);
	free(buffer);

	//Pas 3 : facem eroziunea
	int erosion_size = 3;

	Mat elementE = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	Mat_<uchar> erodedImg;
	erode(dilatedImg, erodedImg, elementE, Point(-1, -1), 1);

	char* buffer = buildWindowTitle(iteration, 3, "Eroded Image");
	imshow(buffer, src);
	free(buffer);
	imshow("1.3 - Eroded Image", erodedImg);

	Mat_<uchar>erodedCopy = erodedImg.clone();

	//Pas 4 : etichetarea imaginii
	Mat_<uchar> labels = labelBFS(erodedImg, 255, N8);
	Mat_<Vec3b> labelledImg = colourForLabels(Vec3b(0, 0, 0), labels);

	//functia pentru aflarea proprietatilor geometrice
	//processObject(labelledImg);

	//getBinary2(src,labelledImg)

	char* buffer = buildWindowTitle(iteration, 4, "Labelled Image");
	imshow(buffer, src);
	free(buffer);

	//Pas 5 + final : filtrarea obiectelor
	//obiecte cu thinness mai mic decat thresh
	//processObject(labelledImg);
	/*filterObjectsByThinness(&labelledImg, bg, THINNESS_THRESH);
	filterObjectsByArea(&labelledImg, bg, AREA_THRESH_LOW, AREA_THRESH_HIGH);*/

	char* buffer = buildWindowTitle(iteration, 5, "Labelled Image");
	imshow(buffer, src);
	free(buffer);

	//imshow("1.5 - Filtered Image", labelledImg);

	//imshow("Eroded Img", erodedCopy);
	//imshow("Labelled Img", labelledImg);


	//std::cout << "Buna!" << std::endl;
	float avg = getBinary2(erodedCopy, labelledImg);

	/*filterObjectsByAreaM(&labelledImg, bg, avg);*/

	//imshow("Labelled Img After Second Filter", labelledImg);

	Mat_<uchar>filteredImg(src.rows, src.cols);
	/*binarizeLabelled(&filteredImg, labelledImg);*/

	//finalImg = erode(filteredImg, 4);

	finalImg = filteredImg.clone();

	return finalImg;
}