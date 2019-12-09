#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/core/core.hpp"

#include "../src/KeyholeDetection.hpp"

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


int main()
{
	//image capture
	VideoCapture cap(1);

	//namedWindow("Frame", WINDOW_NORMAL);
	//setWindowProperty("Frame", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

	// previously calculated roi
	Rect last_roi;
	bool roi_initialized = false;

	while(cap.isOpened())
	{
	
		// read frame from camera
		Mat frame;
		cap.read(frame);


		// calculate gray scale image of camera input
		Mat frame_gray;
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		

		// generate roi for color code
		Rect roi;


		// check if a cirecle was previously detected
		if (!roi_initialized)
		{
			// calculate roi from color space
			if (!getROI(frame, roi)) {continue;}
			roi_initialized = true;
		}
		else
		{
			// use previously calculated roi
			roi = last_roi;
		}


		// get circles in roi
		Mat frame_roi = frame(roi);
		vector<Vec3f> circles;

		
		if (!getCircles(frame_roi, circles)) {continue;}


		// set roi around circle center for net detection
		int r = circles[0][2];
		int roi_x = roi.x + circles[0][0] - 4*r;
		int roi_y = roi.y + circles[0][1] - 4*r;
		int roi_height = 8*r;
		int roi_width = 8*r;
		
		if (roi_x < 0) 
		{
			roi_width += roi_x;
			roi_x = 0;
		}
		if (roi_x + roi_width > frame.size().width-1) 
		{
			roi_width = frame.size().width - roi_x - 1;
		}

		if (roi_y < 0) 
		{
			roi_height += roi_y;
			roi_y = 0;
		}
		if (roi_y + roi_height > frame.size().height-1) 
		{
			roi_height = frame.size().height - roi_y - 1;
		}


		last_roi = Rect(roi_x, roi_y, roi_width,roi_height);


		// define x and y as variables
		double x = ((frame.size().width/2) - (roi.x + circles[0][0]));
		double y = ((frame.size().height/2) - (roi.y + circles[0][1]));

		cv::Vec3f coordinates = calculateCoordinates(x, y, r);

		cout << "x: " << coordinates[0] << endl;
		cout << "y: " << coordinates[1] << endl;
		cout << "z: " << coordinates[2] << endl;


		rectangle(frame, last_roi, Scalar(100,100,100));
		rectangle(frame, roi, Scalar(100,100,100));

		// only use first most dominant
		for (int i=0; i< circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(frame(roi), center, radius, Scalar(0,0,255));
		}
	
		//circle(frame_gray_resized, maxLoc_global, 2, Scalar(0,0,255));


		imshow("Frame", frame);


		if (waitKey(10) == 27) return 0;
	}
		
}