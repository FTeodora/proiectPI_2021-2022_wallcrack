#include "stdafx.h"
#include "common.h"
#include <iostream>
#include <queue>
#include <random>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <vector>
#include "mainf.h"
#include "func.h"


#define THRESH 127
#define THINNESS_THRESH 0.40
#define AREA_THRESH_LOW 1
#define AREA_THRESH_HIGH 200


//Tip de vecinatate
enum {
	N4, N8
};

//typedef struct {
//	Vec3b label;
//	long double area = -1;
//	int perim = -1;
//	float orientationPhi = -1.0f;
//	float thinness = -1.0f;
//}ObjectData;

using namespace std;

Mat_<uchar> show_mask(Mat_<uchar> imgSrc, Mat_<uchar> img) {

	Mat_<uchar> maskImg(img.rows, img.cols);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			uchar pixel = img.at<uchar>(i, j);
			if (pixel == 255) {
				maskImg.at<uchar>(i, j) = imgSrc.at<uchar>(i, j);
			}
			else {
				maskImg.at<uchar>(i, j) = 0;
			}
		}
	}
	return maskImg;
}

//Mat_<uchar> processNTimes(int n, Mat_<uchar> src) {
//	for (int i = 0; i < n; i++)
//		src = process(src);
//	return src;
//
//}

filteredData processResultsReturnFiltered(Mat src, Mat binImg) {


	//Mat_<uchar> labels = labelBFS(binImg, 255, N8);
	//Mat_<Vec3b> labelledImg = colourForLabels(Vec3b(0, 0, 0), labels);

	//daca am primi doua imagini binare si etichetam unde dintre ele aici

	int th = -1;
	filteredData data;

	Mat_<uchar> labels = labelBFS(binImg, 255, 1);
	Mat_<Vec3b> labelledSrc = colourForLabels(Vec3b(0, 0, 0), labels);

	Vec3b bg = Vec3b(0, 0, 0);
	vector<Vec3b> visitedLabels;

	//imshow("labelledSrc", labelledSrc);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			//if (labelledImg.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
			//	//ObjectData obj = createObjectFromLabel(&labelledImg, labelledImg.at<Vec3b>(i, j));
			//	if (!isLabelInVector(visitedLabels, labelledImg.at<Vec3b>(i, j))) {
			//		ObjectData obj = createObjectFromLabel(&labelledImg, labelledImg.at<Vec3b>(i, j));
			//		if (src.at<uchar>(i, j) == 255 && obj.area > 2) {
			//			remained.push_back(obj);
			//		}
			//		else {
			//			if (src.at<uchar>(i, j) == 0)
			//				removed.push_back(obj);
			//		}

			//		visitedLabels.push_back(labelledImg.at<Vec3b>(i, j));
			//	}
			//}

			/*if (src.at<uchar>(i, j) == 255 && labelledImg.at<Vec3b>(i, j) != Vec3b(0, 0, 0) && !isLabelInVector(visitedLabels, labelledImg.at<Vec3b>(i, j))) {
				ObjectData obj = createObjectFromLabel(&labelledImg, labelledImg.at<Vec3b>(i, j));
				remained.push_back(obj);
				visitedLabels.push_back(labelledImg.at<Vec3b>(i, j));
			}
			else {
				if (src.at<uchar>(i, j) == 255 && labelledImg.at<Vec3b>(i, j) == Vec3b(0, 0, 0)) {
					ObjectData obj2 = createObjectFromLabel(&labelledSrc, labelledSrc.at<Vec3b>(i, j));
					removed.push_back(obj2);

				}
			}*/

			if (src.at<uchar>(i, j) == 255 && labelledSrc.at<Vec3b>(i, j) != bg && !isLabelInVector(visitedLabels, labelledSrc.at<Vec3b>(i, j))) {
				ObjectData obj = createObjectFromLabel(&labelledSrc, labelledSrc.at<Vec3b>(i, j));
				if (obj.area > 2) {
					data.remained.push_back(obj);
					visitedLabels.push_back(labelledSrc.at<Vec3b>(i, j));
				}
				else {
					data.removed.push_back(obj);
					visitedLabels.push_back(labelledSrc.at<Vec3b>(i, j));
				}
			}
			else {
				if (src.at<uchar>(i, j) == 255 && labelledSrc.at<Vec3b>(i, j) == bg ) {
					ObjectData obj = createObjectFromLabel(&labelledSrc, labelledSrc.at<Vec3b>(i, j));
					data.removed.push_back(obj);
					visitedLabels.push_back(labelledSrc.at<Vec3b>(i, j));
					//std::cout << "removed path" << std::endl;
				}
			}

		}

	}


	std::cout << "total objects: " << data.remained.size() + data.removed.size() << std::endl;

	std::cout << "remained objects count: " << data.remained.size() << std::endl;


	//Mat_<uchar> bin(labelledImg.rows, labelledImg.cols);
	//binarizeLabelled(bin, labelledImg);

	return data;
}
RecalculatedParameters computeNextParams(vector<ObjectData> removed, int areaOffset) {
	std::cout <<"Elements to consider for next params "<<  removed.size()<<std::endl;
	RecalculatedParameters avg;
	float averageRemovedArea = 0;
	float averageThinness = 0;
	for (auto obj : removed) {
		averageRemovedArea += obj.area;
		averageThinness += obj.thinness;
	}

	averageRemovedArea /= (float)removed.size();
	averageThinness /= (float)removed.size();
	
	avg.area = round(averageRemovedArea)+areaOffset;
	avg.thinness = averageThinness;
	std::cout << "Recalculated area " << averageRemovedArea<<" and thinness "<<averageThinness << std::endl;
	return avg;
}
Mat_<uchar> process(int iteration,Mat_<uchar> src,float thinness, int avg_area) {
	Vec3b bg = Vec3b(0, 0, 0);
	Mat_<uchar> finalImg(src.rows, src.cols);
	char msg[100];

	//Pas 1.1 : avem imaginea binarizata

	sprintf(msg,"%d.1 - Binary Image", iteration);
	imshow(msg, src);
	//Pas 2 : facem dilatarea
	int dilation_size = 1; //dim elementului structural

	Mat elementD = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));

	Mat_<uchar> dilatedImg;
	dilate(src, dilatedImg, elementD, Point(-1, -1), 4); //ult param e nr de iteratii

	sprintf(msg, "%d.2 - Dilated Image", iteration);
	imshow(msg, dilatedImg);
	//imshow("1.2 - Dilated Image", dilatedImg);


	//Pas 3 : facem eroziunea
	int erosion_size = 3;

	Mat elementE = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	Mat_<uchar> erodedImg;
	erode(dilatedImg, erodedImg, elementE, Point(-1, -1), 1);

	sprintf(msg, "%d.3 - Eroded Image", iteration);
	imshow(msg, erodedImg);
	//imshow("1.3 - Eroded Image", erodedImg);

	Mat_<uchar>erodedCopy = erodedImg.clone();

	//Pas 4 : etichetarea imaginii
	Mat_<uchar> labels = labelBFS(erodedImg, 255, N8);
	Mat_<Vec3b> labelledImg = colourForLabels(Vec3b(0, 0, 0), labels);

	//functia pentru aflarea proprietatilor geometrice
	//processObject(labelledImg);
	sprintf(msg, "%d.4 - Labelled Image", iteration);
	imshow(msg, labelledImg);
	//imshow("1.4 - Labelled Image", labelledImg);

	//Pas 5 + final : filtrarea obiectelor
	//obiecte cu thinness mai mic decat thresh
	//processObject(labelledImg);
	filterObjectsByThinness(&labelledImg, bg, thinness);
	filterObjectsByAreaM(&labelledImg, bg, avg_area);

	
	//imshow("1.5 - Filtered Image", labelledImg);

	//imshow("Eroded Img", erodedCopy);
	//imshow("Labelled Img", labelledImg);
	
	Mat_<uchar>filteredImg(erodedCopy.rows, erodedCopy.cols);
	binarizeLabelled(filteredImg, labelledImg);

	finalImg = erode(filteredImg, 4);

	finalImg = filteredImg.clone();
	finalImg = erodedCopy;

	sprintf(msg, "%d.5 - Filtered Image", iteration);
	imshow(msg, finalImg);
	return finalImg;
}