#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>

#include <OpenNI.h>
#include <jsoncpp/json/json.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/core/core.hpp"

#include "../src/KeyholeDetection.hpp"
#include "../src/TCPServer.hpp"


using namespace cv;
using namespace std;
using namespace openni;



// searching always uses hole image
// tracking uses the roi of last detected keyhole to save on processing time 
enum OperationMode
{
	SEARCHING,
	TRACKING
};

// operation mode
OperationMode op_mode;


int main()
{	
	cout << "[INFO] Starting TCP Server..." << endl;

	while(true)
	{
		// start tcp server
		TCPServer server(8080); 
		
		// check if a client is connected
		if(server.isConnected())
		{
			cout << "[INFO] Connected" << endl;

			cout << "[INFO] Waiting for MODE command..." << endl;
			string msg = server.receive(255);
			size_t found = msg.find("TRACKING");

			if (found!=string::npos)
			{
				op_mode = TRACKING;
				cout << "[INFO] Tracking mode activated." << endl;
			}
			else
			{
				op_mode = SEARCHING;
				cout << "[INFO] Searching mode activated." << endl;
			}

			//initializing depth camera
			cout << "[INFO] Initialize OpenNI..." << endl;
			Status rc = OpenNI::initialize();

			if (rc != STATUS_OK)
			{
				cout << "[INFO] Initialize failed " << OpenNI::getExtendedError() << endl;
				return 1;
			}

			Device depth_cap;
			rc = depth_cap.open(ANY_DEVICE);
		   
		   	if (rc != STATUS_OK)
			{
				cout << "Couldn't open device " << OpenNI::getExtendedError() << endl;
				return 2;
			}

			VideoStream depth_stream;

			if (depth_cap.getSensorInfo(SENSOR_DEPTH) != NULL)
			{
				rc = depth_stream.create(depth_cap, SENSOR_DEPTH);
				if (rc != STATUS_OK)
				{
					cout << "Couldn't create depth stream " << OpenNI::getExtendedError() << endl;
					return 3;
				}

				// set depth in mm
				VideoMode vmod;
				vmod.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
				depth_stream.setVideoMode(vmod);
			}

			rc = depth_stream.start();

			if (rc != STATUS_OK)
			{
				cout << "Couldn't start the depth stream" << OpenNI::getExtendedError() << endl;
				return 4;
			}

			
			// initilaizing rgb camera
			cout << "[INFO] Initialize OpenCV..." << endl;
			VideoCapture rgb_cap(0);

			// import keyhole template
			Mat template_gray;
			template_gray = imread("keyhole.png", IMREAD_UNCHANGED);
			cvtColor(template_gray, template_gray, COLOR_BGR2GRAY);
			resize(template_gray, template_gray, Size(), 0.2, 0.2);


			// previously calculated roi
			Rect last_roi;
			bool roi_initialized = false;

			// grep frame and send info as long as tcp and camera are connected
			while(server.isConnected() && rgb_cap.isOpened() && rc == STATUS_OK)
			{
				if (waitKey(1) == 27) return 0;
			
				// read frame from rgb camera
				Mat frame;
				rgb_cap.read(frame);

				// read frame from depth camera
				VideoFrameRef depth_frame_raw;
				depth_stream.readFrame(&depth_frame_raw);

				// raw data to mat transformation for usage with opencv
				Mat depth_frame;
				const DepthPixel* imgBuff = (DepthPixel*)depth_frame_raw.getData();
				depth_frame.create(depth_frame_raw.getHeight(), depth_frame_raw.getWidth(), CV_16UC1);
				memcpy(depth_frame.data, imgBuff, depth_frame_raw.getHeight()*depth_frame_raw.getWidth()*sizeof(uint16_t));
				flip(depth_frame, depth_frame, 1);
				depth_frame *= 30;

				// generate roi for color code
				Rect roi;

				// check if a cirecle was previously detected
				if (op_mode == SEARCHING || !roi_initialized)
				{
					// calculate roi from color space
					if (!getROI(frame, roi)) {continue;}
					roi_initialized = true;
				}
				else if(op_mode == TRACKING)
				{
					// use previously calculated roi
					roi = last_roi;
				}

				// get circles in roi
				Mat frame_roi = frame(roi);

				// coordinate and orientation  
				Vec6f coordinates;
				double probability = 0;

				// only calculate coordinates if a circle was found
				vector<Vec3f> circles;
				if (getCircles(frame_roi, circles)) 
				{
					// set roi around circle center for net detection
					int r = circles[0][2];
					int roi_x = roi.x + circles[0][0] - 4*r;
					int roi_y = roi.y + circles[0][1] - 4*r;
					int roi_height = 8*r;
					int roi_width = 8*r;

					Vec3f in_frame_pos(roi.x+circles[0][0]-r, roi.y+circles[0][1]-r,r);
		
					// circle needs to be in roi and roi needs to be inside of frame
					// roi cant be out of frame
					if (roi_x < 0) 
					{
						roi_x = 0;
					}
					if (roi_x + roi_width > frame.size().width-1) 
					{
						roi_width = frame.size().width - roi_x - 1;
					}

					if (roi_y < 0) 
					{
						roi_y = 0;
					}
					if (roi_y + roi_height > frame.size().height-1) 
					{
						roi_height = frame.size().height - roi_y - 1;
					}

					// calculate the offset from center of image
					// define x and y as variables
					double x = (frame.size().width/2) - (roi.x + circles[0][0]);
					double y = (frame.size().height/2) - (roi.y + circles[0][1]);
			
					coordinates = calculateCoordinates(depth_frame, x, y, r);

					// calculate probability that the circle is actually the keyhole
					probability = calculateMatchProbability(frame, template_gray, in_frame_pos);

					//only use the current position for circle detection if TRACKING is enabled
					if (op_mode == TRACKING)
					{
						// set roi for next iteration
						last_roi = Rect(roi_x, roi_y, roi_width, roi_height);
					}
				}
				else
				{
					roi_initialized = false;
				}

				// reset roi if a wrong math was detected
				if(probability < 0)
				{
					roi_initialized = false;
				}
				
				// integrate variables into json string
				stringstream ss;
				ss << "{ \"probability\": " << probability << ", \"dx\": " << coordinates[0] << ", \"dy\": " << coordinates[1] << ", \"dz\": " << coordinates[2] << ", \"theta_x\": " << coordinates[3] << ", \"theta_y\": " << coordinates[4] << "}";
				cout << ss.str() << endl;


				//send counter as string
				server.send(ss.str()); 

				//rectangle(frame, last_roi, Scalar(100,100,100));
				rectangle(frame, roi, Scalar(100,100,100));

				// only use first most dominant
				for (int i=0; i< circles.size(); i++)
				{
					Point center(cvRound(circles[i][0]),cvRound(circles[i][1]));
					int radius = cvRound(circles[i][2]);
					circle(frame(roi), center, radius, Scalar(0,0,255));
				}
			
				// image output for documentation
				imshow("frame_processed", frame);
				//imwrite("test_rgb.jpg", frame);

				imshow("depth_frame", depth_frame);
			}

			depth_stream.stop();
			depth_stream.destroy();
			depth_cap.close();
			//OpenNI::shutdown();	
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
	
	OpenNI::shutdown();	
		
}


