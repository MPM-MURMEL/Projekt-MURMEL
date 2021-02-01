# -*- coding: utf-8 -*-
"""
Created on Thu Oct  4 18:31:11 2018
partly found on: 
    https://www.pyimagesearch.com/2016/08/08/k-nn-classifier-for-image-classification/
@author: Jiamod
"""
from imutils import paths
import cv2
import numpy
#import math
from pathlib import Path
#import glob
import cv2
import numpy as np
import imutils
#import pylab
from sklearn.externals import joblib
from scipy import misc
from scipy.spatial.distance import mahalanobis
from sklearn import preprocessing
from sklearn.neighbors import KNeighborsClassifier
from sklearn.cross_validation import train_test_split
from matplotlib import pyplot as plt
#import time
import os
#import sys

def extract_color_histogram(image, bins=(8, 8, 8)):
	# extract a 3D color histogram from the HSV color space using
	# the supplied number of `bins` per channel
	hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
	hist = cv2.calcHist([hsv], [0, 1, 2], None, bins,
		[0, 180, 0, 256, 0, 256])
 
	# handle normalizing the histogram if we are using OpenCV 2.4.X
	if imutils.is_cv2():
		hist = cv2.normalize(hist)
 
	# otherwise, perform "in place" normalization in OpenCV 3 (I
	# personally hate the way this is done
	else:
		cv2.normalize(hist, hist)
 
	# return the flattened histogram as the feature vector
	return hist.flatten()
"""
Read in the images from a folder. Each folder within represents one class 
of the input images. 
"""
def load_data(data_directory, imagewidth, imageheight):
    directories = [d for d in os.listdir(data_directory) 
                   if os.path.isdir(os.path.join(data_directory, d))]
    labels = []
    images = []
    histogram = []
    labelString = [] # maps integer to labelstring
    labelInteger = 0
    for d in directories:
        label_directory = os.path.join(data_directory, d)
        file_names = [os.path.join(label_directory, f) 
                      for f in os.listdir(label_directory) 
                      if f.endswith(".jpg")]
        for f in file_names:
            labelString.append(str(d)+' '+str(labelInteger))
            img = cv2.imread(f, 1)
            resized = cv2.resize(img, (imagewidth, imageheight)) 
            #img = io.imread(f, True)
            #ret, img =  cv2.threshold(img,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
            # reshape image matrix to long thin vector of all rows
            hist = extract_color_histogram(resized)
            reshaped = resized.ravel()
            #img = np.ravel(img) # just one long feature vector
            
            histogram.append(hist)
            images.append(reshaped)
            labels.append(int(labelInteger))
        labelInteger = labelInteger + 1
    return images, histogram, labels, labelString
"""
Calculates euclidean distance between two vectors a and b. 
"""
def distance(a, b, metric = 'euclidian'):
    if metric == 'euclidian':
        return np.sqrt(np.sum((a - b) ** 2))
    elif metric == 'mahalanobis': # TODO: Testing!
        return mahalanobis(a, b, np.linalg.inv(np.cov(a, b)))
    
    
    
# =============================================================================
#                         Begin computation
# =============================================================================
#CONSTANT VALUES
imageHeight = 50 #63
imageWidth = 30 #50
neighbor = 3

locationOData = 'kNN_images'

rawImages, features, labels, labelString = load_data(locationOData, imageWidth, imageHeight)

# show some information on the memory consumed by the raw images
# matrix and features matrix
rawImages = np.array(rawImages)

print("[INFO] pixels matrix: {:.2f}MB".format(
	rawImages.nbytes / (1024 * 1000.0)))

feat_train, feat_test, label_train, label_test = train_test_split(rawImages, 
                                                                  labels, 
                                                                  test_size=0.33, 
                                                                  random_state=42)
feat_train_hist, feat_test_hist, label_train_hist, label_test_hist = train_test_split(features, 
                                                                  labels, 
                                                                  test_size=0.33, 
                                                                  random_state=42)

# train and evaluate a k-NN classifer on the raw pixel intensities
print("[INFO] evaluating raw pixel accuracy...")
model = KNeighborsClassifier(n_neighbors=neighbor, n_jobs=-1)
model.fit(feat_train, label_train)
acc = model.score(feat_test, label_test)
print("[INFO] raw pixel accuracy: {:.2f}%".format(acc * 100))

# train and evaluate a k-NN classifer on the raw pixel intensities
print("[INFO] evaluating feature accuracy...")
model = KNeighborsClassifier(n_neighbors=neighbor, n_jobs=-1)
model.fit(feat_train_hist, label_train_hist)
acc = model.score(feat_test_hist, label_test_hist)
print("[INFO] feature accuracy: {:.2f}%".format(acc * 100))
