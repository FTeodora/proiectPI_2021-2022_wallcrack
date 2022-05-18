#pragma once
#include <vector>
#include "func.h"

Mat_<uchar> show_mask(Mat_<uchar> imgSrc, Mat_<uchar> img);
//Mat_<uchar> processNTimes(int n, Mat_<uchar> src);

typedef struct _filtered_data {
	std::vector<ObjectData> remained;
	std::vector<ObjectData> removed;
}filteredData;
typedef struct _res_params {
	float thinness;
	int area;
}RecalculatedParameters;
//rezultat.at(0) - processed image
//rezultat.at(1) - labelled image
Mat_<uchar> process(int iteration,Mat_<uchar> src, float thinness, int avg_area);
filteredData processResultsReturnFiltered(Mat src, Mat labelledImg);
RecalculatedParameters  computeNextParams(std::vector<ObjectData> removed, int areaOffset);
