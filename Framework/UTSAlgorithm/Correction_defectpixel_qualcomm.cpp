#include "StdAfx.h"
#include "Algorithm.h"

//#include <Windows.h>
//#include <iostream>

//#include <foundation_public_macro.h>
//#include <foundation_public_debug.h>
//#pragma comment(lib,"foundation.lib")
//-----------------------------------------------------------------------------
namespace UTS
{
	namespace Algorithm
	{
		namespace Correction_PD
		{
			bool GetPDPattern_Qualcomm
				(
				wchar_t* _PatternFilePath,
				int &PDLength,
				DefectPoint* PD_L_POS,
				DefectPoint* PD_R_POS
				)
			{
				bool bResult = true;

				FILE *fp;
				if(0 != _wfopen_s(&fp,_PatternFilePath,L"r"))
				{
					//ERROR
				}
				wchar_t linebufIn[500] = {L"\0"};
				wchar_t	tmp1_cmd[MAX_PATH]={0};
				wchar_t	tmp2_cmd[MAX_PATH]={0};
				wchar_t	tmp3_cmd[MAX_PATH]={0};
				wchar_t	tmp4_cmd[MAX_PATH]={0};
				wchar_t	tmp5_cmd[MAX_PATH]={0};


				bool bStartScan = false;
				bool bEndScan = false;

				//int PDLength = 0;
				// 	PointF* L_POS;
				// 	PointF* R_POS;

				int i=0;
				while (fgetws(linebufIn,500,fp)&&(false == bEndScan))
				{
					//讀一開始（一行五?讀）
					memset(tmp1_cmd,0,sizeof(wchar_t)*MAX_PATH);
					memset(tmp2_cmd,0,sizeof(wchar_t)*MAX_PATH);
					memset(tmp3_cmd,0,sizeof(wchar_t)*MAX_PATH);
					memset(tmp4_cmd,0,sizeof(wchar_t)*MAX_PATH);
					memset(tmp5_cmd,0,sizeof(wchar_t)*MAX_PATH);

					swscanf_s(linebufIn,L"%s %s %s %s %s",tmp1_cmd,_countof(tmp1_cmd),tmp2_cmd,_countof(tmp2_cmd),tmp3_cmd,_countof(tmp3_cmd),tmp4_cmd,_countof(tmp4_cmd),tmp5_cmd,_countof(tmp5_cmd));

					if ((wcscmp(L"",tmp3_cmd)!=0)&&(wcscmp(L"",tmp4_cmd)==0)&&(wcscmp(L"",tmp5_cmd)==0)&&(bStartScan == false))
					{
						if ((PD_L_POS==nullptr)||(PD_R_POS == nullptr))
						{
							PDLength = _wtoi(tmp3_cmd)*_wtoi(tmp1_cmd);
							fclose(fp);
							return true;
						}
						if(PDLength != _wtoi(tmp3_cmd)*_wtoi(tmp1_cmd))
						{
							fclose(fp);
							return false;
						}
						//L_POS = new PointF[PDLength];
						memset(PD_L_POS,0,sizeof(DefectPoint)*PDLength);
						//R_POS = new PointF[PDLength];
						memset(PD_R_POS,0,sizeof(DefectPoint)*PDLength);
						i=0;
						bStartScan =true;
					}

					if ((bStartScan == true)&&(bEndScan != true))
					{
						if (wcscmp(L"",tmp5_cmd)!=0)
						{
							PD_L_POS[i].x =_wtoi(tmp1_cmd);
							PD_L_POS[i].y =_wtoi(tmp2_cmd);
							PD_R_POS[i].x =_wtoi(tmp3_cmd);
							PD_R_POS[i].y =_wtoi(tmp4_cmd);
							i++;
						}
						if (i==PDLength)
						{
							bEndScan = true;
						}
					}
				}
				fclose(fp);
				return true;
			}
			//-------------------------------------------------
			int /*ALGORITHM_API*/ CalculateDPAFDefectTable_Qualcomm
				(
				int _GlobalShift_X,
				int _GlobalShift_Y,
				int _X_Step,
				int _Y_Step,
				int _BlockNumX,
				int _BlockNumY,
				DefectPoint* _POS_L,
				int _PosLCount,
				DefectPoint* _POS_R,
				int _PosRCount,
				DefectPoint* PDAFDefectTable
				)
			{
				if ((nullptr == _POS_R)||(nullptr == _POS_L)||(nullptr == PDAFDefectTable))
				{
					return ((_BlockNumX*_BlockNumY)*(_PosLCount+_PosRCount));
				}
				int Totalcount = 0;
				for (int j=0;j<_BlockNumY;j++)
				{
					for (int i=0;i<_BlockNumX;i++)
					{
						for (int IndexL=0;IndexL<_PosLCount;IndexL++)
						{
							PDAFDefectTable[Totalcount].x = _GlobalShift_X+_POS_L[IndexL].x+_X_Step*i;
							PDAFDefectTable[Totalcount].y = _GlobalShift_Y+_POS_L[IndexL].y+_Y_Step*j;
							Totalcount++;
						}
						for (int IndexR=0;IndexR<_PosRCount;IndexR++)
						{
							PDAFDefectTable[Totalcount].x = _GlobalShift_X+_POS_R[IndexR].x+_X_Step*i;
							PDAFDefectTable[Totalcount].y = _GlobalShift_Y+_POS_R[IndexR].y+_Y_Step*j;
							Totalcount++;
						}
					}
				}
				return Totalcount;
			}
			//---------------------------------------------------------------------
			int /*ALGORITHM_API*/ CalculateDPAFDefectTable_Qualcomm
				(
				wchar_t PatternFileName[255],
				int _GlobalShift_X,
				int _GlobalShift_Y,
				int _X_Step,
				int _Y_Step,
				int _ImageSizeX,
				int _ImageSizeY,			
				DefectPoint* PDAFDefectTable
				)
			{
				int PDLength = 0;
				GetPDPattern_Qualcomm(PatternFileName,PDLength);			

				int _BlockNumX = (_ImageSizeX-_GlobalShift_X)/_X_Step;
				int _BlockNumY = (_ImageSizeY-_GlobalShift_Y)/_Y_Step;			

				int PosLCount = PDLength;
				int PosRCount = PDLength;

				if (nullptr == PDAFDefectTable)
				{
					return ((_BlockNumX*_BlockNumY)*(PosLCount+PosRCount));
				}

				DefectPoint* POS_L = new DefectPoint[PDLength];
				memset(POS_L,0,sizeof(DefectPoint)*PDLength);
				DefectPoint* POS_R = new DefectPoint[PDLength];
				memset(POS_R,0,sizeof(DefectPoint)*PDLength);
				GetPDPattern_Qualcomm(PatternFileName,PDLength,POS_L,POS_R);

				int Totalcount = CalculateDPAFDefectTable_Qualcomm(_GlobalShift_X,_GlobalShift_Y,_X_Step,_Y_Step,_BlockNumX,_BlockNumY,POS_L,PosLCount,POS_R,PosRCount,PDAFDefectTable);

				RELEASE_ARRAY(POS_L);
				RELEASE_ARRAY(POS_R);

				return Totalcount;
			}
		}
	}
}