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

using namespace std;

int isInside(Mat img, int row, int col) {
	if ((row < img.rows) && (col < img.cols) && (row >= 0) && (col >= 0)) return 1;
	return 0;
}

void saveImage(Mat src, String fileName) {

	Mat dst;

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

	String s = "Images/" + fileName + ".bmp";
	cout << s;
	imwrite(s, src); //writes the destination to file

	imshow(WIN_SRC, src);

	printf("Press any key to continue ...\n");
	waitKey(0);
}

//Prop Geom

Mat_<uchar> getObjectFromImage(Mat src, Vec3b objectPixel) {
	Mat_<uchar> img(src.rows, src.cols);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<Vec3b>(i, j) == objectPixel) {
				img(i, j) = 255;
			}
			else img(i, j) = 0;
		}
	}
	return img;
}

int calculateObjectArea(Mat src, Vec3b objectPixel) {
	int numberOfObjectPixels = 0;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<Vec3b>(i, j) == objectPixel) {
				numberOfObjectPixels++;
			}
		}
	}

	return numberOfObjectPixels;
}

Point objectCenterOfMass(Mat src, Vec3b objectPixel, int objectArea) {//int* massCenterX, int* massCenterY
	float x = 0;
	float y = 0;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<Vec3b>(i, j) == objectPixel) {
				x += (float)j / objectArea;
				y += (float)i / objectArea;
			}
		}
	}

	return Point(x, y);
}

void elongationAxisOperands(Mat src, Vec3b objectPixel, Point massCenter, float* a, float* b, float* c) {
	float x = 0;
	float y = 0;
	float z = 0;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<Vec3b>(i, j) == objectPixel) {
				/*
				x += (i - massCenterY) * (j - massCenterX);
				y += (i - massCenterY) * (i - massCenterY);
				z += (j - massCenterX) * (j - massCenterX);*/
				x += (i - massCenter.y) * (j - massCenter.x);
				y += (i - massCenter.y) * (i - massCenter.y);
				z += (j - massCenter.x) * (j - massCenter.x);
			}
		}

		*a = y;
		*b = x;
		*c = z;
	}
}

double long elongationAxisTangent(float a, float b, float c) {
	return atan2(2 * b, c - a) / 2; //in curs avem a-c
}

Point mirrorPoint(Point p) {
	return Point(-p.x, -p.y);
}

Point rotatePoint(Point p, long double angle) {
	int x, y;
	x = p.x * std::cos(angle) - p.y * std::sin(angle);
	y = p.x * std::sin(angle) + p.y * std::cos(angle);

	return Point(x, y);
}

bool isBorder4V(Mat src, Vec3b objectPixel, Vec3b bg, int i, int j) {
	return (isInside(src, i + 1, j) && (src.at<Vec3b>(i + 1, j) == bg)) || (isInside(src, i - 1, j) && (src.at<Vec3b>(i - 1, j) == bg)) || (isInside(src, i, j - 1) && (src.at<Vec3b>(i, j - 1) == bg)) || (isInside(src, i, j + 1) && (src.at<Vec3b>(i, j + 1) == bg));
}

int calculateObjectPerimeter(Mat src, Vec3b objectPixel, Vec3b bg) {
	int P = 0;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<Vec3b>(i, j) == objectPixel) {
				if (isBorder4V(src, objectPixel, bg, i, j)) {
					P++;
				}
			}
		}
	}
	return P;
}

float thinnessRatio(int area, int perimeter) {
	return 4 * PI * (float)area / (perimeter * perimeter);
}

float aspectRatio(Mat src, Vec3b objectPixel) {
	int cmax = -1;
	int cmin = src.cols;
	int rmax = -1;
	int rmin = src.rows;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src.at<Vec3b>(i, j) == objectPixel) {
				cmax = max(cmax, j);
				cmin = min(cmin, j);
				rmax = max(rmax, i);
				rmin = min(rmin, i);
			}
		}
	}

	return (float)(cmax - cmin + 1) / (rmax - rmin + 1);
}

void drawObjectContour(Mat src, Mat* dst, Vec3b objectPixel, Vec3b backgroundColour, Vec3b borderColour = Vec3b(0, 0, 0)) {

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if ((src.at<Vec3b>(i, j) == objectPixel) && (isBorder4V(src, objectPixel, backgroundColour, i, j))) {
				(*dst).at<Vec3b>(i, j) = borderColour;
			}
		}
	}
}


void drawMassCenter(Mat* src, Point center, int offset, Vec3b color = Vec3b(0, 0, 0)) {
	//(*src).at<Vec3b>(massCenterY, massCenterX) = color;
	//int massCenterX, int massCenterY

	line(*src, Point(center.x - offset, center.y + offset), center, color, 1.5);
	line(*src, Point(center.x + offset, center.y - offset), center, color, 1.5);
	line(*src, Point(center.x - offset, center.y - offset), center, color, 1.5);
	line(*src, Point(center.x + offset, center.y + offset), center, color, 1.5);

}

void drawVerticalProjection(Mat* src, Vec3b objectPixel, Vec3b color = Vec3b(0, 0, 0)) {
	for (int j = 0; j < (*src).cols; j++) {
		int sum = 0;
		for (int i = 0; i < (*src).rows; i++) {
			if ((*src).at<Vec3b>(i, j) == objectPixel) {
				sum++;
			}
		}
		(*src).at<Vec3b>(sum, j) = color;
	}
}

void drawHorizontalProjection(Mat* src, Vec3b objectPixel, Vec3b color = Vec3b(0, 0, 0)) {
	for (int i = 0; i < (*src).rows; i++) {
		int sum = 0;
		for (int j = 0; j < (*src).cols; j++) {
			if ((*src).at<Vec3b>(i, j) == objectPixel) {
				sum++;
			}
		}
		(*src).at<Vec3b>(i, sum) = color;
	}
}


void onSelectedObject(int event, int x, int y, int flags, void* param)
{
	Mat* src = (Mat*)param;
	Vec3b originalPixelLabel = (*src).at<Vec3b>(y, x);
	Vec3b backgroundColour = Vec3b(0, 0, 0);
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		if (originalPixelLabel == backgroundColour) { printf("Select an object!\n"); return; }

		//Aria

		int area = calculateObjectArea(*src, originalPixelLabel);
		printf("Area of object: %d\n", area);

		//Centrul de masa
		Point massCenter(-1, -1);
		//objectCenterOfMass(*src, originalPixelLabel, massCenterX, massCenterY, area);
		massCenter = objectCenterOfMass(*src, originalPixelLabel, area);
		if (massCenter != Point(-1, -1)) {
			//printf("Center of mass: (%d,%d)\n", *massCenterY, *massCenterX);
			std::cout << "Center of mass: " << massCenter << std::endl;
		}
		else
		{
			printf("Error at finding the center of mass coordinates.\n");
		}

		//Axa de alungire - Axis of elongation
		float* a = (float*)malloc(sizeof(float));
		float* b = (float*)malloc(sizeof(float));
		float* c = (float*)malloc(sizeof(float));
		//float* angle = (float*)malloc(sizeof(float));
		*a = -1; *b = -1; *c = -1;
		//auto angle = -999;
		elongationAxisOperands(*src, originalPixelLabel, massCenter, a, b, c);
		if ((*a != -1) && (*b != -1) && (*c != -1)) {
			auto angle = elongationAxisTangent(*a, *b, *c);
			//printf("Axis of elongation angle: %f\n", *angle);
			std::cout << "Axis of elongation angle: " << angle << std::endl;

		}
		else {
			printf("Error at calculating operands for axis of elongation.\n");
		}

		//Perimetrul - Perimeter
		int perimeter = calculateObjectPerimeter(*src, originalPixelLabel, backgroundColour);
		printf("Object Perimeter: %d\n", perimeter); //* (PI / 4)); ?

		//Thiness ratio - circularity
		float thinRatio = thinnessRatio(area, perimeter);
		printf("Thiness ratio: %f\n", thinRatio);

		//Elongatia - Aspect Ratio
		float aspRatio = aspectRatio(*src, originalPixelLabel);
		printf("Aspect ratio: %f\n", aspRatio);


		printf("\n");
	}
}

void processObject(Mat src) {
	//Mat src;
	// Read image from file 
	//char fname[MAX_PATH];
		//src = imread(fname); // IMREAD_COLOR
		//Create a window
	namedWindow("My Window", 1);

	//set the callback function for any mouse event
	setMouseCallback("My Window", onSelectedObject, &src);
	//nu aici sa procesam obiectele

	//show the image
	imshow("My Window", src);

	// Wait until user press some key
	waitKey(0);

}

//Filtrare 

typedef struct {
	Vec3b label;
	long double area = -1;
	float orientationPhi = -1.0f;
	float thinness = -1.0f;
}ObjectData;

long double getObjectAreaByLabel(std::vector<ObjectData> objectData, Vec3b color) {
	for (auto labelC : objectData) {
		if (color == labelC.label) return labelC.area;
	}
	return -1;
}

float getObjectOrientationByLabel(std::vector<ObjectData> objectData, Vec3b color) {
	for (auto labelC : objectData) {
		if (color == labelC.label) return labelC.orientationPhi;
	}
	return -1.0f;
}

float getObjectThinnessByLabel(std::vector<ObjectData> objectData, Vec3b color) {
	for (auto labelC : objectData) {
		if (color == labelC.label) return labelC.thinness;
	}
	return -1.0f;
}

void filterObjectsByArea(Mat* src, Vec3b backgroundColour, int area_LOW, int area_HIGH) {
	std::vector<ObjectData> objectData;
	ObjectData obj;

	for (int i = 0; i < src->rows; i++) {
		for (int j = 0; j < src->cols; j++) {
			if (src->at<Vec3b>(i, j) == backgroundColour) continue; //sarim peste daca e pixel fundal
			auto area = getObjectAreaByLabel(objectData, src->at<Vec3b>(i, j));
			if (area != -1) {
				if (!((area >= area_LOW) && (area <= area_HIGH)))
					src->at<Vec3b>(i, j) = backgroundColour;
			}
			else
			{
				//Daca am dat de o eticheta noua
				obj.area = calculateObjectArea(*src, src->at<Vec3b>(i, j));
				obj.label = src->at<Vec3b>(i, j);
				objectData.push_back(obj);
				//printf("(Label,Area): ((%d, %d, %d), %.2lf)\n", obj.label[0], obj.label[1], obj.label[2], obj.area);

			}

			//continue;
		}

	}
}

void filterObjectsByOrientation(Mat* src, Vec3b backgroundColour, float phi_LOW, float phi_HIGH) {
	std::vector<ObjectData> objectData;
	ObjectData obj;

	for (int i = 0; i < src->rows; i++) {
		for (int j = 0; j < src->cols; j++) {
			if (src->at<Vec3b>(i, j) == backgroundColour) continue; //sarim peste daca e pixel fundal
			auto phi = getObjectOrientationByLabel(objectData, src->at<Vec3b>(i, j));
			if (phi != -1.0f) {
				if (!((phi < phi_HIGH) && (phi > phi_LOW)))
					src->at<Vec3b>(i, j) = backgroundColour;
			}
			else
			{
				//Daca am dat de o eticheta noua
				obj.area = calculateObjectArea(*src, src->at<Vec3b>(i, j));
				Point massCenter = objectCenterOfMass(*src, src->at<Vec3b>(i, j), obj.area);
				float* a = (float*)malloc(sizeof(float));
				float* b = (float*)malloc(sizeof(float));
				float* c = (float*)malloc(sizeof(float));
				elongationAxisOperands(*src, src->at<Vec3b>(i, j), massCenter, a, b, c);
				obj.orientationPhi = elongationAxisTangent(*a, *b, *c);
				obj.label = src->at<Vec3b>(i, j);
				objectData.push_back(obj);
				printf("(Label,Phi): ((%d, %d, %d), %f)\n", obj.label[0], obj.label[1], obj.label[2], obj.orientationPhi);

			}

			//continue;
		}

	}
}

void filterObjectsByThinness(Mat* src, Vec3b backgroundColour, float thinnessTH) {
	std::vector<ObjectData> objectData;
	ObjectData obj;

	for (int i = 0; i < src->rows; i++) {
		for (int j = 0; j < src->cols; j++) {
			if (src->at<Vec3b>(i, j) == backgroundColour) continue; //sarim peste daca e pixel fundal
			auto thinness = getObjectThinnessByLabel(objectData, src->at<Vec3b>(i, j));
			if (thinness != -1) {
				if (!(thinness < thinnessTH))
					src->at<Vec3b>(i, j) = backgroundColour;
			}
			else
			{
				//Daca am dat de o eticheta noua
				auto area = calculateObjectArea(*src, src->at<Vec3b>(i, j));
				auto perimeter = calculateObjectPerimeter(*src, src->at<Vec3b>(i, j), backgroundColour);
				obj.thinness = thinnessRatio(area, perimeter);
				obj.label = src->at<Vec3b>(i, j);
				objectData.push_back(obj);
				//printf("(Label,Thinness Factor): ((%d, %d, %d), %.2lf)\n", obj.label[0], obj.label[1], obj.label[2], obj.area);

			}

			//continue;
		}

	}
}

//Etichetare
vector<Point> neighboursN4 = { {0,-1}, {-1, 0}, {0, 1}, {1, 0} };
vector<Point> neighboursN8 = { {0,-1}, {-1, 0}, {0, 1}, {1, 0},{-1,-1},{-1,1},{1,-1},{1,1} };

Mat_<int> labelBFS(Mat_<uchar> src, int objectPixel, int V = 1) {

	Mat_<int> labels(src.rows, src.cols);
	labels.setTo(0);
	int label = 0;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if ((src(i, j) == objectPixel) && (labels(i, j) == 0)) {
				label++;
				queue<Point> Q;
				labels(i, j) = label;
				Q.push({ j,i });
				while (!Q.empty()) {
					Point P = Q.front();
					Q.pop();
					if (V == 0) {
						for (auto n : neighboursN4) {
							Point n2 = P + n;
							if (isInside(src, n2.y, n2.x))
								if ((src(n2.y, n2.x) == objectPixel) && (labels(n2.y, n2.x) == 0)) {
									labels(n2.y, n2.x) = label;
									Q.push(n2);
								}
						}
					}
					else if (V == 1) {
						for (auto n : neighboursN8) {
							Point n2 = P + n;
							if (isInside(src, n2.y, n2.x))
								if ((src(n2.y, n2.x) == objectPixel) && (labels(n2.y, n2.x) == 0)) {
									labels(n2.y, n2.x) = label;
									Q.push(n2);
								}
						}
					}

				}

			}
		}
	}

	return labels;

}

typedef struct {
	int label = 0;
	Vec3b colour = Vec3b(0, 0, 0);
}LabelColour;

Vec3b findLabel(vector<LabelColour> labelCol, int label) {
	for (auto el : labelCol)
		if (el.label == label) return el.colour;
	return Vec3b(0, 0, 0);
}

Mat_<Vec3b> colourForLabels(Vec3b backgroundColour, Mat_<int> labels) {
	Mat_<Vec3b> dst(labels.rows, labels.cols);
	default_random_engine gen;
	uniform_int_distribution<int>d(1, 255);
	uchar x = d(gen);
	vector<LabelColour> labelCol;

	for (int i = 0; i < labels.rows; i++) {
		for (int j = 0; j < labels.cols; j++) {
			if ((findLabel(labelCol, labels(i, j)) == Vec3b(0, 0, 0)) && (labels(i, j) != 0)) {
				LabelColour y;
				y.label = labels(i, j);
				y.colour[0] = x;
				x = d(gen);
				y.colour[1] = x;
				x = d(gen);
				y.colour[2] = x;
				x = d(gen);
				labelCol.push_back(y);
			}
			if (labels(i, j) != 0)
				dst(i, j) = findLabel(labelCol, labels(i, j));
			else
				dst(i, j) = backgroundColour;
		}
	}

	return dst;
}

void binarizeLabelled(Mat_<uchar>& src, Mat_<Vec3b> labelledImg, Vec3b bg = Vec3b(0, 0, 0)) {
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (labelledImg(i, j) != bg) src(i, j) = 255;
			else src(i, j) = 0;
		}
	}
}


Mat getBinary() {
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname, IMREAD_GRAYSCALE);
		Mat th2 = Mat(src.rows, src.cols, CV_8UC1);
		Mat th3 = Mat(src.rows, src.cols, CV_8UC1);
		Mat th4 = Mat(src.rows, src.cols, CV_8UC1);
		double C = 0.0f;
		adaptiveThreshold(src, th2, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 15);
		adaptiveThreshold(src, th3, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 7);
		adaptiveThreshold(src, th4, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 11, 12);

		return th4;
	}

}

//Eroziune si dilatare

int xPos[] = { -1,1,0,0,-1,-1,1,1 };
int yPos[] = { 0,0,1,-1,1,-1,-1,1 };

Mat_<uchar> dilate(Mat_<uchar> src, int n) {
	Mat_<uchar> prev = src;
	Mat_<uchar> dst = Mat(src.rows, src.cols, CV_8UC1);
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				if (prev(i, j) == 0) {
					bool isBackgroundPixel = true;
					for (int pos = 0; pos < 8; pos++) {

						if (!((xPos[pos] == -1 && j == 0) || (yPos[pos] == -1 && i == 0) || (xPos[pos] == 1 && j == src.cols - 1) || (yPos[pos] == 1 && i == src.rows - 1)))
						{
							if (prev(i + yPos[pos], j + xPos[pos]) == 255)
							{
								isBackgroundPixel = false;
								break;
							}
						}
					}
					if (isBackgroundPixel)
						dst(i, j) = 0;
					else
						dst(i, j) = 255;
				}
				else
					dst(i, j) = 255;
			}
		}
		prev = dst;
		dst = Mat(src.rows, src.cols, CV_8UC1);
	}

	return prev;
}

//Eroziune

Mat_<uchar> erode(Mat_<uchar> src, int n) {
	Mat_<uchar> prev = src;
	Mat_<uchar> dst = Mat(src.rows, src.cols, CV_8UC1);
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				if (prev(i, j) == 255) {
					bool isObjectPixel = true;
					//std::cout <<std::endl<< i << "," << j << std::endl;
					for (int pos = 0; pos < 8; pos++) {

						if (!((xPos[pos] == -1 && j == 0) || (yPos[pos] == -1 && i == 0) || (xPos[pos] == 1 && j == src.cols - 1) || (yPos[pos] == 1 && i == src.rows - 1)))
						{
							//std::cout << (int) prev(i + xPos[pos], j + yPos[pos])<<" , ";
							if (prev(i + yPos[pos], j + xPos[pos]) == 0)
							{
								//std::cout << i << "," << j << std::endl;
								isObjectPixel = false;
								break;
							}
						}
					}
					if (isObjectPixel)
						dst(i, j) = 255;
					else
						dst(i, j) = 0;
				}
				else
					dst(i, j) = 0;
			}
		}
		prev = dst;
		dst = Mat(src.rows, src.cols, CV_8UC1);
	}

	return prev;
}

/*
	!!Deci functia asta face si ea direct binarizarea. Nu mai trebuie apelata chestia din openCV cu adaptiveThreshold
	Faza e ca imi face putin ciudat pe unele imagini si s-ar putea sa fie nevoie sa egalizez putin histograma inainte prin functii care se joaca cu contrastul
	(mai ales la alea cu pete mari)
	Am gasit niste chestii din opencv pentru asta, dar nu rea mi-au iesit
	https://docs.opencv.org/3.4/d2/d74/tutorial_js_histogram_equalization.html#:~:text=In%20adaptive%20histogram%20equalization%2C%20image,there%2C%20it%20will%20be%20amplified.

*/
int otsuThresh(Mat_<uchar> src) {
	
	unsigned int hist[256];
	uchar thresh = 0;
	int max_var = 0;
	unsigned long sum = 0, sum2 = 0, var;
	for (int i = 0; i < 255; i++)
		hist[i] = 0;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			hist[src(i, j)]++;
		}
	}
	for (int i = 0; i < 255; i++) {
		sum += i * hist[i];
	}
	std::cout << "Sum: " << sum << std::endl;
	unsigned long q1 = 0, q2 = 0;
	unsigned long N = src.rows * src.cols;
	for (int i = 0; i < 255; i++) {
		q1 += hist[i];
		if (q1 == 0)
			continue;
		if (q1 == N)
			break;
		q2 = N - q1;
		sum2 += i * hist[i];
		int u1 = sum2 / q1;
		int u2 = (sum - sum2) / q2;
		int dif = u1 - u2;
		int var = q1 * q2 * dif * dif;
		if (var > max_var) {
			max_var = var;
			thresh = i;
		}

	}
	std::cout << "Thresh found: " << thresh << std::endl;
	/*Mat_<uchar> dst = Mat(src.rows, src.cols, CV_8UC1);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src(i, j) < thresh)
				dst(i, j) = 255;
			else
				dst(i, j) = 0;
		}
	}*/
	//std::cout << "Otsu thresh done\n";
	return thresh;
}