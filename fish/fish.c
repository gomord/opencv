#include <stdio.h>
#include <time.h>
#include <cv.h>
#include "highgui.h"
#include "mouse.h"
#include "../ctlMotor/motor.h"
#define SHOW_IMAGE 1
#define AVG_POS 1
#define RANGE_POS 2
int        g_slider_hue_max = 63 + 17;
int        g_slider_hue_min = 63 - 17;
int        g_slider_sat_max = 59 + 19;
int        g_slider_sat_min = 59 - 19;
int        g_slider_val_max = 49 + 20;
int        g_slider_val_min = 49 - 20;
int        g_mouse	    = 0;
CvRect	   g_range;  
int	   g_is_range = 1;
CvScalar   g_hsv_max;
CvScalar   g_hsv_min;
CvCapture* g_capture         = NULL;

int     frames = 0;
void onTrackbarSlide(int pos) {
	g_hsv_max = cvScalar(g_slider_hue_max,
			     g_slider_sat_max,
			     g_slider_val_max,0
			);
	g_hsv_min = cvScalar(g_slider_hue_min,
			     g_slider_sat_min,
			     g_slider_val_min,0
			);
	/*cvSetCaptureProperty(
	  g_capture,
	  CV_CAP_PROP_POS_FRAMES,
	  pos
	  );*/
	//frames = pos;
	//g_slider_position = pos;
}

void onTrackbarMouse(int pos){
	printf("test pos %d, g_mouse %d\n",pos, g_mouse);
}
void set_mouse_bar(const char* win_name){
	cvCreateTrackbar("null-avg-range",
			win_name,
			&g_mouse,
			2,
			onTrackbarMouse);
}
void set_trac_bar(const char* win_name){
	cvCreateTrackbar(
			"Hue Max",
			win_name,
			&g_slider_hue_max,
			255,
			onTrackbarSlide
			);
	cvCreateTrackbar(
			"Hue Min",
			win_name,
			&g_slider_hue_min,
			255,
			onTrackbarSlide
			);
	cvCreateTrackbar(
			"Sat Max",
			win_name,
			&g_slider_sat_max,
			255,
			onTrackbarSlide
			);
	cvCreateTrackbar(
			"Sat Min",
			win_name,
			&g_slider_sat_min,
			255,
			onTrackbarSlide
			);
	cvCreateTrackbar(
			"Val Max",
			win_name,
			&g_slider_sat_max,
			255,
			onTrackbarSlide
			);
	cvCreateTrackbar(
			"Val Min",
			win_name,
			&g_slider_val_min,
			255,
			onTrackbarSlide
			);
}
void set_sliders_filter_hsv(const char * win_name, CvScalar *avg,CvScalar *stn){
        g_slider_hue_max =  avg->val[0]+1*stn->val[0];
	//cvSetTrackbarPos("Hue Max",win_name,g_slider_hue_max );
        g_slider_sat_max =  avg->val[1]+2*stn->val[1];
	//cvSetTrackbarPos("Sat Max",win_name,g_slider_sat_max );
        g_slider_val_max =  avg->val[2]+2*stn->val[2];
	//cvSetTrackbarPos("Val Max",win_name,g_slider_val_max );
        g_slider_hue_min =  avg->val[0]-1*stn->val[0];
	//cvSetTrackbarPos("Hue Min",win_name,g_slider_hue_min );
        g_slider_sat_min =  avg->val[1]-2*stn->val[1];
	//cvSetTrackbarPos("Sat Min",win_name,g_slider_sat_min );
        g_slider_val_min =  avg->val[2]-2*stn->val[2];
	//cvSetTrackbarPos("Val Min",win_name,g_slider_val_min );
	onTrackbarSlide(0);

}
void set_sliders_filter(const char * win_name, CvScalar *avg,CvScalar *stn){
        g_slider_hue_max =  avg->val[0]+2*stn->val[0];
	//cvSetTrackbarPos("Hue Max",win_name,g_slider_hue_max );
        g_slider_sat_max =  avg->val[1]+2*stn->val[1];
	//cvSetTrackbarPos("Sat Max",win_name,g_slider_sat_max );
        g_slider_val_max =  avg->val[2]+2*stn->val[2];
	//cvSetTrackbarPos("Val Max",win_name,g_slider_val_max );
        g_slider_hue_min =  avg->val[0]-2*stn->val[0];
	//cvSetTrackbarPos("Hue Min",win_name,g_slider_hue_min );
        g_slider_sat_min =  avg->val[1]-2*stn->val[1];
	//cvSetTrackbarPos("Sat Min",win_name,g_slider_sat_min );
        g_slider_val_min =  avg->val[2]-2*stn->val[2];
	//cvSetTrackbarPos("Val Min",win_name,g_slider_val_min );
	onTrackbarSlide(0);

}
clock_t begin, end;
double	time_spent;
MouseParams g_msPrm = {0};
int set_avg_pos( IplImage* frame){
	CvScalar  avgScalar;
	CvScalar  stnScalar;
	cvSetImageROI(frame,g_msPrm.box);
	cvAvgSdv(frame,&avgScalar,&stnScalar,NULL);
	set_sliders_filter("set_HSV",&avgScalar,&stnScalar);
	printf("avg h - %f s - %f v - %f\n",
			avgScalar.val[0],
			avgScalar.val[1],
			avgScalar.val[2]
	      );
	printf("avg hs - %f ss - %f vs - %f\n",
			stnScalar.val[0],
			stnScalar.val[1],
			stnScalar.val[2]
	      );
	cvResetImageROI(frame);
	//cvCvtColor(frame,frame,CV_HSV2RGB);
	draw_box(frame,g_msPrm.box);
	if(SHOW_IMAGE == 1)
		cvShowImage("Camera",frame);
	return 0;

}
int set_range_pos( IplImage* frame){
	draw_box(frame,g_msPrm.box);
	g_range = g_msPrm.box;
	printf("range %d %d %d %d\n",
		g_range.x,g_range.y,g_range.width,g_range.height);
	g_is_range = 1;
	if(SHOW_IMAGE == 1)
		cvShowImage("Camera",frame);
	return 0;
}
void fish_motor(CvPoint objPos){
/*	printf("ob-x %d ob-y %d r-x %d r-y %d y-w %d r-h %d\n",
		objPos.x, objPos.y, g_range.x, g_range.y,
		g_range.width, g_range.height);
*/
	if(objPos.x == 0 && objPos.y == 0){
		return;
	}
	if(objPos.x - g_range.x < 3*g_range.width/10){
		ctlMotor('r');
	}	
	else if(objPos.x - g_range.x > 7*g_range.width/10){
		ctlMotor('l');
	}	
	if(objPos.y - g_range.y < 3*g_range.height/10){
		ctlMotor('d');
	}	
	else if(objPos.y - g_range.y > 7*g_range.height/10){
		ctlMotor('u');
	}	
}
CvMoments mom = {0};
CvPoint fishPos = {0,0};
IplImage* frame;
IplImage* cl_frame;
IplImage* cl_frame_temp;
IplImage* gr_frame;
CvSize    frameSize ;

int init_fish(){
	//if(SHOW_IMAGE == 1)
	cvNamedWindow( "set_HSV", CV_WINDOW_NORMAL);
	cvNamedWindow( "Camera", CV_WINDOW_NORMAL);
	set_mouse_bar("Camera");
	g_capture = cvCreateCameraCapture( 0 );
	if(g_capture == NULL){
		printf("no camra in comp\n");
		exit(0);
	}
	frame		= cvQueryFrame( g_capture );
	cvCvtColor(frame,frame,CV_RGB2HSV);
	cl_frame	= cvCloneImage(frame);
	cl_frame_temp	= cvCloneImage(frame);
	frameSize	= cvGetSize(frame);
	gr_frame	= cvCreateImage(frameSize,IPL_DEPTH_8U,1);
//	set_trac_bar("set_HSV");
	onTrackbarSlide(0);
	g_msPrm.isDrawing = 0;

	g_msPrm.image	= cl_frame;
	g_msPrm.box	= cvRect( 0, 0, 1, 1 );
	mouse("Camera",&g_msPrm);
	return 0;


}
	

int mod = 0;
void get_fish_pos(){
	mod++;
	if(g_msPrm.isDrawing){
		cvCopy(frame,cl_frame,NULL);
		switch(g_mouse){
		case AVG_POS:
			set_avg_pos(cl_frame);
		break;
		case RANGE_POS:
			set_range_pos(cl_frame);
		break;

		default:
		break;
		}
	}
	else{

		frame = cvQueryFrame( g_capture );
//			if(!(++mod&0x3))
//			cvShowImage("Camera",frame);
#if 1
//		if(g_msPrm.box.width != 0){
//			draw_box(frame,g_msPrm.box);
//		}
//		else{
//		}
		cvCircle(frame,
			 fishPos,
			 sqrt(mom.m00)/1,
			 cvScalar(0x00,0x00,0x00,0),1,8,0
			 );
		//if(!(++mod&0x3))
		if(SHOW_IMAGE == 1)
			cvShowImage("Camera",frame);
		if(g_is_range){
			cvSetImageROI(frame,g_range);
			cvSetZero(gr_frame);
			cvSetImageROI(gr_frame,g_range);
		}

		cvSmooth( frame, frame, CV_GAUSSIAN, 3, 3 ,0,0);
		//cvCvtColor(frame,frame,CV_RGB2HSV);
		cvInRangeS(frame,g_hsv_min,g_hsv_max,gr_frame);

		if(g_is_range){
			cvSetImageROI(frame,g_range);
		}
		cvErode(gr_frame,gr_frame,NULL,2);
		cvDilate(gr_frame,gr_frame,NULL,2);
		if(g_is_range){
			cvResetImageROI(frame);
			cvResetImageROI(gr_frame);
		}
		cvMoments(gr_frame,&mom,1);
		fishPos.x = (mom.m10/mom.m00);
		fishPos.y = (mom.m01/mom.m00);

		if(SHOW_IMAGE == 1)
			cvShowImage( "set_HSV", gr_frame );
//			cvErode(gr_frame,gr_frame,NULL,10);
#endif
	}

}
void exit_fish(){
	cvReleaseImage( &cl_frame );
	cvReleaseImage( &cl_frame_temp );
	cvReleaseCapture( &g_capture );
	if(SHOW_IMAGE == 1)
		cvDestroyWindow( "set_HSV" );
	cvDestroyWindow( "Camera" );

}
int main( int argc, char** argv ) {
	char c;
	g_range  = cvRect(111,68,470,364);
	init_motor(frameSize.width,frameSize.height);
	init_fish();	
	//printf("hacked frames %d w %d h %d\n",frames,tmpw,tmph);

	begin = clock();
#if 1
	while(1) {
		get_fish_pos();
		if(SHOW_IMAGE == 1)	
			cvShowImage( "set_HSV", gr_frame );
		fish_motor(fishPos);
		c = (char)cvWaitKey(50);
		if( c == 27 ) break;
		if( c == 'c'){
			while(1){
				c = getchar();
				if(ctlMotor(c)<0) break;
				
			}
		}
	}
#endif
	end = clock();
	time_spent = (double)(end - begin)/CLOCKS_PER_SEC;
	time_spent += 1;
	printf("time of prog %f, numframe %d,numframe for sec %d\n",
		time_spent,mod,mod/(int)time_spent);
	exit_fish();
	exit_motors();
	return(0);
}
