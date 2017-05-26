import cv2
import numpy as np
import copy


def getE_Sobel(img):
	x = cv2.convertScaleAbs(cv2.Sobel(img,cv2.CV_16S,1,0))
	y = cv2.convertScaleAbs(cv2.Sobel(img,cv2.CV_16S,0,1))
	x = cv2.cvtColor(x,cv2.COLOR_BGR2GRAY)
	y = cv2.cvtColor(y,cv2.COLOR_BGR2GRAY)
	return (x,y)

def get_Row((Ex,Ey)):
	n,m = Ex.shape
	f = np.zeros((n,m))
	for i in range(0,n):
		for j in range(0,m):
			cur = 1e+10 * i
			if i >= 1:
				cur = min(f[i - 1,j],cur)
				if j > 1:
					cur = min(f[i - 1][j - 1],cur)
				if j < m - 1:
					cur = min(f[i - 1][j + 1],cur)
			dx = Ex[i][j]
			dy = Ey[i][j]
			f[i][j] = cur + dx + dy
	
	bestj = 0
	for j in range(0,m):
		if f[n - 1][j] < f[n - 1][bestj]:
			bestj = j
	R = np.zeros((n)).astype(int)
	for i in range(0,n)[::-1]:
		R[i] = bestj
		j = bestj
		if i > 0:
			if j > 0:
				if f[i - 1][j - 1] < f[i - 1][bestj]:
					bestj = j - 1
			if j < m - 1:
				if f[i - 1][j + 1] < f[i - 1][bestj]:
					bestj = j + 1
	return R

def get_Column((Ex,Ey)):
	n,m = Ex.shape
	f = np.zeros((m,n))
	for j in range(0,m):
		for i in range(0,n):
			cur = 1e+10 * j
			if j >= 1:
				cur = min(f[j - 1][i],cur)
				if i > 1:
					cur = min(f[j - 1][i - 1],cur)
				if i < n - 1:
					cur = min(f[j - 1][i + 1],cur)
			dx = Ex[i][j]
			dy = Ey[i][j]
			f[j][i] = cur + dx + dy
	
	besti = 0
	for i in range(0,n):
		if f[m - 1][i] < f[m - 1][besti]:
			besti = i
	C = np.zeros((m)).astype(int)
	for j in range(0,m)[::-1]:
		C[j] = besti
		i = besti
		if j > 0:
			if i > 0:
				if f[j - 1][i - 1] < f[j - 1][besti]:
					besti = i - 1
			if i < n - 1:
				if f[j - 1][i + 1] < f[j - 1][besti]:
					besti = i + 1
	return C
def Cut_Row(img,R):
	n,m,d = img.shape
	out = np.zeros((n,m - 1,d))
	for i in range(0,n):
		k = 0
		for j in range(0,m):
			if j != R[i]:
				out[i][k] = img[i][j]
				k = k + 1
	return out

def Cut_Column(img,R):
	n,m,d = img.shape
	out = np.zeros((n - 1,m,d))
	for j in range(0,m):
		k = 0
		for i in range(0,n):
			if i != R[j]:
				out[k][j] = img[i][j]
				k = k + 1
	return out

def getDebug_Row(img,R):
	n,m,d = img.shape
	out = copy.deepcopy(img)
	for i in range(0,n):
		out[i][R[i]][0] = 0
		out[i][R[i]][1] = 0
		out[i][R[i]][2] = 255
	return out

def getDebug_Column(img,C):
	n,m,d = img.shape
	out = copy.deepcopy(img)
	for j in range(0,m):
		out[C[j]][j][0] = 255
		out[C[j]][j][1] = 0
		out[C[j]][j][2] = 0
	return out

def getSeamResult(img,S1,S2):
	n,m,d = img.shape
	A = np.zeros((n,m)).astype(int)
	B = np.zeros((n,m)).astype(int)

	out = copy.deepcopy(img)
	for i in range(0,n):
		for j in range(0,m):
			A[i][j] = j
			B[i][j] = i
	t1 = len(S1)
	for k in range(0,t1):
		R = S1[k]
		for i in range(0,n):
			x = B[i][R[i]]
			y = A[x][R[i]]

			out[x][y][0] = 0
			out[x][y][1] = 0
			out[x][y][2] = 255
			
			for j in range(R[i],m):
				if j < m - 1:
					A[i][j] = A[i][j + 1]
	
	for i in range(0,n):
		for j in range(0,m):
			B[i][j] = i

	t2 = len(S2)
	for k in range(0,t2):
		C = S2[k]
		for j in range(0,m - t1):
			x = B[C[j]][j]
			y = A[C[j]][j]

			out[x][y][0] = 255
			out[x][y][1] = 0
			out[x][y][2] = 0
			for i in range(C[j],n):
				if i < n - 1:
					B[i][j] = B[i + 1][j]
	return out

name = "1.jpg"
img = cv2.imread("source/" + name)
SaveImg = copy.deepcopy(img)

tRow,tColumn = 280,60

S1 = []
for tot in range(0,tRow):
	R = get_Row(getE_Sobel(img))
	Debug = getDebug_Row(img,R)
	cv2.imwrite("Debug/DebugImage_Row_" + str(tot) + ".jpg",Debug)
	img = Cut_Row(img,R)
	S1.append(R)

S2 = []
for tot in range(0,tColumn):
	C = get_Column(getE_Sobel(img))
	Debug = getDebug_Column(img,C)
	cv2.imwrite("Debug/DebugImage_Column_" + str(tot) + ".jpg",Debug)
	img = Cut_Column(img,C)
	S2.append(C)

cv2.imwrite("Result/out" + name, img)

cv2.imwrite("Result_with_Seam/out" + name,getSeamResult(SaveImg,S1,S2))

