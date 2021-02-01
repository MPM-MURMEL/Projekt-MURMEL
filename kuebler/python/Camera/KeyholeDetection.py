#!/usr/bin/env python


import math
import numpy as np

import cv2
import cv2.cv as cv

from primesense import openni2 as ni
from primesense import _openni2 as c_ni

import socket
import json




# -------------------------------------------------------------------------
# FUNCTIONS
# -------------------------------------------------------------------------

# automatic white balance of an image
def white_balance(img):
	result = cv2.cvtColor(img, cv2.COLOR_BGR2LAB)
	avg_a = np.average(result[:,:,1])
	avg_b = np.average(result[:,:,2])
	result[:,:,1] = result[:,:,1] - ((avg_a -128) * (result[:,:,0]/255.0) * 1.1)
	result[:,:,2] = result[:,:,2] - ((avg_b -128) * (result[:,:,0]/255.0) * 1.1)
	result = cv2.cvtColor(result, cv2.COLOR_LAB2BGR)
	return result


# returns the average value in a matrix not considering zero
def non_zero_mean(img):

	points = img[np.nonzero(img)]
	
	if len(points):
		return np.mean(points)

	return 0





# -------------------------------------------------------------------------
# PARAMETER
# -------------------------------------------------------------------------

# udp network config
UDP_IP = "192.168.100.100"
UDP_PORT = 5005

# udp data structure
DATA_TEMPLATE = {"x":0, "y":0, "z":0, "thetaX":0, "thetaY":0, "thetaZ":0}




# define resolution
RESOLUTION_X = 640
RESOLUTION_Y = 480

# define constant field of view in degree
HORIZONTAL_FIELD_OF_VIEW = 60.0
VERTICAL_FIELD_OF_VIEW = 49.5

# calculate angular size of pixel
PHI_X = HORIZONTAL_FIELD_OF_VIEW / RESOLUTION_X
PHI_Y = VERTICAL_FIELD_OF_VIEW / RESOLUTION_Y




# define trash can orange
ORANGE = np.uint8([[[40,120,255]]])
HSV_ORANGE = cv2.cvtColor(ORANGE, cv2.COLOR_BGR2HSV)
HSV_VALUE = HSV_ORANGE[0][0][0]

# calculate lower and upper bound for color detection
LOWER_ORANGE = np.array([HSV_VALUE - 10,100,100])
UPPER_ORANGE = np.array([HSV_VALUE + 7,255,255])




# define padding for trash can roi
ROI_PEDDING = 20

# offset between rgb and depth image
ROI_OFFSET_X = 25
ROI_OFFSET_Y = 10

# size of the keyhole distance sample frame
SAMPLE_FRAME_SIZE = 40

# threshhold of keyhole probability
KEYHOLE_THRESHOLD = 0.55




# -------------------------------------------------------------------------
# INIT UDP SOCKET
# -------------------------------------------------------------------------

# socket object
SOCKET = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 




# -------------------------------------------------------------------------
# INIT CAMERA STREAMS
# -------------------------------------------------------------------------

# initialize depth stream
ni.initialize("/home/ubuntu/OpenNI-Linux-Arm-2.3/Redist")
dev = ni.Device.open_any()
depth_stream = dev.create_depth_stream()
depth_stream.start()
depth_stream.set_video_mode(c_ni.OniVideoMode(pixelFormat = c_ni.OniPixelFormat.ONI_PIXEL_FORMAT_DEPTH_100_UM, resolutionX = RESOLUTION_X, resolutionY = RESOLUTION_Y, fps = 30))


# initialize rgb stream
cap = cv2.VideoCapture(0)




while(1):

	# -------------------------------------------------------------------------
	# GRAB DEPTH IMAGE
	# -------------------------------------------------------------------------

	# take frame from depth camera
	depth_frame = depth_stream.read_frame()
	frame_data = depth_frame.get_buffer_as_uint16()

	# reshape depth data into rgb frame compatible format 
	depth_frame = np.frombuffer(frame_data, dtype=np.uint16)
	depth_frame.shape = (1, RESOLUTION_Y, RESOLUTION_X)
	depth_frame = np.concatenate((depth_frame, depth_frame, depth_frame), axis=0)
	depth_frame = np.swapaxes(depth_frame, 0, 2)
	depth_frame = np.swapaxes(depth_frame, 0, 1)

	# important to mirrow depth image vertivally
	# normal image and depth image are recorded differently and do not match without the mirrowing effect
	depth_frame = cv2.flip(depth_frame, 1)
	depth_frame = depth_frame[:,:,1]




	# -------------------------------------------------------------------------
	# GRAB RGB IMAGE
	# -------------------------------------------------------------------------

    	# take frame from rgb camera
    	_,frame = cap.read()
	



	# -------------------------------------------------------------------------
	# RGB IMAGE FILTER
	# -------------------------------------------------------------------------

	# white balance
	# enhances robustness but slow
	#frame = white_balance(frame)

	# smooth frame input
	smoothed_frame = cv2.blur(frame,(20,20))    	




	# -------------------------------------------------------------------------
	# REGION OF INTEREST
	# -------------------------------------------------------------------------

	# convert BGR to HSV
    	hsv = cv2.cvtColor(smoothed_frame,cv2.COLOR_BGR2HSV)

    	# threshold the HSV image to only get pre define color range
    	mask = cv2.inRange(hsv,LOWER_ORANGE,UPPER_ORANGE)

	# apply mask to frame
	frame = cv2.bitwise_and(frame, frame, mask=mask)

	# find roi via mask
	points = cv2.findNonZero(mask)


	roi = None

	# check if enough points for roi exists
	if points is not None :
		
		# bounding box of points
		roi = cv2.boundingRect(points)	

	


	# -------------------------------------------------------------------------
	# KEYHOLE DETECTION
	# -------------------------------------------------------------------------

	keyhole = None

	#check if roi exists
	if roi is not None:
		
		# grab roi from frame
		roi_frame = frame[roi[1]:roi[1]+roi[3],roi[0]:roi[0]+roi[2]]

		# transform keyhole frame to grayscale for hough transformation
		roi_frame_gray = cv2.cvtColor(roi_frame,cv2.COLOR_BGR2GRAY)

		# grab keyhole circles via hough transformation 
		circles = cv2.HoughCircles(roi_frame_gray,cv.CV_HOUGH_GRADIENT,1,1000,param1=50,param2=10,minRadius=10,maxRadius=30)

		# check if only one cirle was detected
		if circles is not None and len(circles) == 1:

			# only use first most dominant
			circle = np.uint16(np.around(circles))[0][0]

			# keyhole roi from cirecle detection
			roi_frame_keyhole_gray = roi_frame_gray[circle[1] - circle[2] * 2 : circle[1] + circle[2] * 2, circle[0] - circle[2] * 2: circle[0] + circle[2] * 2]

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




	# -------------------------------------------------------------------------
	# POSITION
	# -------------------------------------------------------------------------

	keyhole_coordinate_x = None
	keyhole_coordinate_y = None
	keyhole_coordinate_z = None


	if False:#keyhole is not None:

		# position of keyhole in depthframe
		Y = keyhole[1] + roi[1] - ROI_OFFSET_Y
		X = keyhole[0] + roi[0] + ROI_OFFSET_X


		# sample roi around keyhole in depth frame
		roi_frame_keyhole = depth_frame[Y - SAMPLE_FRAME_SIZE : Y + SAMPLE_FRAME_SIZE, X - SAMPLE_FRAME_SIZE : X + SAMPLE_FRAME_SIZE]

		# calculate mean distance in sample roi
		mean  = non_zero_mean(roi_frame_keyhole)
		
		# if depthmap is available use depth value
		if mean > 0:

			# calculate z coordinate of keyhole in mm
			keyhole_coordinate_z = mean

			# calculate angle theta for x coordinate
			theta_x = PHI_X * (keyhole[0] + roi[0] - RESOLUTION_X/2)

			# calculate angle theta for y coordinate
			theta_y = PHI_Y * (RESOLUTION_Y/2 - keyhole[1] - roi[1])

			# calculate x coordinate of keyhole in mm
			keyhole_coordinate_x = round(math.tan(math.radians(theta_x)) * keyhole_coordinate_z, 2)

			# calculate y coordinate of keyhole in mm
			keyhole_coordinate_y = round(math.tan(math.radians(theta_y)) * keyhole_coordinate_z, 2)

		# if depthmap doesnt exist use pixel values for x and y
		else:

			# x coordinate of keyhole in px
			# x increases from left to right of image
			keyhole_coordinate_x =  keyhole[0] + roi[0] - RESOLUTION_X/2

			# y coordinate of keyhole in px
			# y increases from top to bottom of image
			keyhole_coordinate_y = RESOLUTION_Y/2 - keyhole[1] - roi[1]  




	# -------------------------------------------------------------------------
	# ORIENTATION in deg
	# -------------------------------------------------------------------------

	orentation_angle_x = None
	orentation_angle_y = None

	if False:

		# grab roi from frame
		roi_depth = depth_frame[roi[1]:roi[1]+roi[3],roi[0]:roi[0]+roi[2]]
	
		# draw keyhole
		cv2.circle(roi_depth,(keyhole[0]+ROI_OFFSET_X,keyhole[1]-ROI_OFFSET_Y),keyhole[2],(0,255,255),3)
		cv2.circle(roi_depth,(keyhole[0]+ROI_OFFSET_X,keyhole[1]-ROI_OFFSET_Y),1,(0,255,255),3)


		cv2.imshow("roi_depth", roi_depth)


		# TODO
		sample_depth_left = depth_frame[keyhole[1]+roi[1], keyhole[0]+roi[0]-20][0]
		sample_depth_right = depth_frame[keyhole[1]+roi[1], keyhole[0]+roi[0]+20][0]


		# calculate x coordinate of sample pint left in mm
		sample_left_coordinate_x = math.tan(math.radians(theta_x)) * sample_depth_left

		# calculate x coordinate of sample point right in mm
		sample_right_coordinate_x = math.tan(math.radians(theta_x)) * sample_depth_right


		# calculate depth variation of sample points
		delta_sample_depth = int(sample_depth_left)-int(sample_depth_right)

		# calculate the rotation angle around the y axis
		orentation_angle_y = math.degrees(math.atan(delta_sample_depth/(math.fabs(sample_left_coordinate_x)+math.fabs(sample_right_coordinate_x))))





		# TODO
		sample_depth_top = depth_frame[keyhole[1]+roi[1]-20, keyhole[0]+roi[0]][0]
		sample_depth_bottom = depth_frame[keyhole[1]+roi[1]+20, keyhole[0]+roi[0]][0]


		# calculate y coordinate of sample pint top in mm
		sample_top_coordinate_y = math.tan(math.radians(theta_y)) * sample_depth_top

		# calculate y coordinate of sample point bottom in mm
		sample_bottom_coordinate_y = math.tan(math.radians(theta_y)) * sample_depth_bottom


		# calculate depth variation of sample points
		delta_sample_depth = int(sample_depth_top)-int(sample_depth_bottom)

		# calculate the rotation angle around the x axis
		orentation_angle_x = math.degrees(math.atan(delta_sample_depth/(math.fabs(sample_top_coordinate_y)+math.fabs(sample_bottom_coordinate_y))))




	# -------------------------------------------------------------------------
	# UDP MESSAGE
	# -------------------------------------------------------------------------

	#DATA_TEMPLATE = {"x":0, "y":0, "z":0, "thetaX":0, "thetaY":0, "thetaZ":0}
    data = DATA_TEMPLATE;
	data['x'] = keyhole_coordinate_x
	data['y'] = keyhole_coordinate_y
	data['z'] = keyhole_coordinate_z
	data['thetaX'] = orentation_angle_x
	data['thetaY'] = orentation_angle_y
	data['thetaZ'] = 0

    #SOCKET.sendto( json.dumps(data), (UDP_IP, UDP_PORT))


    cv2.imshow('DISPLAY',frame)
    cv2.imshow('MASK',mask)
	#cv2.imshow('ROI',roi_frame)	
	cv2.imshow("DEPTH", depth_frame)


    	k = cv2.waitKey(5) & 0xFF
    	if k == 27:
        	break

cv2.destroyAllWindows()
ni.unload()














