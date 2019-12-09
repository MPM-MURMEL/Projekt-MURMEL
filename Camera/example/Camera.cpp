#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>

//#include <OpenNI.h>
#include <jsoncpp/json/json.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/core/core.hpp"

#include "../src/KeyholeDetection.hpp"
#include "../src/TCPServer.hpp"


using namespace cv;
using namespace std;
//using namespace openni;


enum OperationMode
{
	searching,
	tracking
};


OperationMode op_mode;


int main()
{
	/*
	int val1 = 90;
	int val2 = 90;
	namedWindow("Test");
	createTrackbar("hsv max.", "Test", &val1, 255);
	createTrackbar("hsv min.", "Test", &val2, 255);
	*/
	
	cout << "[INFO] Starting TCP Server..." << endl;

	while(true)
	{
		// start tcp server
		TCPServer server(8080); 


		// check if a client is connected
		if(server.isConnected())
		{
			cout << "[INFO] Connected" << endl;

			cout << "[INFO] Waiting for mode command..." << endl;
			string msg = server.receive(255);

			if (msg.compare("tracking") <= 0)
			{
				op_mode = tracking;
				cout << "[INFO] Tracking mode activated." << endl;
			}
			else
			{
				op_mode = searching;
				cout << "[INFO] Searching mode activated." << endl;
			}


			// use default camera as input
			VideoCapture cap(1);

			// import keyhole template
			/*
			Mat template_gray;
			template_gray = imread("keyhole.png", IMREAD_UNCHANGED);
			cvtColor(template_gray, template_gray, COLOR_BGR2GRAY);
			resize(template_gray, template_gray, Size(), 0.2, 0.2);
			*/

			// previously calculated roi
			Rect last_roi;
			bool roi_initialized = false;

			// grep frame and send info as long as tcp and camera are connected
			while(server.isConnected() && cap.isOpened())
			{
				if (waitKey(1) == 27) return 0;
			
				// read frame from camera
				Mat frame;
				cap.read(frame);


				
				// image output for documentation
				//waitKey(1);
				//imshow("Frame", frame);
				//imwrite("calibration_x.png", frame);
				
				// generate roi for color code
				Rect roi;

				// check if a cirecle was previously detected
				if (op_mode == searching || !roi_initialized)
				{
					// calculate roi from color space
					if (!getROI(frame, roi)) {continue;}
					roi_initialized = true;
				}
				else if(op_mode == tracking)
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
		
				// circle needs to be in roi and roi needs to be inside of frame
				// roi cant be out of frame
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

				//only use the current position for circle detection if tracking is enabled
				if (op_mode == tracking)
				{
					// set roi for next iteration
					last_roi = Rect(roi_x, roi_y, roi_width, roi_height);
				}

				// calculate the offset from center of image
				// define x and y as variables
				double x = (frame.size().width/2) - (roi.x + circles[0][0]);
				double y = (frame.size().height/2) - (roi.y + circles[0][1]);

				Vec3f coordinates = calculateCoordinates(x,y,r);

				// integrate variables into json string
				stringstream ss;
				ss << "{ \"dx\": " << coordinates[0] << ", \"dy\": " << coordinates[1] << ", \"dz\": " << coordinates[2] << "}";
				cout << ss.str() << endl;

				//send counter as string
				server.send(ss.str()); 
				//usleep(1000);

				//rectangle(frame, last_roi, Scalar(100,100,100));
				rectangle(frame, roi, Scalar(100,100,100));

				// only use first most dominant
				for (int i=0; i< circles.size(); i++)
				{
					Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
					int radius = cvRound(circles[i][2]);
					circle(frame(roi), center, radius, Scalar(0,0,255));
				}
			
				//circle(frame_gray_resized, maxLoc_global, 2, Scalar(0,0,255));

				// image output for documentation
				imshow("frame_processed", frame);

				
			}
			
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
		
}


