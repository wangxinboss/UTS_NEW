/*=======================================================================
		Accuracy Test Sample code for LC898123F40
                                             	by Rex.Tang
                                                2016.03.27
========================================================================*/
#include "StdAfx.h"
#include	"math.h"
#include	"OisLc898123F40.h"
#include	"Ois.h"
#include "UTSDefine.h"
#include "OnSemi_LC898123.h"

//****************************************************
//	CUSTOMER NECESSARY CREATING LIST
//****************************************************
/* for I2C communication */ 
// extern	void RamWrite32A(int addr, int data);
// extern 	void RamRead32A( unsigned short addr, void * data );
// extern void	WitTim( unsigned short	UsWitTim );

/* Raw data buffers */	
//Dual_Axis_t xy_raw_data[360/DEGSTEP + 1];
//Dual_Axis_t xy_raw_data[360/3 + 1];
//float xMaxAcc, yMaxAcc;
//float xLimit, yLimit;

#define		LOOPTIME	3							// Read times at each step

// Constants



union	FLTVAL2 {
	float			SfFltVal ;
	struct {
		unsigned char UcDataHH;
		unsigned char UcDataHL;
		unsigned char UcDataLH;
		unsigned char UcDataLL;
	} StFltVal ;
} ;

typedef union FLTVAL2	UnFltVal2 ;

typedef struct tag_Dual_Axis
{
	float xpos;
	float xhall;
	float ypos;
	float yhall;
}Dual_Axis_t;

Dual_Axis_t	xy_raw_data[360/3 + 1];
float		xMaxAcc, yMaxAcc;
float		xLimit, yLimit;

static float fix2float(unsigned int fix)
{
    if((fix & 0x80000000) > 0)
    {
        return ((float)fix-(float)0x100000000)/(float)0x7FFFFFFF;
    } else {
        return (float)fix/(float)0x7FFFFFFF;
    }
}

static unsigned int float2fix(float f)
{
    if(f < 0)
    {
        return (unsigned int)(f * (float)0x7FFFFFFF + 0x100000000);
    } else {
        return (unsigned int)(f * (float)0x7FFFFFFF);
    }
}

/*-------------------------------------------------------------------
	Function Name: Accuracy
	Param:	none
	Return:	value = 0 (no NG point)
			value > 0 (High byte: X total NG points;
					   Low byte: Y total NG points)	
--------------------------------------------------------------------*/
//unsigned short Accuracy()
unsigned short Accuracy(float ACCURACY, unsigned short RADIUS, unsigned short DEGSTEP, unsigned short WAIT_MSEC1, unsigned short WAIT_MSEC2, unsigned short WAIT_MSEC3)
{
	float xpos, ypos;
	unsigned int xhall_value, yhall_value;
	float xMaxHall, yMaxHall;
    unsigned short xng = 0, yng = 0;
    unsigned short deg;
    float xRadius, yRadius;
	unsigned int xGyrogain, yGyrogain;
    unsigned int xGLenz, yGLenz;

	// Get Gyro gain
	RamRead32A(GyroFilterTableX_gxzoom, &xGyrogain);
	RamRead32A(GyroFilterTableY_gyzoom, &yGyrogain);

	// Get Lenz
	RamRead32A(GyroFilterTableX_gxlenz, &xGLenz);
	RamRead32A(GyroFilterTableY_gylenz, &yGLenz);

	// Calculate Radius (117um)
	xRadius = 0.242970F * fabsf(fix2float(xGyrogain)) * fabsf(fix2float(xGLenz)) * 4;
	yRadius = 0.242970F * fabsf(fix2float(yGyrogain)) * fabsf(fix2float(yGLenz)) * 4;

	// Calculate Limit
	xLimit = ACCURACY / 117 * xRadius;
    yLimit = ACCURACY / 117 * yRadius;

	// Radius change (by RADIUS value)
	xRadius = xRadius * RADIUS / 117;
	yRadius = yRadius * RADIUS / 117;

	xMaxAcc = 0;
	yMaxAcc = 0;

	// Circle check
	xpos = xRadius * float(cos(0.0));
	ypos = yRadius * float(sin(0.0));
	RamWrite32A(HALL_RAM_HXOFF1, float2fix(xpos));
	RamWrite32A(HALL_RAM_HYOFF1, float2fix(ypos));
	WitTim(WAIT_MSEC1);

	for( deg = 0; deg <= 360; deg += DEGSTEP ) // 0-360 degree
	{
		xpos = xRadius * float(cos(deg * PI/180));
		ypos = yRadius * float(sin(deg * PI/180));
    	RamWrite32A(HALL_RAM_HXOFF1, float2fix(xpos));
		RamWrite32A(HALL_RAM_HYOFF1, float2fix(ypos));

		xMaxHall = 0;
		yMaxHall = 0;
		WitTim(WAIT_MSEC2);
		
		for(short i=0; i<LOOPTIME; i++)
		{
			WitTim(WAIT_MSEC3);
			RamRead32A( HALL_RAM_HXOUT0, &xhall_value );
			RamRead32A( HALL_RAM_HYOUT0, &yhall_value );
			if(fabsf(fix2float(xhall_value) - xpos) > fabsf(xMaxHall))	
				xMaxHall = fix2float(xhall_value) - xpos;
			if(fabsf(fix2float(yhall_value) - ypos) > fabsf(yMaxHall))	
				yMaxHall = fix2float(yhall_value) - ypos;
		}

		if(fabsf(xMaxHall) > xMaxAcc)	xMaxAcc = fabsf(xMaxHall);
		if(fabsf(yMaxHall) > yMaxAcc)	yMaxAcc = fabsf(yMaxHall);
		
        // Save raw data
		xy_raw_data[deg/DEGSTEP].xpos = xpos;
		xy_raw_data[deg/DEGSTEP].xhall = xMaxHall + xpos;
		xy_raw_data[deg/DEGSTEP].ypos = ypos;
		xy_raw_data[deg/DEGSTEP].yhall = yMaxHall + ypos;
		
		if(fabsf(xMaxHall) > xLimit)	xng++; 	// Have NG point;
		if(fabsf(yMaxHall) > yLimit)	yng++; 	// Have NG point; 

	}
	RamWrite32A(HALL_RAM_HXOFF1, 0); // x = center
	RamWrite32A(HALL_RAM_HYOFF1, 0); // y = center

	return (xng << 8) | yng;
}

//unsigned short HallCheck(void)
unsigned short HallCheck_ON123(float ACCURACY, unsigned short RADIUS, unsigned short DEGSTEP, unsigned short WAIT_MSEC1, unsigned short WAIT_MSEC2, unsigned short WAIT_MSEC3,
	                     float *fxMaxAcc,float *fyMaxAcc,float *fxLimit,float *fyLimit,void *fxy_raw_data)
{
//	short i;
//	unsigned short ret = Accuracy();
	unsigned short ret = Accuracy(ACCURACY, RADIUS, DEGSTEP, WAIT_MSEC1, WAIT_MSEC2, WAIT_MSEC3);

	if(ret)
	{
    	TRACE("\n VCM has NG points: X = %d, Y = %d", ret >> 8, ret & 0xff);
	} else {
    	TRACE("\n VCM is good!");
	}
	
	/*
	// Max Accuracy
	//TRACE("\n X Max Accuracy = %f, Y Max Accuracy = %f", xMaxAcc, yMaxAcc);
	TRACE("\n X Max Accuracy = %d, Y Max Accuracy = %d", (int)(xMaxAcc*1000), (int)(yMaxAcc*1000));

	// Limit vale
	//TRACE("\n xLimit = %f, yLimit = %f", xLimit, yLimit);
	TRACE("\n xLimit = %d, yLimit = %d", (int)(xLimit*1000), (int)(yLimit*1000));

	// Circle
	for(i=0; i<=(360/DEGSTEP); i++)
	{
		//TRACE("\n xPos = %f, xHall = %f, yPos = %f, yHall = %f", xy_raw_data[i].xpos, xy_raw_data[i].xhall, xy_raw_data[i].ypos, xy_raw_data[i].yhall);
		TRACE("\n xPos = %d, xHall = %d, yPos = %d, yHall = %d", (int)(xy_raw_data[i].xpos*1000), (int)(xy_raw_data[i].xhall*1000), (int)(xy_raw_data[i].ypos*1000), (int)(xy_raw_data[i].yhall*1000));
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	Dual_Axis_t	*buf =  (Dual_Axis_t*)fxy_raw_data;

	memcpy(buf,xy_raw_data,sizeof(Dual_Axis_t)*121);
	*fxMaxAcc = xMaxAcc*1000;
	*fyMaxAcc = yMaxAcc*1000;
	*fxLimit  = xLimit*1000;
	*fyLimit  = yLimit*1000;

	//////////////////////////////////////////////////////////////////////////
	return( ret );
}
