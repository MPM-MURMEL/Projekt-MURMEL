#include "KeyholeDetection.hpp"



int getROI(cv::Mat &frame, cv::Rect &roi)
{
	// blur image for smother resulter
	cv::Mat frame_blur;
	cv::blur(frame,frame_blur,cv::Size(30,30));

	// image output for documentation
	//cv::imshow("frame_blur",frame_blur);

	// convert BGR to HSV
	cv::Mat frame_hsv;
	cv::cvtColor(frame_blur, frame_hsv, cv::COLOR_BGR2HSV);

	// calculate lower and upper bound for color detection
	const int HSV_VALUE_MIN = 0;
	const int HSV_VALUE_MAX = 16;
	cv::Scalar LOWER_ORANGE(HSV_VALUE_MIN, 100, 100);
	cv::Scalar UPPER_ORANGE(HSV_VALUE_MAX, 255, 255);
	
	// threshold the HSV image to only get pre define color range
	cv::Mat frame_mask;
	cv::inRange(frame_hsv,LOWER_ORANGE,UPPER_ORANGE, frame_mask);

	// image output for documentation
	//cv::imshow("frame_mask",frame_mask);

	// extract contours from color mask
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(frame_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// retrun false if no contours were detected
	if (contours.size() <= 0) { return 0; }

	int largest_index = 0;
	int largest_area = 0;
	//iterate through contours and find largest area of all contours
	for (int i=0; i<contours.size(); i++)
	{
		// get area of contour
		double a = cv::contourArea(contours[i], false);
	
		// remember lagest index and value
		if(largest_area < a)
		{
			largest_index = i;
			largest_area = a;
		}
	}
	
	// get boundingbox of largest contour
	roi = cv::boundingRect(contours[largest_index]);

	return 1;
}

int getCircles(cv::Mat &frame, std::vector<cv::Vec3f> &circles)
{
	// change saturation and brighness of image for easyser circle detection
	cv::Mat frame_high_constrast = cv::Mat::zeros(frame.size(), frame.type());
	double alpha = 11;
	int beta = 100;

	// loop through all pixel of image
	for (int i=0; i < frame.rows; i++)
	{
		for (int j=0; j < frame.cols; j++)
		{
			for (int c=0; c < 3; c++)
			{
				// apply saturation and brighness of pixel
				frame_high_constrast.at<cv::Vec3b>(i,j)[c] = cv::saturate_cast<uchar>(alpha * (frame.at<cv::Vec3b>(i,j)[c]) + beta);
			}
		}
	}	

	// image output for documentation
	//cv::imshow("frame_high_constrast",frame_high_constrast);

	cv::Mat frame_high_constrast_gray;
	cv::cvtColor(frame_high_constrast, frame_high_constrast_gray, cv::COLOR_BGR2GRAY);

	// image output for documentation
	//cv::imshow("frame_high_constrast_gray",frame_high_constrast_gray);

	// grab keyhole circles via hough transformation 
	cv::HoughCircles(frame_high_constrast_gray, circles, cv::HOUGH_GRADIENT,1,1000,100,20,5,80);

	// check if only one cirle was detected
	if (circles.size()<=0) {return 0;}

	return 1;
}

cv::Vec3f calculateCoordinates(double x_px, double y_px, double r_px)
{	
	// focal length of rgb camera
	const double FOCAL_LENGTH = 470.4; //Orbbec Perseee

	// radius of keyhole in mm
	const double RADIUS = 25/2;

	// coordinate calculations
	double z = FOCAL_LENGTH / r_px * RADIUS;
	double x = x_px / FOCAL_LENGTH * z;
	double y = y_px / FOCAL_LENGTH * z;

	cv::Vec3f result(x,y,z);

	return result;
}

cv::Vec6f calculateCoordinates(cv::Mat &depth_frame, double x_px, double y_px, double r_px)
{	
	// focal length of rgb camera
	const double FOCAL_LENGTH = 470.4; //Orbbec Perseee

	// radius of keyhole in mm
	const double RADIUS = 25/2;

	// radius of orientation samples
	const double SAMPLE_RADIUS = 100;

	// coordinate calculations in mm
	double z = FOCAL_LENGTH / r_px * RADIUS;
	double x = x_px / FOCAL_LENGTH * z;
	double y = y_px / FOCAL_LENGTH * z;

	// get depth of circle center by depth camera
	//double z_depth_camera = depth_frame.at<uint16_t>((int)(depth_frame.size().height/2 - y_px), (int)(depth_frame.size().width/2 - x_px));

	double x_px_upper_limit = depth_frame.size().width/2 - ((x + SAMPLE_RADIUS) / z * FOCAL_LENGTH);
	double x_px_lower_limit = depth_frame.size().width/2 - ((x - SAMPLE_RADIUS) / z * FOCAL_LENGTH);

	double y_px_upper_limit = depth_frame.size().height/2 - ((y + 2*SAMPLE_RADIUS) / z * FOCAL_LENGTH);
	double y_px_lower_limit = depth_frame.size().height/2 - (y / z * FOCAL_LENGTH);

	//default value
	double theta_x = 0;
	double theta_y = 0;
	
	// check if sample points are within depth frame
	if(x_px_upper_limit > 0 && x_px_upper_limit < depth_frame.size().width && 
		x_px_lower_limit > 0 && x_px_lower_limit < depth_frame.size().width &&
		y_px_upper_limit > 0 && y_px_upper_limit < depth_frame.size().height &&
		y_px_lower_limit > 0 && y_px_lower_limit < depth_frame.size().height)

	{
		// get depthvalues from depth camera
		double x_depth_diff = depth_frame.at<uint16_t>((int)(depth_frame.size().height/2 - y_px), (int)x_px_upper_limit) - depth_frame.at<uint16_t>((int)(depth_frame.size().height/2 - y_px), (int)x_px_lower_limit);
		double y_depth_diff = depth_frame.at<uint16_t>((int)y_px_upper_limit, (int)(depth_frame.size().width/2 - x_px)) - depth_frame.at<uint16_t>((int)y_px_lower_limit, (int)(depth_frame.size().width/2 - x_px));

		//mark sample points in depth frame
		
		depth_frame.at<uint16_t>((int)(depth_frame.size().height/2 - y_px), (int)x_px_upper_limit) = 60000;
		depth_frame.at<uint16_t>((int)(depth_frame.size().height/2 - y_px), (int)x_px_lower_limit) = 60000;

		depth_frame.at<uint16_t>((int)y_px_upper_limit, (int)(depth_frame.size().width/2 - x_px)) = 60000;
		depth_frame.at<uint16_t>((int)y_px_lower_limit, (int)(depth_frame.size().width/2 - x_px)) = 60000;
		

		// calcualte andles and convert them to degree
		theta_x = atan(y_depth_diff / (2*SAMPLE_RADIUS)) / 3.1416 * 180.;
	 	theta_y = atan(x_depth_diff / (2*SAMPLE_RADIUS)) / 3.1416 * 180.;
	}

	cv::Vec6f result(x,y,z,theta_x,theta_y,0);

	return result;
}

double calculateMatchProbability(cv::Mat &frame, cv::Mat &frame_template, cv::Vec3f &keyhole)
{
	// get roi from circle dimensions
	cv::Rect roi;
	roi.x = keyhole[0];
	roi.y = keyhole[1];
	roi.width = 2*keyhole[2];
	roi.height = 2*keyhole[2];


	//check if roi is within bounds
	if (roi.x < 0 || roi.y < 0 || roi.x + roi.width >= frame.size().width || roi.y + roi.height >= frame.size().height)
	{
		// return error that will trigger new roi search
		return -1;
	}

	cv::Mat frame_roi = frame(roi);

	// image output for documentation
	//cv::imshow("frame_roi",frame_roi);

	// convert roi to grayscale
	cv::cvtColor(frame_roi, frame_roi, cv::COLOR_BGR2GRAY);

	//resize roi to template keyhole
	cv::resize(frame_roi, frame_roi, frame_template.size());

	// match keyhole template
	cv::Mat points;
	cv::matchTemplate(frame_roi, frame_template, points, cv::TM_CCOEFF_NORMED);

	// sort points and find point with highest probability
	double minVal;
	double maxVal;
	cv::Point minLoc;
	cv::Point maxLoc;
	cv::minMaxLoc(points, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

	return 100*maxVal;
}