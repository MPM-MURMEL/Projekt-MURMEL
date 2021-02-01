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


/**
* @brief Keyhole detection utilities
*
* A set of functions for the keyhole detection of an bsr trashcan.
* All functions are optimized based on the testsubect.
*
* @example TEST_OpenCV.cpp
* @example TEST_OpenNI.cpp
*/

/**
* @brief Region on interest 
*
* Calculates the overall region of interest based on an orange color scheme
*
* @param frame Image frame to search in.
* @param roi Region of interest
*
* @return If a region of interest was found.
*/
int getROI(cv::Mat &frame, cv::Rect &roi);

/**
* @brief Circle detection 
*
* Findes all circles in a Frame.
*
* @param frame Image frame to search in.
* @param circles Circles found.
*
* @return If circles were detected.
*/
int getCircles(cv::Mat &frame, std::vector<cv::Vec3f> &circles);

/**
* @brief Keyhole koordinates calculation 3DOF
*
* Calculates the distance by using the keyhole width and the focal length of the camera as constants.
* x and y can be calculated based on the shift in the image and the distance.
*
* @param x_px Circle x coordinate in pixels.
* @param y_py Circle y coordinate in pixels.
* @param r_pr Radios of the circle.
*
* @return Coordinates in mm (x,y,z) 
*/
cv::Vec3f calculateCoordinates(double x_px, double y_px, double r_px);

/**
* @brief Keyhole koordinates calculation 5DOF
*
* Calculates the distance by using the keyhole width and the focal length of the camera as constants.
* x and y can be calculated based on the shift in the image and the distance.
* phy_x and phy_y use the depth camera and sample the distance around the keyhole to calculate the angles.
*
* @param depth_frame Depth frame.
* @param x_px Circle x coordinate in pixels.
* @param y_py Circle y coordinate in pixels.
* @param r_pr Radios of the circle.
*
* @return Coordinates in mm and ° (x,y,z,phy_x,phy_y,phy_z) phy_z=0 not used 
*/
cv::Vec6f calculateCoordinates(cv::Mat &depth_frame, double x_px, double y_px, double r_px);

/**
* @brief Keyhole math probability
*
* Tries to match the keyhole in a frame with a template. 
* The result is the probability of the circle been the keyhole.
*
* @param frame_gray Image frame to seach in (grayscale).
* @param template_gray Template image to seach in (grayscale).
* @param keyhole Keyhole circle
*
* @return The probability of been the actual keyhole in %
*/
double calculateMatchProbability(cv::Mat &frame_gray, cv::Mat &template_gray, cv::Vec3f &keyhole);


#endif