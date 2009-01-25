/*
 *  of1394VideoGrabber.cpp
 *  openFrameworks
 *
 *  Created by Zach Gage on 8/8/08.
 *  Copyright 2008 stfj. All rights reserved.
 *
 */

#include "of1394VideoGrabber.h"


//--------------------------------------------------------------------
of1394VideoGrabber::of1394VideoGrabber()
{	
	// common
	bIsFrameNew				= false;
	bVerbose 				= true;
	bGrabberInited 			= false;
	bUseTexture				= true;
	bChooseDevice			= false;
	deviceID				= 0;
	width 					= 640;	// default setting
	height 					= 480;	// default setting

	//frameCt =0;
	//oldFrameCt=0;

	camera=NULL;
	
	failedToInit = true;
}


//--------------------------------------------------------------------
of1394VideoGrabber::~of1394VideoGrabber()
{
	if(!failedToInit) // if the init failed, then trying to do this stuff is a very bad idea
	{		
		stopThread();
		ofSleepMillis(200);
		dc1394_video_set_transmission(camera, DC1394_OFF);
		dc1394_capture_stop(camera);
		dc1394_camera_free(camera);
		dc1394_free (d);
	}
}


//--------------------------------------------------------------------
void of1394VideoGrabber::listDevices()
{
	//nothing to see here, move along.
}

//--------------------------------------------------------------------
void of1394VideoGrabber::setVerbose(bool bTalkToMe)
{
	bVerbose = bTalkToMe;
}


//--------------------------------------------------------------------
void of1394VideoGrabber::setDeviceID(int _deviceID)
{
	deviceID		= _deviceID;
	bChooseDevice	= true;
}

//---------------------------------------------------------------------------
unsigned char * of1394VideoGrabber::getPixels()
{
	if( lock() )
	{
		return pixels;
	}
	else
	{
		return NULL;
	}
}

//---------------------------------------------------------------------------
bool  of1394VideoGrabber::isFrameNew()
{
	bool temp=false;
	
		dc1394_avt_get_frame_info(camera, &frameCt);
		if(frameCt != oldFrameCt);
		{
		   oldFrameCt=frameCt;
			
		   temp= true;
		}
	
	   return temp;

}

//--------------------------------------------------------------------
void of1394VideoGrabber::grabFrame()
{	
	
}


//--------------------------------------------------------------------
void of1394VideoGrabber::videoSettings(void)
{

}

void cleanup_and_exit(dc1394camera_t *camera)
{
    dc1394_video_set_transmission(camera, DC1394_OFF);
    dc1394_capture_stop(camera);
    dc1394_camera_free(camera);
    exit(1);
}

//--------------------------------------------------------------------
int of1394VideoGrabber::initGrabber()
{
	// moved from constructor
	int i;
	
	printf("Making new camera\n");
	
	d = dc1394_new ();
	
	err=dc1394_camera_enumerate (d, &list);
    DC1394_ERR_RTN(err,"Failed to enumerate cameras");
	
	if (list->num == 0) {
		if(err!=0 && bVerbose)printf("\n No cameras found\n ");
		return 1;
	}
	
	camera = dc1394_camera_new (d, list->ids[0].guid);
	if (!camera) {
		if(err!=0 && bVerbose)printf("\n Failed to initialize camera with guid %PRIx64", list->ids[0].guid);
		return 1;
	}
	dc1394_camera_free_list (list);
	
	//if(err!=0 && bVerbose)printf("Using camera with GUID %PRIx64\n", camera->guid);
	
	/*-----------------------------------------------------------------------
	 *  get the best video mode and highest framerate. This can be skipped
	 *  if you already know which mode/framerate you want...
	 *-----------------------------------------------------------------------*/
	// get video modes:
	// no point for modes now...
	
	//err=dc1394_video_get_supported_modes(camera,&video_modes);
	//if(err!=0 && bVerbose)printf("\n Can't get video modes\n ");
	
	//printf("Listing Modes\n");
	//for(i = 0; i < video_modes.num; i++ )  { dc1394video_mode_t mode = video_modes.modes[i]; print_mode_info(mode ); } 		   
	
	
	/*-----------------------------------------------------------------------
	 *  setup capture
	 *-----------------------------------------------------------------------*/
	
	video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
	framerate = DC1394_FRAMERATE_30;
	
	err=dc1394_video_set_iso_speed(camera, DC1394_ISO_SPEED_200);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set iso speed");
	
	err=dc1394_video_set_mode(camera, video_mode);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set video mode\n");

	err=dc1394_video_set_framerate(camera, framerate);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set framerate\n");
	
	dc1394_feature_set_mode(camera, DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(camera, DC1394_FEATURE_GAMMA, DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(camera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(camera, DC1394_FEATURE_IRIS, DC1394_FEATURE_MODE_MANUAL);

/*dc1394_video_set_mode(camera, DC1394_VIDEO_MODE_FORMAT7_0);
    err = dc1394_format7_set_roi(camera, DC1394_VIDEO_MODE_FORMAT7_0,
                                 DC1394_COLOR_CODING_RGB8,
                                 DC1394_USE_MAX_AVAIL, // use max packet size
                                 0, 0, // left, top
                                 720, 480);  // width, height
    DC1394_ERR_RTN(err,"Unable to set Format7 mode 0.\nEdit the example file manually to fit your camera capabilities");
*/
	err=dc1394_capture_setup(camera,4, DC1394_CAPTURE_FLAGS_DEFAULT);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera\n");
	
	unsigned int min_bytes, max_bytes;
    unsigned int actual_bytes;
    uint64_t total_bytes = 0;


	
    DC1394_ERR_RTN(err,"Packet para inq error");
    printf( "camera reports allowed packet size from %d - %d bytes\n", min_bytes, max_bytes);
	
  /*  err=dc1394_format7_get_packet_size(camera, DC1394_VIDEO_MODE_FORMAT7_0, &actual_bytes);
    DC1394_ERR_RTN(err,"dc1394_format7_get_packet_size error");
    printf( "camera reports actual packet size = %d bytes\n", actual_bytes);
	
    err=dc1394_format7_get_total_bytes(camera, DC1394_VIDEO_MODE_FORMAT7_0, &total_bytes);
    DC1394_ERR_RTN(err,"dc1394_query_format7_total_bytes error");
    printf( "camera reports total bytes per frame = %d bytes\n",  total_bytes);
	*/
	/*-----------------------------------------------------------------------
	 *  report camera's features
	 *-----------------------------------------------------------------------*/
	err=dc1394_feature_get_all(camera,&features);
	if (err!=DC1394_SUCCESS) {
		dc1394_log_warning("Could not get feature set\n ");
	}
	else if(bVerbose) {
		dc1394_feature_print_all(&features, stdout);
	}
	
	/*-----------------------------------------------------------------------
	 *  have the camera start sending us data
	 *-----------------------------------------------------------------------*/
	err=dc1394_video_set_transmission(camera, DC1394_ON);
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not start camera iso transmission\n");

	dc1394_get_image_size_from_video_mode(camera, video_mode, &width, &height);
	if(err != DC1394_SUCCESS){
		cout<<"get image size error: "<<err<<endl;
	}
	printf("Size from video mode = %i wide %i high\n", width, height);

	pixels	= new unsigned char[width*height*3]; // x3 for RGB
	pixels2	= new unsigned char[width*height*3]; // x3 for RGB

	tex.allocate(width,height,GL_LUMINANCE);

	err = dc1394_capture_dequeue( camera, DC1394_CAPTURE_POLICY_WAIT, &frame ) ;
    DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not capture a frame\n");

	memcpy( pixels, frame->image, width * height*3) ;
	
	dc1394_capture_enqueue( camera, frame ) ;
	
	ofSleepMillis(100);
	startThread(true, false);
	
	failedToInit = false;
	return 0;
}

dc1394error_t of1394VideoGrabber::getBayerTile()
{
	uint32_t value;
	dc1394error_t err;
	
	// query register 0x1040
	// This register is an advanced PGR register called BAYER_TILE_MAPPING
	// For more information check the PGR IEEE-1394 Digital Camera Register Reference
	// err = GetCameraControlRegister( camera, 0x1040, &value );
	err = dc1394_get_register( camera, 0x1040, &value );
	
	if ( err != DC1394_SUCCESS )
	{
		printf("ERROR ON BAYER \n");
		return err;
	}
	printf("NO ERROR ON BAYER \n");
	
	
	// Ascii R = 52 G = 47 B = 42 Y = 59
	switch( value )
	{
		default:
		case 0x59595959:	// YYYY
			// no bayer
			bayer_pattern = (dc1394color_filter_t) 0;
			break;
		case 0x52474742:	// RGGB
			bayer_pattern = DC1394_COLOR_FILTER_RGGB;
			break;
		case 0x47425247:	// GBRG
			bayer_pattern = DC1394_COLOR_FILTER_GBRG;
			break;
		case 0x47524247:	// GRBG
			bayer_pattern = DC1394_COLOR_FILTER_GRBG;
			break;
		case 0x42474752:	// BGGR
			bayer_pattern = DC1394_COLOR_FILTER_BGGR;
			break;
	}
	
	return err;
}

/*-----------------------------------------------------------------------
 *  Prints the type of format to standard out
 *-----------------------------------------------------------------------*/
void of1394VideoGrabber::print_format( uint32_t format )
{
#define print_case(A) case A: printf(#A ""); break;
	
    switch( format ) {
			print_case(DC1394_VIDEO_MODE_160x120_YUV444);
			print_case(DC1394_VIDEO_MODE_320x240_YUV422);
			print_case(DC1394_VIDEO_MODE_640x480_YUV411);
			print_case(DC1394_VIDEO_MODE_640x480_YUV422);
			print_case(DC1394_VIDEO_MODE_640x480_RGB8);
			print_case(DC1394_VIDEO_MODE_640x480_MONO8);
			print_case(DC1394_VIDEO_MODE_640x480_MONO16);
			print_case(DC1394_VIDEO_MODE_800x600_YUV422);
			print_case(DC1394_VIDEO_MODE_800x600_RGB8);
			print_case(DC1394_VIDEO_MODE_800x600_MONO8);
			print_case(DC1394_VIDEO_MODE_1024x768_YUV422);
			print_case(DC1394_VIDEO_MODE_1024x768_RGB8);
			print_case(DC1394_VIDEO_MODE_1024x768_MONO8);
			print_case(DC1394_VIDEO_MODE_800x600_MONO16);
			print_case(DC1394_VIDEO_MODE_1024x768_MONO16);
			print_case(DC1394_VIDEO_MODE_1280x960_YUV422);
			print_case(DC1394_VIDEO_MODE_1280x960_RGB8);
			print_case(DC1394_VIDEO_MODE_1280x960_MONO8);
			print_case(DC1394_VIDEO_MODE_1600x1200_YUV422);
			print_case(DC1394_VIDEO_MODE_1600x1200_RGB8);
			print_case(DC1394_VIDEO_MODE_1600x1200_MONO8);
			print_case(DC1394_VIDEO_MODE_1280x960_MONO16);
			print_case(DC1394_VIDEO_MODE_1600x1200_MONO16);
			print_case(DC1394_VIDEO_MODE_EXIF);
			print_case(DC1394_VIDEO_MODE_FORMAT7_0);
			print_case(DC1394_VIDEO_MODE_FORMAT7_1);
			print_case(DC1394_VIDEO_MODE_FORMAT7_2);
			print_case(DC1394_VIDEO_MODE_FORMAT7_3);
			print_case(DC1394_VIDEO_MODE_FORMAT7_4);
			print_case(DC1394_VIDEO_MODE_FORMAT7_5);
			print_case(DC1394_VIDEO_MODE_FORMAT7_6);
			print_case(DC1394_VIDEO_MODE_FORMAT7_7);
		default:
			dc1394_log_error("Unknown format\n");
			//exit(1);
    }
	
}

/*-----------------------------------------------------------------------
 *  Returns the number of pixels in the image based upon the format
 *-----------------------------------------------------------------------*/
uint32_t of1394VideoGrabber::get_num_pixels(uint32_t format ) {
    uint32_t w,h;
	
    dc1394_get_image_size_from_video_mode(camera, format,&w,&h);
	
    return w*h;
}

/*-----------------------------------------------------------------------
 *  Prints the type of color encoding
 *-----------------------------------------------------------------------*/
void of1394VideoGrabber::print_color_coding( uint32_t color_id )
{
    switch( color_id ) {
		case DC1394_COLOR_CODING_MONO8:
			printf("MONO8");
			break;
		case DC1394_COLOR_CODING_YUV411:
			printf("YUV411");
			break;
		case DC1394_COLOR_CODING_YUV422:
			printf("YUV422");
			break;
		case DC1394_COLOR_CODING_YUV444:
			printf("YUV444");
			break;
		case DC1394_COLOR_CODING_RGB8:
			printf("RGB8");
			break;
		case DC1394_COLOR_CODING_MONO16:
			printf("MONO16");
			break;
		case DC1394_COLOR_CODING_RGB16:
			printf("RGB16");
			break;
		case DC1394_COLOR_CODING_MONO16S:
			printf("MONO16S");
			break;
		case DC1394_COLOR_CODING_RGB16S:
			printf("RGB16S");
			break;
		case DC1394_COLOR_CODING_RAW8:
			printf("RAW8");
			break;
		case DC1394_COLOR_CODING_RAW16:
			printf("RAW16");
			break;
		default:
			dc1394_log_error("Unknown color coding = %d\n",color_id);
			exit(1);
    }
}

/*-----------------------------------------------------------------------
 *  Prints various information about the mode the camera is in
 *-----------------------------------------------------------------------*/
void of1394VideoGrabber::print_mode_info(dc1394video_mode_t mode )
{
    int j;
	
    printf("Mode: ");
    print_format(mode);
    printf("\n");
	
    dc1394framerates_t framerates;
    dc1394error_t err;
    err=dc1394_video_get_supported_framerates(camera,mode,&framerates);
    DC1394_ERR(err,"Could not get frame rates");
	
    printf("Frame Rates:\n");
    for( j = 0; j < framerates.num; j++ ) {
        dc1394framerate_t rate = framerates.framerates[j];
        float f_rate;
        dc1394_framerate_as_float(rate,&f_rate);
        printf("  [%d] rate = %f\n",j,f_rate );
    }
	
}
