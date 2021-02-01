#!/usr/bin/env python


import math
import numpy as np

import cv2
import cv2.cv as cv

import time



# -------------------------------------------------------------------------
# Load Image
# -------------------------------------------------------------------------

# load image from file
img = cv2.imread('in/TestBild.png', cv2.IMREAD_COLOR)




# -------------------------------------------------------------------------
# Prepare Gaussian White Noise Image
# -------------------------------------------------------------------------

row, col, ch = img.shape

# mean and standard deviation
mean = 0
sigma = 10

# apply gaussian noise
gauss = np.random.normal(mean, sigma, (row, col, ch))
gauss = gauss.reshape(row, col, ch)
img_noisy = img + gauss

# convert to 32bit for bilateral Filter (only for uint8 and float32)
img_noisy = np.float32(img_noisy)

# write image to file
cv2.imwrite('out/noisy_image.png', img_noisy)




# -------------------------------------------------------------------------
# Blur
# -------------------------------------------------------------------------

# get time at computation start
start = time.time()

# averaging
img_blur = cv2.blur(img_noisy, (21,21))    	

# get time at computation end
end = time.time()

# calculate time difference
diff = end - start
print ("Blur: %8.4f s" % diff)

# write image to file
cv2.imwrite('out/blur_test_image.png', img_blur)




# -------------------------------------------------------------------------
# Gauss
# -------------------------------------------------------------------------

# get time at computation start
start = time.time()

# gaussian blur
img_gauss = cv2.GaussianBlur(img_noisy, (21,21), 0)    	

# get time at computation end
end = time.time()

# calculate time difference
diff = end - start
print ("GaussianBlur: %8.4f s" % diff)

# write image to file
cv2.imwrite('out/gaussian_test_image.png', img_gauss)





# -------------------------------------------------------------------------
# Bilateral
# -------------------------------------------------------------------------

img_noisy = img_noisy.astype(np.float32)

# get time at computation start
start = time.time()

# smooth frame input
img_bilateral = cv2.bilateralFilter(img_noisy, 9, 21, 21)    	

# get time at computation end
end = time.time()

# calculate time difference
diff = end - start
print ("bilateralFilter: %8.4f s" % diff)

# write image to file
cv2.imwrite('out/bilateral_test_image.png', img_bilateral)















