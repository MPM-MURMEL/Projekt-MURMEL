#ifndef _KEYHOLEDETECTION_HPP_
#define _KEYHOLEDETECTION _HPP_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/core/core.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <math.h>


int getROI(cv::Mat &frame, cv::Rect &roi);


int getCircles(cv::Mat &frame, std::vector<cv::Vec3f> &circles);


cv::Vec3f calculateCoordinates(double x_px, double y_px, double r_px);


int getKeyhole(cv::Mat &frame_gray, cv::Mat &template_gray, cv::Vec3f &keyhole);


#endif