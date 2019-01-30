#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "CommonFunc.h"
#include "BaseOperator.h"
#include "BaseOtp.h"


struct SobelParam
{
	int sta, end, step;
	::ROI roi;
	int down_step_cnt;
	double down_sobel_th;
};
struct SobelResult
{
	int focus_point;
	double focus_sobel;
};

int MTFFocus(BaseDevice *dev,SobelParam *param,int Width,int Height,SobelResult *result);