#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstdlib>
#include <cstring>
#include <set>
#include <vector>
#include <map>
#include <cmath>

#define rep(i,l,r) for (i=l;i<=r;i++)
#define drep(i,l,r) for (i=l;i>=r;i--)
using namespace cv;

const int N = 3100;
typedef double db;

Mat img,Fimg,img2,Ex,Ey;
int R[N];
db e[N][N],f[N][N];

inline void getE()
{
	int i,j,n = img.rows,m = img.cols;
	Sobel(img, Ex, CV_16S, 1, 0); Sobel(img, Ey, CV_16S, 0, 1);
	convertScaleAbs(Ex,Ex); convertScaleAbs(Ey,Ey);
	cvtColor(Ex,Ex,COLOR_BGR2GRAY); cvtColor(Ey,Ey,COLOR_BGR2GRAY);
	uchar *o,*p;  db b,g,r;
	rep(i,0,n - 1)
	{
		p = Ex.ptr<uchar>(i); o = Ey.ptr<uchar>(i);
		rep(j,0,m - 1)
		{
			db x = (db)(*o++),y = (db)(*p++);
			e[i][j] = x + y;
		}
	}
}

inline void DP()
{
	int i,j,k,n = img.rows,m = img.cols;
	db cur;
	rep(i,0,n - 1) rep(j,0,m - 1)
	{
		cur = i ? (1e+10) : (0.0);
		if (i && j) cur = min(cur,f[i - 1][j - 1]);
		if (i) cur = min(cur,f[i - 1][j]);
		if (i && (j < m - 1)) cur = min(cur,f[i - 1][j + 1]);
		f[i][j] = cur + e[i][j];
	}
	k = 0; rep(j,0,m - 1) if (f[n - 1][j] < f[n - 1][k]) k = j;
	drep(i,n - 1,0)
	{
		R[i] = k; j = k;
		if (i && j) if (f[i - 1][j - 1] < f[i - 1][k]) k = j - 1;
		if (i && (j < m - 1)) if (f[i - 1][j + 1] < f[i - 1][k]) k = j + 1;
	}
}
inline void Cut()
{
	int i,j,k,n = img.rows,m = img.cols;
	img2 = Mat(Size(m - 1,n),CV_8UC3,Scalar(0));
	uchar *o,*p;
	rep(i,0,n - 1)
	{
		o = img2.ptr<uchar>(i); p = img.ptr<uchar>(i);
		rep(j,0,R[i] - 1){ *o++ = *p++; *o++ = *p++; *o++ = *p++;}
		if (R[i] != m - 1){ *p++; *p++; *p++;}
		rep(j,R[i] + 1,m - 1){*o++ = *p++; *o++ = *p++; *o++ = *p++;}
	}
	img = img2;
}

inline void Rotate()
{
	int i,j,n = img.rows,m = img.cols;
	transpose(img,img); flip(img,img,-1);
}

int main(int argc, char** argv)
{
	char fname[100],oname[100]; int tRow,tCol; db fR,fC;
	
	freopen("shrink.in","r",stdin);
	scanf("%s\n%s\n%lf %lf",fname,oname,&fR,&fC);
	fclose(stdin);

	img = imread(fname);
	printf("%d %d %d\n",img.rows,img.cols,img.channels());
	tRow = (int)((db)img.cols * fR);
	while (tRow--)
	{
		printf("Row Stil %d Left....\n",tRow);
		getE(); DP(); Cut();
	}
	
	tCol = (int)((db)img.rows * fC);
	Rotate();
	while (tCol--)
	{
		printf("Column Still %d Left....\n",tCol);
		getE(); DP(); Cut();
	}
	Rotate();
	imwrite(oname,img);
	
	return 0;
}
