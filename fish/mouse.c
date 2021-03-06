#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include "mouse.h"
// Define our callback which we will install for
// mouse events.
//
void my_mouse_callback(
		       int event, int x, int y, int flags, void* param
		      );
void moti_mouse_callback(
			 int event, int x, int y, int flags, void* param
			);

CvRect box;
int drawing_box = 1;//false;

// A litte subroutine to draw a box onto an image
//
void draw_box( IplImage* img, CvRect box ) {
	cvRectangle (
		     img,
		     cvPoint(box.x,box.y),
		     cvPoint(box.x+box.width,box.y+box.height),
		     cvScalar(0xff,0x00,0x00,0),1,8,0    /* red */
		    );
}
int mouse(const char *win_name,MouseParams* mouseParams){
	printf("win_name - %s\n",win_name);
	//	CvRect* box = (CvRect *)malloc(sizeof(CvRect));
	cvSetMouseCallback(
			   win_name,
			   moti_mouse_callback,
			   mouseParams
			  );
	return 0;
}
#if 0
int main( int argc, char* argv[] ) {

	box = cvRect(-1,-1,0,0);
	IplImage* image = cvCreateImage(
					cvSize(200,200),
					IPL_DEPTH_8U,
					3
				       );
	cvZero( image );
	MouseParams msPrm = {
		.image = image,
		.box   = box,
		.isDrawing = false
	};
	IplImage* temp = cvCloneImage( image );

	cvNamedWindow( "Box Example" );

	// Here is the crucial moment that we actually install
	// the callback.  Note that we set the value ‘param’ to
	// be the image we are working with so that the callback
	// will have the image to edit.
	//
	/*
	   cvSetMouseCallback(
	   "Box Example",
	   my_mouse_callback,
	   (void*) image
	   );
	 */
	mouse("Box Example",&msPrm);

	// The main program loop.  Here we copy the working image
	// to the ‘temp’ image, and if the user is drawing, then
	// put the currently contemplated box onto that temp image.
	// display the temp image, and wait 15ms for a keystroke,
	// then repeat…
	//
	while( 1 ) {

		cvCopyImage( image, temp );
		if( msPrm.isDrawing ){
			//printf("is Drawing\n");
			draw_box( temp, msPrm.box );
		};
		cvShowImage( "Box Example", temp );

		if( cvWaitKey( 15 )==27 ) break;
	}

	// Be tidy
	//
	cvReleaseImage( &image );
	cvReleaseImage( &temp );
	cvDestroyWindow( "Box Example" );
}
#endif
void moti_mouse_callback(int event, int x, int y, int flags, void* param )
{

	MouseParams*  msPrm = (MouseParams*) param;
	int width_temp  = 0;
	int height_temp = 0;
	switch( event ) {
	case CV_EVENT_RBUTTONDOWN: {
		msPrm->isDrawing = 0;//false;
		msPrm->box = cvRect( x, y, 0, 0 );
	}
		break;
	case CV_EVENT_MOUSEMOVE: {
		if( msPrm->isDrawing ) {
			width_temp  = x - msPrm->start_x;
			height_temp = y - msPrm->start_y;
			//printf("mw %d, mh %d\n",width_temp,height_temp);
			//printf("mx %d, my %d\n",x,y);
			if(width_temp < 0  ) {
				msPrm->box.x    = x;
				msPrm->box.width = -width_temp;
			}
			else{
				msPrm->box.x     = msPrm->start_x;
				msPrm->box.width = width_temp;
			}

			if(height_temp<0 ) {
				msPrm->box.y      = y;
				msPrm->box.height = -height_temp ;
			}
			else{
				msPrm->box.y	  = msPrm->start_y;
				msPrm->box.height = height_temp;
			}
		}
	}
		break;
	case CV_EVENT_LBUTTONDOWN: {
		msPrm->isDrawing = 1;//true;
		//printf("BD\n");
		msPrm->start_x = x;
		msPrm->start_y = y;
		msPrm->box = cvRect( x, y, 0, 0 );
	}
		break;
	case CV_EVENT_LBUTTONUP: {
		msPrm->isDrawing = 0;//false;
		//printf("BU\n");

//		draw_box( msPrm->image,msPrm->box);
	}
		break;
	}
}
// This is our mouse callback.  If the user
// presses the left button, we start a box.
// when the user releases that button, then we
// add the box to the current image.  When the
// mouse is dragged (with the button down) we
// resize the box.
//
void my_mouse_callback(
		       int event, int x, int y, int flags, void* param )
{

	IplImage* image = (IplImage*) param;

	switch( event ) {
	case CV_EVENT_MOUSEMOVE: {
		if( drawing_box ) {
			box.width  = x-box.x;
			box.height = y-box.y;
		}
	}
		break;
	case CV_EVENT_LBUTTONDOWN: {
		drawing_box = 1;//true;
		box = cvRect( x, y, 0, 0 );
	}
		break;
	case CV_EVENT_LBUTTONUP: {
		drawing_box = 0;//false;
		if( box.width < 0  ) {
			box.x+=box.width;
			box.width *=-1;
		}
		if( box.height < 0 ) {
			box.y+=box.height;
			box.height *= -1;
		}
		draw_box( image, box );
	}
		break;
	}
}
