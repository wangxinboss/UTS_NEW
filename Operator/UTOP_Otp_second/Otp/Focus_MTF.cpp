#include "StdAfx.h"
#include "Algorithm.h"
#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include "VCM.h"
#include <direct.h>
#include "Focus_MTF.h"



int MTFFocus(BaseDevice *dev,SobelParam *param,int Width,int Height,SobelResult *result)
{
	using namespace UTS::Algorithm::Image;
	//::ROI *roi = &param->roi;
	RECT MTFrect;

	double sobelmax = 0.0, sobelpre = 0.0, sobel = 0.0;
	int downcnt = 0;
	int i, dacmax = 0;
	TEST_BUFFER bufferObj;
	UI_TEXT text;
	UI_MARK uiMark;
	UI_RECT rc;

	rc.color = text.color = COLOR_BLUE;

	MTFrect.left = param->roi.x;
	MTFrect.right = param->roi.x + param->roi.width;
	MTFrect.top = param->roi.y;
	MTFrect.bottom = param->roi.y + param->roi.height;

	memcpy(&rc.rcPos, &MTFrect, sizeof(RECT));

	text.ptPos = CPoint(MTFrect.left, MTFrect.bottom + 30);

	u8 *bmpYroi = (u8*)malloc(Width*Height);
	for (i = param->sta; i < param->end; i += param->step) {
		DLLSetVCM_Move(dev, uts.info.nVCMType, i);
		Algorithm::Sleep(200);
		dev->Recapture(bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
		CImageProc::GetInstance().Cal_RGBtoYBuffer(
			bufferObj.pBmpBuffer,
			Width,Height,
			bmpYroi);

		Algorithm::MTF::MtfCalc::GetYBlockMTF(bmpYroi, Width,Height, MTFrect, sobel);

		uts.log.Debug(_T("MTF: %.4f DAC:%d"),sobel,i);

		text.strText.Format(_T("%.2lf "),sobel);
		uiMark.vecUIText.push_back(text);
		uiMark.vecUiRect.push_back(rc);

		dev->DisplayImage(bufferObj.pBmpBuffer, &uiMark);
		uiMark.clear();

		if (sobelmax < sobel) {
			sobelmax = sobel;
			dacmax = i;
		}

		if (sobel < sobelpre) downcnt++;
		else downcnt = 0;
		if (downcnt >= param->down_step_cnt) break;
		sobelpre = sobel;

		if (sobelmax - sobel >= param->down_sobel_th) break;
	}

	DLLSetVCM_Move(dev, uts.info.nVCMType, dacmax);
	Algorithm::Sleep(300);
	dev->Recapture(bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame);
	dev->DisplayImage(bufferObj.pBmpBuffer);

	uts.log.Debug(_T("MTFFocus MAXDAC:%d"),dacmax);

	RELEASE_ARRAY(bmpYroi);

	result->focus_point = dacmax;
	result->focus_sobel = sobelmax;

	return dacmax;
}