import cv2
import numpy as np
import copy


def getE_Sobel(img):
	x = cv2.convertScaleAbs(cv2.Sobel(img,cv2.CV_16S,1,0))
	y = cv2.convertScaleAbs(cv2.Sobel(img,cv2.CV_16S,0,1))
	x = cv2.cvtColor(x,cv2.COLOR_BGR2GRAY)
	y = cv2.cvtColor(y,cv2.COLOR_BGR2GRAY)
	return (x,y)

def get_Row(Exy,Er):
	(Ex,Ey) = Exy
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
			f[i][j] = cur + dx + dy + Er[i][j]
	
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

def get_Column(Exy,Ec):
	(Ex,Ey) = Exy
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
			f[j][i] = cur + dx + dy + Ec[i][j]
	
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

def amp_Row(img,R,Er):
	n,m,d = img.shape
	out = np.zeros((n,m + 1,d))
	nEr = np.zeros((n,m + 1))
	for i in range(0,n):
		k = 0
		for j in range(0,m):
			if j != R[i]:
				out[i][k] = img[i][j]
				nEr[i][k] = Er[i][j]
				k = k + 1
			else:
				out[i][k] = out[i][k + 1] = img[i][j]
				nEr[i][k] = nEr[i][k + 1] = Er[i][j] +  1e+5 
				k = k + 2
	return (out,nEr)

def amp_Column(img,C,Ec):
	n,m,d = img.shape
	out = np.zeros((n + 1,m,d))
	nEc = np.zeros((n + 1,m))
	for j in range(0,m):
		k = 0
		for i in range(0,n):
			if i != C[j]:
				out[k][j] = img[i][j]
				nEc[k][j] = Ec[i][j]
				k = k + 1
			else:
				out[k][j] = out[k + 1][j] = img[i][j]
				nEc[k][j] = nEc[k + 1][j] = Ec[i][j] + 1e+5
				k = k + 2
	return (out,nEc)

name = "x23(2).jpg"
img = cv2.imread("source/" + name)

tRow,tColumn = 100,0
Er = np.zeros((img.shape[0],img.shape[1]))
for i in range(0,tRow):
	R = get_Row(getE_Sobel(img),Er)
	print 'Er.shape',Er.shape
	(img,Er) = amp_Row(img,R,Er)

Ec = np.zeros((img.shape[0],img.shape[1]))
for i in range(0,tColumn):
	C = get_Column(getE_Sobel(img),Ec)
	print 'Ec.shape',Ec.shape
	(img,Ec) = amp_Column(img,C,Ec)

cv2.imwrite("Result/out_amplify_" + name, img)

