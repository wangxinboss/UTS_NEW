#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
	namespace Algorithm
	{
		namespace SobelFocus
		{
			void Gradient(BYTE *image_in,int _width,int _Height, int cx[9], int cy[9], double &GValue)
			{
				int		d[9];
				int		i, j/*, dat*/;
				double	xx, yy, zz;
				double  total_value = 0;

				for (i = 1; i < _Height-1; i++) {
					for (j = 1; j < _width-1; j++) {
						d[0] = image_in[(i-1)*_width + (j-1)];
						d[1] = image_in[(i-1)*_width + (j)  ];
						d[2] = image_in[(i-1)*_width + (j+1)];
						d[3] = image_in[(i  )*_width + (j-1)];
						d[4] = image_in[(i  )*_width + (j)  ];
						d[5] = image_in[(i  )*_width + (j+1)];
						d[6] = image_in[(i+1)*_width + (j-1)];
						d[7] = image_in[(i+1)*_width + (j)  ];
						d[8] = image_in[(i+1)*_width + (j+1)];

						xx = (double)abs(cx[0]*d[0] + cx[1]*d[1] + cx[2]*d[2]
						                             + cx[3]*d[3] + cx[4]*d[4] + cx[5]*d[5]
						                             + cx[6]*d[6] + cx[7]*d[7] + cx[8]*d[8]);

						yy = (double)abs(cy[0]*d[0] + cy[1]*d[1] + cy[2]*d[2]
						                             + cy[3]*d[3] + cy[4]*d[4] + cy[5]*d[5]
						                             + cy[6]*d[6] + cy[7]*d[7] + cy[8]*d[8]);

						//zz = (double)(sqrt(xx*xx+yy*yy));
						zz = (double)(xx+yy);
						total_value += zz;
						// 			dat = (int)zz;
						// 			
						// 			if(dat > 255) dat = 255;
						// 			else if(dat < 0) dat = 0;
						// 
						// 			image_out[i*_width +j] = (BYTE)dat;
					}
				}
				GValue = total_value;
			}

			BOOL CalImageSobel(unsigned char* _YBuffer,int Width,int Height,double &SobelValue)
			{
				bool Res = true;
				int cx_sobel[9] = 	{-1, 0, 1,	
					-2, 0, 2,
					-1, 0, 1 };

				int cy_sobel[9] = 	{ 1, 2, 1,
					0, 0, 0,	
					-1,-2,-1 };	
				Gradient(_YBuffer,Width,Height, cx_sobel, cy_sobel, SobelValue);
				SobelValue /= (Width*Height);

				return Res;
			}

			BOOL CalImageSobelX(unsigned char* _YBuffer,int Width,int Height,double &SobelValue)
			{
				bool Res = true;
				int cx_sobel[9] = 	{-1, 0, 1,	
					-2, 0, 2,
					-1, 0, 1 };

				int cy_sobel[9] = 	{ 0, 0, 0,
					0, 0, 0,	
					0, 0, 0};	
				Gradient(_YBuffer,Width,Height, cx_sobel, cy_sobel, SobelValue);
				SobelValue /= (Width*Height);

				return Res;
			}

		}
	}
	
}