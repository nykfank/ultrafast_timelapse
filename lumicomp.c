#include <stdlib.h> 
#include <gd.h>
#include <limits.h> 
#include <math.h> 

void lumi_compare(char **filename1, char **filename2, int *dist) {
	gdImagePtr img1,img2;
	int x,y,c1,c2;
	double sum,s;
	FILE *fp;
	fp = fopen(*filename1, "rb");
	if(fp == NULL) return; // unable to open
	img1=gdImageCreateFromJpeg(fp);
	fclose(fp);
	if(img1 == NULL) return; // not jpeg
	fp = fopen(*filename2, "rb");
	if(fp == NULL) {  // unable to open
		gdImageDestroy(img1);
		return;
	}
	img2=gdImageCreateFromJpeg(fp);
	fclose(fp);
	if(img2 == NULL) { // not jpeg
		gdImageDestroy(img1);
		return;
	}
	sum=0;
	for (x=0;x<img1->sx;x+=10) {
		for (y=0;y<img1->sy;y+=10) {
			c1=gdImageGetPixel(img1,x,y);
			c2=gdImageGetPixel(img2,x,y);
			s=0.299*abs(gdTrueColorGetRed(c1)-gdTrueColorGetRed(c2));
			s+=0.587*abs(gdTrueColorGetGreen(c1)-gdTrueColorGetGreen(c2));
			s+=0.114*abs(gdTrueColorGetBlue(c1)-gdTrueColorGetBlue(c2));
			sum+=s;
		} 
	}
	gdImageDestroy(img1);
	gdImageDestroy(img2);
	*dist = (int)sum;
	return;
}

// R CMD SHLIB lumicomp.c -lgd
// dyn.load("lumicomp.so")
// .C("lumi_compare", filename1="/home/nyk/1402395726.jpg", filename1="/home/nyk/1543661882.jpg", dist=.Machine$integer.max)
