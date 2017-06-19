#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstdlib>
#include <cstring>
#include <set>
#include <vector>
#include <map>
#include <cmath>
#include <ctime>

#define rep(i,l,r) for (i=l;i<=r;i++)
#define drep(i,l,r) for (i=l;i>=r;i--)
using namespace cv;

const int N = 3100;
typedef double db;

Mat img,Fimg,img2,Ex,Ey,save,simg;
int R[N],Pos[N][N][3],C[N][N][3],tcols,trows,isAdd,x1,x2,yl,y2;
db e[N][N],ec[N][N],ee[N][N],f[N][N],fR,fC;
char fname[100],oname[100],soname[100];
char Opt[100];

inline void Init()
{
	db cl,cr,cu,cd;
	freopen("work.in","r",stdin);
	scanf("%s\n%s\n%s\n%lf\n%lf\n%s\n",fname,oname,soname,&fC,&fR,Opt);
	scanf("%lf%lf%lf%lf",&cl,&cr,&cu,&cd);
	fclose(stdin);
	int i,j;
	img = imread(fname); save = img;
	rep(i,0,img.rows - 1) rep(j,0,img.cols - 1){Pos[i][j][0] = i; Pos[i][j][1] = j;}
	x1 = (int)(cl * (db)img.cols); x2 = (int)(cr * (db)img.cols);
	yl = (int)(cu * (db)img.rows); y2 = (int)(cd * (db)img.rows);
	rep(i,yl - 1,y2 - 1) rep(j,x1 - 1,x2 - 1)ec[i][j] = -1e+3;
	printf("%d %d %d\n",img.rows,img.cols,img.channels());
}
inline void getE()
{
	int i,j,n = img.rows,m = img.cols;
	if (!strcmp(Opt,"Sobel"))
	{
		Sobel(img, Ex, CV_16S, 1, 0); Sobel(img, Ey, CV_16S, 0, 1);
		convertScaleAbs(Ex,Ex); convertScaleAbs(Ey,Ey);
		cvtColor(Ex,Ex,COLOR_BGR2GRAY); cvtColor(Ey,Ey,COLOR_BGR2GRAY);
	}
	else
	{
		Laplacian(img, Ex, CV_16S); Laplacian(img, Ey, CV_16S);
		convertScaleAbs(Ex,Ex); convertScaleAbs(Ey,Ey);
		cvtColor(Ex,Ex,COLOR_BGR2GRAY); cvtColor(Ey,Ey,COLOR_BGR2GRAY);
	}
	uchar *o,*p;  db b,g,r;
	rep(i,0,n - 1)
	{
		p = Ex.ptr<uchar>(i); o = Ey.ptr<uchar>(i);
		rep(j,0,m - 1)
		{
			db x = (db)(*o++),y = (db)(*p++);
			e[i][j] = x + y + ec[i][j];
		}
	}
}

inline bool ck(db x, db y)
{
	if (x < y - 0.1) return 1; if (x > y + 0.1) return 0;
	return rand()&1;
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
	k = 0; rep(j,0,m - 1) if (ck(f[n - 1][j] , f[n - 1][k])) k = j; 
	drep(i,n - 1,0)
	{
		R[i] = k; j = k;
		if (i && j)if (ck(f[i - 1][j - 1] , f[i - 1][k])) k = j - 1; 
		if (i && (j < m - 1)) if (ck(f[i - 1][j + 1] ,f[i - 1][k])) k = j + 1;
	}
}
inline void Add()
{
	int i,j,k,n = img.rows,m = img.cols;
	img2 = Mat(Size(m + 1,n),CV_8UC3,Scalar(0));
	uchar *o,*p,b,g,r;
	rep(i,0,n - 1)
	{
		o = img2.ptr<uchar>(i); p = img.ptr<uchar>(i);
		rep(j,0,R[i]){C[i][j][0] = Pos[i][j][0];C[i][j][1] = Pos[i][j][1];}
		C[i][R[i] + 1][0] = Pos[i][R[i]][0];C[i][R[i] + 1][1] = Pos[i][R[i]][1];
		rep(j,R[i] + 2,m){C[i][j][0] = Pos[i][j - 1][0]; C[i][j][1] = Pos[i][j - 1][1];}
		rep(j,0,R[i] - 1){*o++ = *p++; *o++ = *p++; *o++ = *p++; ee[i][j] = ec[i][j];}
		//if (R[i] != m - 1)
		{
			b = *p++; g = *p++; r = *p++;
			*o++ = b; *o++ = g; *o++ = r;
			*o++ = b; *o++ = g; *o++ = r;
		}
		ee[i][R[i]] = ee[i][R[i] + 1] = ec[i][R[i]] + 49;
		rep(j,R[i] + 1,m - 1){*o++ = *p++; *o++ = *p++; *o++ = *p++; ee[i][j + 1] = ec[i][j];}
	}
	img = img2;
	rep(i,0,n - 1) rep(j,0,m){ec[i][j] = ee[i][j]; Pos[i][j][0] = C[i][j][0];Pos[i][j][1] = C[i][j][1];}
}

inline void Cut()
{
	int i,j,k,n = img.rows,m = img.cols;
	img2 = Mat(Size(m - 1,n),CV_8UC3,Scalar(0));
	uchar *o,*p;
	rep(i,0,n - 1)
	{
		o = img2.ptr<uchar>(i); p = img.ptr<uchar>(i);
		rep(j,0,R[i] - 1){C[i][j][0] = Pos[i][j][0];C[i][j][1] = Pos[i][j][1];ec[i][j] = ec[i][j];}
		rep(j,R[i],m - 2){C[i][j][0] = Pos[i][j + 1][0];C[i][j][1] = Pos[i][j + 1][1];ec[i][j] = ec[i][j + 1];}
		rep(j,0,R[i] - 1){ *o++ = *p++; *o++ = *p++; *o++ = *p++;}
		if (R[i] != m - 1){ *p++; *p++; *p++;}
		rep(j,R[i] + 1,m - 1){*o++ = *p++; *o++ = *p++; *o++ = *p++;}
	}
	img = img2;
	rep(i,0,n - 1) rep(j,0,m - 2) {Pos[i][j][0] = C[i][j][0]; Pos[i][j][1] = C[i][j][1];}
}

inline void Rotate()
{
	int i,j,n = img.rows,m = img.cols;
	transpose(img,img); flip(img,img,1);
	rep(i,0,m - 1) rep(j,0,n - 1) {C[i][j][0] = Pos[n - 1 - j][i][0]; C[i][j][1] = Pos[n - 1 - j][i][1]; ee[i][j] = ec[n - 1 - j][i];}
	rep(i,0,m - 1) rep(j,0,n - 1) {Pos[i][j][0] = C[i][j][0]; Pos[i][j][1] = C[i][j][1]; ec[i][j] = ee[i][j];}
}

int mark[N][N];
uchar color[N][N][4];
inline void getseam()
{
	int i,j,k,nn = img.rows,mm = img.cols,n = save.rows,m = save.cols,x,y;
	uchar *o,*p,b,g,r;
	std :: memset(mark,0,sizeof(mark));
	rep(i,0,nn - 1) rep(j,0,mm - 1) mark[Pos[i][j][0]][Pos[i][j][1]]++;
	
	if ((nn <= n) && (mm <= m))
	{
		simg = Mat(Size(m,n),CV_8UC3,Scalar(0));
		rep(i,0,n - 1) 
		{
			o = simg.ptr<uchar>(i); p = save.ptr<uchar>(i);
			rep(j,0,m - 1)
			{
				b = *p++; g = *p++; r = *p++;
				if (mark[i][j]) {*o++ = b; *o++ = g; *o++ = r;}
				else {*o++ = 0; *o++ = 0; *o++ = 255;}
			}
		}
	}
	else if ((nn >= n) && (mm >= m))
	{
		simg = Mat(Size(mm,nn),CV_8UC3,Scalar(0));
		rep(i,0,n - 1)
		{
			o = save.ptr<uchar>(i);
			rep(j,0,m - 1) {color[i][j][0] = *o++; color[i][j][1] = *o++; color[i][j][2] = *o++;}
		}
		rep(i,0,nn - 1)
		{
			o = simg.ptr<uchar>(i);
			rep(j,0,mm - 1)
			{
				x = Pos[i][j][0]; y = Pos[i][j][1];
				if (mark[x][y] > 1)
				{
					*o++ = 0; *o++ = 255; *o++ = 0;				
				}
				else {*o++ = color[x][y][0]; *o++ = color[x][y][1]; *o++ = color[x][y][2];}
			}
		}
	}
	else simg = img;
}
int main(int argc, char** argv)
{
	std :: memset(ec,0,sizeof(ec));
	Init(); srand(time(0));
	tcols = (int)((db)img.cols * fC) - img.cols; isAdd = tcols > 0 ? 1 : -1; tcols *= isAdd;
	trows = (int)((db)img.rows * fR) - img.rows; isAdd = trows > 0 ? 1 : -1; trows *= isAdd;
	if (tcols + trows > 0)
	{
		while (tcols--)
		{
			printf("Column Stil %d Left....\n",tcols);
			getE(); DP(); if (isAdd > 0) Add(); else Cut();
		}
		
		std :: memset(ec,0,sizeof(ec));
		Rotate();
		while (trows--)
		{
			printf("Row Still %d Left....\n",trows);
			getE(); DP(); if (isAdd > 0) Add(); else Cut();
		}
		Rotate(); Rotate(); Rotate();
		imwrite(oname,img);
		getseam();
		imwrite(soname,simg);
	}
	else
	{
		tcols = (x2 - x1); trows = (y2 - yl);
		int tc = 0,tr = 0;
		while (tcols && trows)
		{
			if (rand()&1){tc++; getE(); DP(); Cut(); tcols--;}
			else {tr++; Rotate();getE();DP();Cut();Rotate();Rotate();Rotate(); trows--;}
			printf("Cut Column %d Rows %d\n",tcols,trows);
		}
		getseam(); imwrite(soname,simg);
		std :: memset(ec,0,sizeof(ec));
		while (tc--){getE();DP();Add();}
		Rotate(); while(tr--){getE();DP();Add();}Rotate();Rotate();Rotate();
		imwrite(oname,img);
	}
	return 0;
}
