#!/usr/bin/env python

import sys
import csv

points = [[None,None,None]]

def addPoint(laser, x, y):
	points.append([laser, float(x), float(y)])
	print float(x), float(y)

def procPoints():
	pointCloud = open(sys.argv[2], 'w+')
	pointCloud.write("{\n")
	for point in points:
		pointCloud.write('{' +  str(point[0]) + ',')
		pointCloud.write(str(int(round(point[1]))) +  ',')
		pointCloud.write(str(int(round(point[2]))) + "},\n")
	pointCloud.seek(-1, 1)
	pointCloud.write('}')

with open(sys.argv[1], 'rb') as csvfile:
	prevX = 0
	prevY = 0
	
	csv.QUOTE_NONE
	coordList = csv.reader(csvfile, delimiter=',')
	count = 0
	for row in coordList:
		count = count + 1
		if count == 6: break
		#if (prevX != row[0] or prevY != row[1]):
		#	addPoint(0, row[0], row[1])
		print row
		addPoint(0, row[0], row[1])
		addPoint(1, row[2], row[3])

	procPoints()
