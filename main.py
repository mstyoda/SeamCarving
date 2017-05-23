import cv2
import numpy as np
import math
import copy
from getLine import *

def getc(i,j,k,b1,b2):
	(x,y) = (i + 0.5,j + 0.5)	
	ans = 0
	for di in range(-1,2):
		for dj in range(-1,2):
			(xx,yy) = ((x + di * (1./3.)),(y + dj * (1./3.)))
			if (yy - (k * xx + b1)) * (yy - (k * xx + b2)) <= 0:
				d = (2.**(2 - abs(di) - abs(dj))) / 16.
				ans = ans + d
	return (ans * 255.)

def kfAA(Line,M,T):
	((x0,y0),(x1,y1)) = Line
	k = float(y1 - y0) / float(x1 - x0)
	b = (y0 + 0.5) - k * (x0 + 0.5)
	theta = math.atan(k)
	cos = math.cos(theta)
	(b1,b2) = (b - (0.5)/cos,b + (0.5)/cos)
	
	P = getLine((x0,y0),(x1,y1),800,800)
	for (i,j) in P:
		if j >= 1:
			if T == False:
				M[i][j - 1] = max(M[i][j - 1],getc(i,j - 1,k,b1,b2))
			else:
				M[j - 1][i] = max(M[j - 1][i],getc(i,j - 1,k,b1,b2))
		if T == False:
			M[i][j] = max(M[i][j],getc(i,j,k,b1,b2))
		else:
			M[j][i] = max(M[j][i],getc(i,j,k,b1,b2))
		if j <= 800:
			if T == False:
				M[i][j + 1] = max(M[i][j + 1],getc(i,j + 1,k,b1,b2))
			else:
				M[j + 1][i] = max(M[j + 1][i],getc(i,j + 1,k,b1,b2))
	return M
def Bresenham(P0,P1,mX,mY,T,M):
	P = getLine(P0,P1,mX,mY)
	for (x,y) in P:
		if T:
			M[y][x] = 255
		else:
			M[x][y] = 255
	return M
Final1 = np.array([[0]*800]*800).astype('uint8')
(Final2,Final3) = (copy.copy(Final1),copy.copy(Final1))
L = []
cnt = 50
while True:
	#(x0,y0,x1,y1) = input("input x0,y0,x1,y1 (in range[1,799])\n",)
	while True:
		[x0,y0,x1,y1] = np.random.randint(2,799,size=4)
		if (x0,y0) != (x1,y1):
			break
	#if (x0 == 0):
	cnt = cnt - 1
	if (cnt == 0):
		break
	L.append((x0,y0,x1,y1))
for id in range(0,len(L)):
	(x0,y0,x1,y1) = L[id]
	Tflag = abs(x1 - x0) < abs(y1 - y0)
	if Tflag:
		Line = ((y0,x0),(y1,x1))
	else:
		Line = ((x0,y0),(x1,y1))
	Final1 = Bresenham(Line[0],Line[1],800,800,Tflag,Final1)
	img2 = kfAA(Line,Final2,Tflag)
cv2.imwrite("Bresenham.png",Final1)
cv2.imwrite("kfAA.png",Final2)
Final3 = cv2.resize(Final2,(1600,1600))
Final3 = cv2.resize(Final3,(800,800))
cv2.imwrite("SSAA.png",Final3)