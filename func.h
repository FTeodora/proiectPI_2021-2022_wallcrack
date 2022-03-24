#pragma once

int isInside(Mat img, int row, int col);
void saveImage(Mat src, String fileName);

Mat_<uchar> getObjectFromImage(Mat src, Vec3b objectPixel);
int calculateObjectArea(Mat src, Vec3b objectPixel);
Point objectCenterOfMass(Mat src, Vec3b objectPixel, int objectArea);
void elongationAxisOperands(Mat src, Vec3b objectPixel, Point massCenter, float* a, float* b, float* c);
double long elongationAxisTangent(float a, float b, float c);

Point mirrorPoint(Point p); //utils
Point rotatePoint(Point p, long double angle); //utils

bool isBorder4V(Mat src, Vec3b objectPixel, Vec3b bg, int i, int j);
int calculateObjectPerimeter(Mat src, Vec3b objectPixel, Vec3b bg);
float thinnessRatio(int area, int perimeter);
float aspectRatio(Mat src, Vec3b objectPixel);


Point operator*(const Point& p1, const Point& p2);//utils
void drawMassCenter(Mat* src, Point center, int offset, Vec3b color = Vec3b(0, 0, 0));
void drawVerticalProjection(Mat* src, Vec3b objectPixel, Vec3b color = Vec3b(0, 0, 0));
void drawHorizontalProjection(Mat* src, Vec3b objectPixel, Vec3b color = Vec3b(0, 0, 0));
//void drawObjectContour(Mat src, Mat* dst, Vec3b objectPixel, Vec3b backgroundColour, Vec3b borderColour = Vec3b(0, 0, 0));


//Functions with object processing
void processObject(Mat src);
void filterObjectsByArea(Mat* src, Vec3b backgroundColour, int areaTH);
void filterObjectsByOrientation(Mat* src, Vec3b backgroundColour, float phi_LOW, float phi_HIGH);

Mat_<int> labelBFS(Mat_<uchar> src, int objectPixel, int V = 1);
Mat_<Vec3b> colourForLabels(Vec3b backgroundColour, Mat_<int> labels);
void filterObjectsByThinness(Mat* src, Vec3b backgroundColour, float thinnessTH);

void binarizeLabelled(Mat_<uchar>& src, Mat_<Vec3b> labelledImg, Vec3b bg = Vec3b(0, 0, 0));

//Eroziune+Dilatare
Mat_<uchar> dilate(Mat_<uchar> src, int n);
Mat_<uchar> erode(Mat_<uchar> src, int n);