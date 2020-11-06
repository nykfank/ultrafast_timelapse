// imgtools python module, 2011-10-28 by Nick Fankhauser
// edge: if the returned value for a jpeg-image is above 20 (at threshold 10 and quant 1), the photo is in-focus.
// Required to compile: 
// make swig binutils gcc gcc-4.4 libc-dev-bin libc6-dev libgomp1 linux-libc-dev manpages-dev libexpat1-dev libfontconfig1-dev libfreetype6-dev libgd2-xpm-dev libjpeg62-dev libpng12-dev libpthread-stubs0 libpthread-stubs0-dev libx11-dev libxau-dev libxcb1-dev libxdmcp-dev libxpm-dev pkg-config x11proto-core-dev x11proto-input-dev x11proto-kb-dev xtrans-dev zlib1g-dev libpython2.6  libssl-dev  python-dev python2.6-dev

// Required to run: libgd2-xpm


#include <stdlib.h> 
#include <gd.h>
#include <limits.h> 
#include <math.h> 


int edge(char *filename, int threshold, int quant) {
	gdImagePtr img;
	int x,y,c,xi,yi,ci,red,green,blue,avg;
	long area,count;
	FILE *fp;
	fp = fopen(filename, "rb");
	if(fp == NULL) {return 0;} // unable to open
	img=gdImageCreateFromJpeg(fp);
	fclose(fp);
 	if(img == NULL) {return 0;} // not jpeg
	count=0;
	area=img->sx/quant * img->sy/quant;
	for (x=1;x<img->sx;x+=quant) {
		for (y=1;y<img->sy;y+=quant) {
			c=gdImageGetPixel(img,x,y);
			for (xi=-1;xi<2;xi++) {
				for (yi=-1;yi<2;yi++) {
					if (xi==yi) {continue;}
					ci=gdImageGetPixel(img,x+xi,y+yi);
					red=abs(gdTrueColorGetRed(c)-gdTrueColorGetRed(ci));
					green=abs(gdTrueColorGetGreen(c)-gdTrueColorGetGreen(ci));
					blue=abs(gdTrueColorGetBlue(c)-gdTrueColorGetBlue(ci));
					avg=(red+green+blue)/3;
     					if (avg>threshold) {count++;break;}
				}
				if (avg>threshold) {break;}
			}
		} 
	}
	gdImageDestroy(img);
	return (int)((float)count/(float)area*100);
}

long compare(char *filename1, char *filename2, int quant) {
	gdImagePtr img1,img2;
	int x,y,c1,c2;
	long sum,s;
	FILE *fp;
	fp = fopen(filename1, "rb");
	if(fp == NULL) return LONG_MAX; // unable to open
	img1=gdImageCreateFromJpeg(fp);
	fclose(fp);
	if(img1 == NULL) return LONG_MAX; // not jpeg
	fp = fopen(filename2, "rb");
	if(fp == NULL) {  // unable to open
		gdImageDestroy(img1);
		return LONG_MAX;
	}
	img2=gdImageCreateFromJpeg(fp);
	fclose(fp);
	if(img2 == NULL) { // not jpeg
		gdImageDestroy(img1);
		return LONG_MAX;
	}
	sum=0;
	for (x=0;x<img1->sx;x+=quant) {
		for (y=0;y<img1->sy;y+=quant) {
			c1=gdImageGetPixel(img1,x,y);
			c2=gdImageGetPixel(img2,x,y);
			s=abs(gdTrueColorGetRed(c1)-gdTrueColorGetRed(c2));
			s+=abs(gdTrueColorGetGreen(c1)-gdTrueColorGetGreen(c2));
			s+=abs(gdTrueColorGetBlue(c1)-gdTrueColorGetBlue(c2));
			sum+=s/3;
		} 
	}
	gdImageDestroy(img1);
	gdImageDestroy(img2);
	return sum;
}

long lumi_compare(char *filename1, char *filename2, int quant) {
	gdImagePtr img1,img2;
	int x,y,c1,c2;
	double sum,s;
	FILE *fp;
	fp = fopen(filename1, "rb");
	if(fp == NULL) return LONG_MAX; // unable to open
	img1=gdImageCreateFromJpeg(fp);
	fclose(fp);
	if(img1 == NULL) return LONG_MAX; // not jpeg
	fp = fopen(filename2, "rb");
	if(fp == NULL) {  // unable to open
		gdImageDestroy(img1);
		return LONG_MAX;
	}
	img2=gdImageCreateFromJpeg(fp);
	fclose(fp);
	if(img2 == NULL) { // not jpeg
		gdImageDestroy(img1);
		return LONG_MAX;
	}
	sum=0;
	for (x=0;x<img1->sx;x+=quant) {
		for (y=0;y<img1->sy;y+=quant) {
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
	return (long)sum;
}


long compare_png(char *filename1, char *filename2, int quant) {
	gdImagePtr img1,img2;
	int x,y,c1,c2;
	long sum,s;
	FILE *fp;
	fp = fopen(filename1, "rb");
	if(fp == NULL) return LONG_MAX; // unable to open
	img1=gdImageCreateFromPng(fp);
	fclose(fp);
	if(img1 == NULL) return LONG_MAX; // not jpeg
	fp = fopen(filename2, "rb");
	if(fp == NULL) {  // unable to open
		gdImageDestroy(img1);
		return LONG_MAX;
	}
	img2=gdImageCreateFromPng(fp);
	fclose(fp);
	if(img2 == NULL) { // not jpeg
		gdImageDestroy(img1);
		return LONG_MAX;
	}
	sum=0;
	for (x=0;x<img1->sx;x+=quant) {
		for (y=0;y<img1->sy;y+=quant) {
			c1=gdImageGetPixel(img1,x,y);
			c2=gdImageGetPixel(img2,x,y);
			s=abs(gdTrueColorGetRed(c1)-gdTrueColorGetRed(c2));
			s+=abs(gdTrueColorGetGreen(c1)-gdTrueColorGetGreen(c2));
			s+=abs(gdTrueColorGetBlue(c1)-gdTrueColorGetBlue(c2));
			sum+=s/3;
		} 
	}
	gdImageDestroy(img1);
	gdImageDestroy(img2);
	return sum;
}


int int_cmp(const void *a, const void *b) { 
    const int *ia = (const int *)a; // casting pointer types 
    const int *ib = (const int *)b;
    return *ia  - *ib; 
} 

int median_merge(int argc, char **argv) {
	gdImagePtr *images, median_image;
	int x,y,c,i,j;
	int median_red, median_green, median_blue;
	int *red, *blue, *green;
	FILE *fp;
	int nb_images=argc-1;
	fp = fopen(argv[nb_images], "rb");
	if(fp != NULL) { // output exists
		fclose(fp);
		return 1; 
	}
	images = malloc(sizeof(gdImagePtr) * nb_images);
	for (i=0;i<nb_images;i++) {
		fp = fopen(argv[i], "rb");
		if(fp == NULL) { // unable to open
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 2;
		}
		images[i]=gdImageCreateFromJpeg(fp);
		fclose(fp);
		if(images[i] == NULL) {  // not jpeg
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 3;
		}
	}
	red=malloc(sizeof(int) * nb_images);
	green=malloc(sizeof(int) * nb_images);
	blue=malloc(sizeof(int) * nb_images);
	median_image=gdImageCreateTrueColor(images[0]->sx,images[0]->sy);
	for (x=0;x<images[0]->sx;x++) {
		for (y=0;y<images[0]->sy;y++) {
			for (i=0;i<nb_images;i++) {
				c=gdImageGetPixel(images[i],x,y);
				red[i]=gdTrueColorGetRed(c);
				green[i]=gdTrueColorGetGreen(c);
				blue[i]=gdTrueColorGetBlue(c);
			}
			qsort(red, nb_images, sizeof(int), int_cmp);
			qsort(green, nb_images, sizeof(int), int_cmp);
			qsort(blue, nb_images, sizeof(int), int_cmp);
			median_red=red[nb_images/2];
			median_green=green[nb_images/2];
			median_blue=blue[nb_images/2];
			c = gdImageColorAllocate(median_image, median_red, median_green, median_blue); 
			gdImageSetPixel(median_image, x, y, c);
		} 
	}
	fp = fopen(argv[nb_images], "wb");
	gdImageJpeg(median_image, fp, 75);
	fclose(fp);
	gdImageDestroy(median_image);
	free(blue);free(green);free(red);
	for (i=0;i<nb_images;i++) gdImageDestroy(images[i]);
	free(images);
	return 0;
}

int dark(char *filename, int threshold, int quant) {
	gdImagePtr img;
	int x,y,c,area,count;
	FILE *fp;
	fp = fopen(filename, "rb");
	if(fp == NULL) {return -1;} // unable to open
	img=gdImageCreateFromJpeg(fp);
	fclose(fp);
 	if(img == NULL) {return -1;} // not jpeg
	count=0;
	area=img->sx/quant * img->sy/quant;
	for (x=0;x<img->sx;x+=quant) {
		for (y=0;y<img->sy;y+=quant) {
			c=gdImageGetPixel(img,x,y);
			if ((gdTrueColorGetRed(c)<threshold) && 
			(gdTrueColorGetGreen(c)<threshold) && 
			(gdTrueColorGetBlue(c)<threshold)) {count++;}
		} 
	}
	gdImageDestroy(img);
	return (int)((float)count/(float)area*100);
}

int sdev_merge(int argc, char **argv) {
	gdImagePtr *images, median_image;
	int x,y,c,i,j;
	int std_red, std_green, std_blue;
	int avg_red, avg_green, avg_blue;
	int *red, *blue, *green;
	long redsum,greensum,bluesum;
	long redsum2,greensum2,bluesum2;
	FILE *fp;
	int nb_images=argc-1;
	fp = fopen(argv[nb_images], "rb");
	if(fp != NULL) { // output exists
		fclose(fp);
		return 1; 
	}
	images = malloc(sizeof(gdImagePtr) * nb_images);
	for (i=0;i<nb_images;i++) {
		fp = fopen(argv[i], "rb");
		if(fp == NULL) { // unable to open
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 2;
		}
		images[i]=gdImageCreateFromJpeg(fp);
		fclose(fp);
		if(images[i] == NULL) {  // not jpeg
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 3;
		}
	}
	red=malloc(sizeof(int) * nb_images);
	green=malloc(sizeof(int) * nb_images);
	blue=malloc(sizeof(int) * nb_images);
	median_image=gdImageCreateTrueColor(images[0]->sx,images[0]->sy);
	for (x=0;x<images[0]->sx;x++) {
		for (y=0;y<images[0]->sy;y++) {
			redsum=0;
			greensum=0;
			bluesum=0;
			for (i=0;i<nb_images;i++) {
				c=gdImageGetPixel(images[i],x,y);
				red[i]=gdTrueColorGetRed(c);
				green[i]=gdTrueColorGetGreen(c);
				blue[i]=gdTrueColorGetBlue(c);
				redsum+=gdTrueColorGetRed(c);
				greensum+=gdTrueColorGetGreen(c);
				bluesum+=gdTrueColorGetBlue(c);
			}
			avg_red=redsum/nb_images;  // check if the division works with ints!
			avg_green=greensum/nb_images;
			avg_blue=bluesum/nb_images;
			redsum2=0;
			greensum2=0;
			bluesum2=0;		
			for (i=0;i<nb_images;i++) {
				redsum2+=abs(red[i]-avg_red);
				greensum2+=abs(green[i]-avg_green);
				bluesum2+=abs(blue[i]-avg_blue);
			}
			std_red=2*redsum2/nb_images;
			std_green=2*greensum2/nb_images;
			std_blue=2*bluesum2/nb_images;
			c = gdImageColorAllocate(median_image, std_red, std_green, std_blue); 
			gdImageSetPixel(median_image, x, y, c);
		} 
	}
	fp = fopen(argv[nb_images], "wb");
	gdImageJpeg(median_image, fp, 75);
	fclose(fp);
	gdImageDestroy(median_image);
	free(blue);free(green);free(red);
	for (i=0;i<nb_images;i++) gdImageDestroy(images[i]);
	free(images);
	return 0;
}

int mean_merge_png(int argc, char **argv, int faktor1, int faktor2) {
	gdImagePtr *images, mean_image;
	int x,y,c,i,j;
	int avg_red, avg_green, avg_blue;
	long redsum,greensum,bluesum;
	FILE *fp;
	int nb_images=argc-1;
	fp = fopen(argv[nb_images], "rb");
	if(fp != NULL) { // output exists
		fclose(fp);
		return 1; 
	}
	images = malloc(sizeof(gdImagePtr) * nb_images);
	for (i=0;i<nb_images;i++) {
		fp = fopen(argv[i], "rb");
		if(fp == NULL) { // unable to open
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 2;
		}
		images[i]=gdImageCreateFromPng(fp);
		fclose(fp);
		if(images[i] == NULL) {  // not PNG
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 3;
		}
	}
	mean_image=gdImageCreateTrueColor(images[0]->sx,images[0]->sy);
	for (x=0;x<images[0]->sx;x++) {
		for (y=0;y<images[0]->sy;y++) {
			redsum=0;
			greensum=0;
			bluesum=0;
			c=gdImageGetPixel(images[0],x,y);
			redsum+=faktor1 * gdTrueColorGetRed(c);
			greensum+=faktor1 * gdTrueColorGetGreen(c);
			bluesum+=faktor1 * gdTrueColorGetBlue(c);
			c=gdImageGetPixel(images[1],x,y);
			redsum+=faktor2 * gdTrueColorGetRed(c);
			greensum+=faktor2 * gdTrueColorGetGreen(c);
			bluesum+=faktor2 * gdTrueColorGetBlue(c);
			avg_red=redsum/(faktor1+faktor2);
			avg_green=greensum/(faktor1+faktor2);
			avg_blue=bluesum/(faktor1+faktor2);
			c = gdImageColorAllocate(mean_image, avg_red, avg_green, avg_blue); 
			gdImageSetPixel(mean_image, x, y, c);
		} 
	}
	fp = fopen(argv[nb_images], "wb");
	gdImagePng(mean_image, fp);
	fclose(fp);
	gdImageDestroy(mean_image);
	for (i=0;i<nb_images;i++) gdImageDestroy(images[i]);
	free(images);
	return 0;
}

int mean_merge(int argc, char **argv, int faktor1, int faktor2) {
	gdImagePtr *images, mean_image;
	int x,y,c,i,j;
	int avg_red, avg_green, avg_blue;
	long redsum,greensum,bluesum;
	FILE *fp;
	int nb_images=argc-1;
	fp = fopen(argv[nb_images], "rb");
	if(fp != NULL) { // output exists
		fclose(fp);
		return 1; 
	}
	images = malloc(sizeof(gdImagePtr) * nb_images);
	for (i=0;i<nb_images;i++) {
		fp = fopen(argv[i], "rb");
		if(fp == NULL) { // unable to open
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 2;
		}
		images[i]=gdImageCreateFromJpeg(fp);
		fclose(fp);
		if(images[i] == NULL) {  // not jpeg
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 3;
		}
	}
	mean_image=gdImageCreateTrueColor(images[0]->sx,images[0]->sy);
	for (x=0;x<images[0]->sx;x++) {
		for (y=0;y<images[0]->sy;y++) {
			redsum=0;
			greensum=0;
			bluesum=0;
			c=gdImageGetPixel(images[0],x,y);
			redsum+=faktor1 * gdTrueColorGetRed(c);
			greensum+=faktor1 * gdTrueColorGetGreen(c);
			bluesum+=faktor1 * gdTrueColorGetBlue(c);
			c=gdImageGetPixel(images[1],x,y);
			redsum+=faktor2 * gdTrueColorGetRed(c);
			greensum+=faktor2 * gdTrueColorGetGreen(c);
			bluesum+=faktor2 * gdTrueColorGetBlue(c);
			avg_red=redsum/(faktor1+faktor2);
			avg_green=greensum/(faktor1+faktor2);
			avg_blue=bluesum/(faktor1+faktor2);
			c = gdImageColorAllocate(mean_image, avg_red, avg_green, avg_blue); 
			gdImageSetPixel(mean_image, x, y, c);
		} 
	}
	fp = fopen(argv[nb_images], "wb");
	gdImageJpeg(mean_image, fp, 75);
	fclose(fp);
	gdImageDestroy(mean_image);
	for (i=0;i<nb_images;i++) gdImageDestroy(images[i]);
	free(images);
	return 0;
}

int lumi(char *filename,int quant) {
	gdImagePtr img;
	int x,y,c;
	double sum,area;
	FILE *fp;
	fp = fopen(filename, "rb");
	if(fp == NULL) {return 0;} // unable to open
	img=gdImageCreateFromJpeg(fp);
	fclose(fp);
 	if(img == NULL) {return 0;} // not jpeg
	area=img->sx/quant * img->sy/quant;
	for (x=0;x<img->sx;x+=quant) {
		for (y=0;y<img->sy;y+=quant) {
			c=gdImageGetPixel(img,x,y);
			sum+=gdTrueColorGetRed(c)*0.299+gdTrueColorGetGreen(c)*0.587+gdTrueColorGetBlue(c)*0.114;
		} 
	}
	gdImageDestroy(img);
	return (int)(sum/area);
}

long lumi_area(char *filename,int quant) {
	gdImagePtr img;
	int x,y,c;
	double sum,area;
	FILE *fp;
	fp = fopen(filename, "rb");
	if(fp == NULL) {return 0;} // unable to open
	img=gdImageCreateFromJpeg(fp);
	fclose(fp);
 	if(img == NULL) {return 0;} // not jpeg
	//area=img->sx/quant * img->sy/quant;
	for (x=0;x<img->sx;x+=quant) {
		for (y=0;y<img->sy;y+=quant) {
			c=gdImageGetPixel(img,x,y);
			sum+=gdTrueColorGetRed(c)*0.299+gdTrueColorGetGreen(c)*0.587+gdTrueColorGetBlue(c)*0.114;
		} 
	}
	gdImageDestroy(img);
	return (long)sum;
	//return (int)(sum/area);
}

int sdev_merge_png(int argc, char **argv) { // Copy of sdev_merge with PNG output
	gdImagePtr *images, median_image;
	int x,y,c,i,j;
	int std_red, std_green, std_blue;
	int avg_red, avg_green, avg_blue;
	int *red, *blue, *green;
	long redsum,greensum,bluesum;
	long redsum2,greensum2,bluesum2;
	FILE *fp;
	int nb_images=argc-1;
	fp = fopen(argv[nb_images], "rb");
	if(fp != NULL) { // output exists
		fclose(fp);
		return 1; 
	}
	images = malloc(sizeof(gdImagePtr) * nb_images);
	for (i=0;i<nb_images;i++) {
		fp = fopen(argv[i], "rb");
		if(fp == NULL) { // unable to open
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 2;
		}
		images[i]=gdImageCreateFromJpeg(fp);
		fclose(fp);
		if(images[i] == NULL) {  // not jpeg
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 3;
		}
	}
	red=malloc(sizeof(int) * nb_images);
	green=malloc(sizeof(int) * nb_images);
	blue=malloc(sizeof(int) * nb_images);
	median_image=gdImageCreateTrueColor(images[0]->sx,images[0]->sy);
	for (x=0;x<images[0]->sx;x++) {
		for (y=0;y<images[0]->sy;y++) {
			redsum=0;
			greensum=0;
			bluesum=0;
			for (i=0;i<nb_images;i++) {
				c=gdImageGetPixel(images[i],x,y);
				red[i]=gdTrueColorGetRed(c);
				green[i]=gdTrueColorGetGreen(c);
				blue[i]=gdTrueColorGetBlue(c);
				redsum+=gdTrueColorGetRed(c);
				greensum+=gdTrueColorGetGreen(c);
				bluesum+=gdTrueColorGetBlue(c);
			}
			avg_red=redsum/nb_images;  // check if the division works with ints!
			avg_green=greensum/nb_images;
			avg_blue=bluesum/nb_images;
			redsum2=0;
			greensum2=0;
			bluesum2=0;		
			for (i=0;i<nb_images;i++) {
				redsum2+=abs(red[i]-avg_red);
				greensum2+=abs(green[i]-avg_green);
				bluesum2+=abs(blue[i]-avg_blue);
			}
			std_red=2*redsum2/nb_images;
			std_green=2*greensum2/nb_images;
			std_blue=2*bluesum2/nb_images;
			c = gdImageColorAllocate(median_image, std_red, std_green, std_blue); 
			gdImageSetPixel(median_image, x, y, c);
		} 
	}
	fp = fopen(argv[nb_images], "wb");
	gdImagePng(median_image, fp);
	fclose(fp);
	gdImageDestroy(median_image);
	free(blue);free(green);free(red);
	for (i=0;i<nb_images;i++) gdImageDestroy(images[i]);
	free(images);
	return 0;
}

int sdev_merge_png_to_png(int argc, char **argv) { // Copy of sdev_merge with PNG output
	gdImagePtr *images, median_image;
	int x,y,c,i,j;
	int std_red, std_green, std_blue;
	int avg_red, avg_green, avg_blue;
	int *red, *blue, *green;
	long redsum,greensum,bluesum;
	long redsum2,greensum2,bluesum2;
	FILE *fp;
	int nb_images=argc-1;
	fp = fopen(argv[nb_images], "rb");
	if(fp != NULL) { // output exists
		fclose(fp);
		return 1; 
	}
	images = malloc(sizeof(gdImagePtr) * nb_images);
	for (i=0;i<nb_images;i++) {
		fp = fopen(argv[i], "rb");
		if(fp == NULL) { // unable to open
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 2;
		}
		images[i]=gdImageCreateFromPng(fp);
		fclose(fp);
		if(images[i] == NULL) {  // not PNG
			for (j=0;j<i;j++) gdImageDestroy(images[j]);
			free(images);
			return 3;
		}
	}
	red=malloc(sizeof(int) * nb_images);
	green=malloc(sizeof(int) * nb_images);
	blue=malloc(sizeof(int) * nb_images);
	median_image=gdImageCreateTrueColor(images[0]->sx,images[0]->sy);
	for (x=0;x<images[0]->sx;x++) {
		for (y=0;y<images[0]->sy;y++) {
			redsum=0;
			greensum=0;
			bluesum=0;
			for (i=0;i<nb_images;i++) {
				c=gdImageGetPixel(images[i],x,y);
				red[i]=gdTrueColorGetRed(c);
				green[i]=gdTrueColorGetGreen(c);
				blue[i]=gdTrueColorGetBlue(c);
				redsum+=gdTrueColorGetRed(c);
				greensum+=gdTrueColorGetGreen(c);
				bluesum+=gdTrueColorGetBlue(c);
			}
			avg_red=redsum/nb_images;  // check if the division works with ints!
			avg_green=greensum/nb_images;
			avg_blue=bluesum/nb_images;
			redsum2=0;
			greensum2=0;
			bluesum2=0;		
			for (i=0;i<nb_images;i++) {
				redsum2+=abs(red[i]-avg_red);
				greensum2+=abs(green[i]-avg_green);
				bluesum2+=abs(blue[i]-avg_blue);
			}
			std_red=2*redsum2/nb_images;
			std_green=2*greensum2/nb_images;
			std_blue=2*bluesum2/nb_images;
			c = gdImageColorAllocate(median_image, std_red, std_green, std_blue); 
			gdImageSetPixel(median_image, x, y, c);
		} 
	}
	fp = fopen(argv[nb_images], "wb");
	gdImagePng(median_image, fp);
	fclose(fp);
	gdImageDestroy(median_image);
	free(blue);free(green);free(red);
	for (i=0;i<nb_images;i++) gdImageDestroy(images[i]);
	free(images);
	return 0;
}
