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
	int HSV_VALUE_MIN = 0;
	int HSV_VALUE_MAX = 16;
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
	
	//scale image down to 100 px width
	// is needed for closeups and the decrease in processing time
	double scale = 100.0/((double)frame.rows);
	
	cv::Mat resized;

	try
	{
		cv::resize(frame, resized, cv::Size(frame.cols*scale,frame.rows*scale));
	}
	catch(...)
	{
		return 0;
	}


	// change saturation and brighness of image for easyser circle detection
	cv::Mat resized_high_constrast = cv::Mat::zeros(resized.size(), resized.type());
	double alpha = 11;
	int beta = 100;

	// loop through all pixel of image
	for (int i=0; i < resized.rows; i++)
	{
		for (int j=0; j < resized.cols; j++)
		{
			for (int c=0; c < 3; c++)
			{
				// apply saturation and brighness of pixel
				resized_high_constrast.at<cv::Vec3b>(i,j)[c] = cv::saturate_cast<uchar>(alpha * (resized.at<cv::Vec3b>(i,j)[c]) + beta);
			}
		
		}
	}	

	// image output for documentation
	//cv::imshow("frame_high_constrast",frame_high_constrast);

	cv::Mat resized_high_constrast_gray;
	cv::cvtColor(resized_high_constrast, resized_high_constrast_gray, cv::COLOR_BGR2GRAY);

	// image output for documentation
	//cv::imshow("frame_high_constrast_gray",frame_high_constrast_gray);

	// grab keyhole circles via hough transformation 
	cv::HoughCircles(resized_high_constrast_gray, circles, cv::HOUGH_GRADIENT,1,1000,100,20,10,80);

	// check if only one cirle was detected
	if (circles.size()<=0) {return 0;}

	circles[0][2] /= scale;
	circles[0][0] /= scale;
	circles[0][1] /= scale;
	
	return 1;
}

cv::Vec3f calculateCoordinates(double x_px, double y_px, double r_px)
{	
	// focal length of rgb camera
	//const double FOCAL_LENGTH = 235.2; //Orbbec Perseee
	const double FOCAL_LENGTH = 435; //Trust Webcam

	// radius of keyhole in mm
	const double RADIUS = 25;

	// coordinate calculations
	double z = FOCAL_LENGTH / r_px * RADIUS;
	double x = x_px / FOCAL_LENGTH * z;
	double y = y_px / FOCAL_LENGTH * z;

	cv::Vec3f result(x,y,z);

	return result;
}

int getKeyhole(cv::Mat &frame, cv::Mat &frame_template, cv::Vec3f &keyhole)
{
	/*
	double maxVal_global;
	Point maxLoc_global;
	int scale_global;

	for (int i = 60; i>=130; i++)
	{
		double scale = (double)i/100;

		// calculate a resized roi
		Mat frame_gray_resized = frame_gray.clone();
		resize(frame_gray_resized, frame_gray_resized, Size(), scale, scale);

		// match keyhole template
		Mat points;
		matchTemplate(frame_gray_resized, template_gray, points, TM_CCOEFF_NORMED);

		// sort points and find point with highest probability
		double minVal;
		double maxVal;
		Point minLoc;
		Point maxLoc;
		minMaxLoc(points, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

		// save max values through scale iterations
		if (maxVal_global < maxVal)
		{
			// update global variables
			maxVal_global = maxVal;
			maxLoc_global = maxLoc;
			scale_global = scale;
		}
	}

	keyhole[0] = scale_global * maxLoc_global[0];
	keyhole[1] = scale_global * maxLoc_global[0];
	*/
	


	/*
	# check for full rank
	if roi_frame_keyhole_gray.shape[0] != 0 and roi_frame_keyhole_gray.shape[1] != 0 :

		# import keyhole template
		template_gray = cv2.imread('keyhole.png',cv2.IMREAD_UNCHANGED)
		template_gray =	cv2.cvtColor(template_gray,cv2.COLOR_BGR2GRAY)

		# extract edges from keyhole sub roi
		roi_frame_keyhole_canny = cv2.Canny(roi_frame_keyhole_gray, 50, 200)
		roi_frame_keyhole_canny = cv2.blur(roi_frame_keyhole_canny,(3,3))  
		#cv2.imshow('CANNY Keyhole',roi_frame_keyhole_canny)


		# variables to save scale results
		maxLoc_global = None
		maxVal_global = None
		scale_global = None

		#test with multiple template scales
		for scale in np.linspace(0.6, 1.3, 20):

			# resize template to match circle size
			template_gray_resized = cv2.resize(template_gray, (int(circle[2]*2*scale), int(circle[2]*2*scale)))

			#extract edges from template
			template_canny = cv2.Canny(template_gray_resized, 50, 200)
			template_canny = cv2.blur(template_canny,(3,3))  
			#cv2.imshow('CANNY template',template_canny)

			# check if template is smaller then roi
			if roi_frame_keyhole_canny.shape[0] >= template_canny.shape[0] and roi_frame_keyhole_canny.shape[1] >= template_canny.shape[1] : 

				# find points where the template matches
				points = cv2.matchTemplate(roi_frame_keyhole_canny, template_canny, cv2.TM_CCOEFF_NORMED)

				# sort points and find point with highest probability
				_, maxVal, _, maxLoc = cv2.minMaxLoc(points)
				
				# save max values through scale iterations
				if maxVal_global is None or maxVal_global < maxVal:

					# update global variables
					maxVal_global = maxVal
					maxLoc_global = maxLoc
					scale_global = scale


		print maxVal_global

		# check if maxVal is enought for keyhole recognition
		if maxVal_global > KEYHOLE_THRESHOLD:

			keyhole = (maxLoc_global[0] + roi[0] + int(circle[0] - circle[2]/scale_global), maxLoc_global[1] + roi[1] + int(circle[1] - circle[2]/scale_global), int(circle[2]*scale_global))


			cv2.circle(frame,(keyhole[0],keyhole[1]), keyhole[2], (255,0,255),3)
			cv2.circle(frame,(keyhole[0],keyhole[1]), 1, (255,0,255),3)

		*/
	
	return 1;
}