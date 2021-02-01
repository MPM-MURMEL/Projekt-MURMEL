#!/usr/bin/python

import cv2 as cv
import numpy as np
from primesense import openni2 as ni
from primesense import _openni2 as c_ni


ni.initialize("/home/ubuntu/OpenNI-Linux-Arm-2.3/Redist")
dev = ni.Device.open_any()
depth_stream = dev.create_depth_stream()
depth_stream.start()
depth_stream.set_video_mode(c_ni.OniVideoMode(pixelFormat = c_ni.OniPixelFormat.ONI_PIXEL_FORMAT_DEPTH_100_UM, resolutionX = 640, resolutionY = 480, fps = 30))

while True:
	frame = depth_stream.read_frame()
	frame_data = frame.get_buffer_as_uint16()
	img = np.frombuffer(frame_data, dtype=np.uint16)
	img.shape = (1, 480, 640)
	img = np.concatenate((img, img, img), axis=0)
	img = np.swapaxes(img, 0, 2)
	img = np.swapaxes(img, 0, 1)
	cv.imshow("image", img)


	k = cv.waitKey(5) & 0xFF
    	if k == 27:
        	break

ni.unload()
