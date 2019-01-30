#include	"stdafx.h"
#include	"Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace Image
        {
            //-------------------------------------------------------------------------------------------------------------
            //----------------------------------------  CImageProc class   ---------------------------------------------------
            //-------------------------------------------------------------------------------------------------------------
            CImageProc::CImageProc()
            {
                MakeSpaceTableITU();
            }

            CImageProc::~CImageProc()
            {

            }

            CImageProc& CImageProc::GetInstance()
            {
                static CImageProc instance;
                return instance;
            }

            void	CImageProc::MakeSpaceTableITU(void)
            {
                int		Y,Cr,Cb,tmp;
                for (Y=0; Y<256; Y++) {	//Y
                    for (Cr=0; Cr<256; Cr++) { //Cr
                        for (Cb=0; Cb<256; Cb++) { //Cb
                            //B = 1.164(Y-16)+2.018(Cr-128)
                            tmp = (int)(1.164*(Y-16)+2.018*(Cb-128));
                            if (tmp > 255) tmp = 255;
                            if (tmp < 0) tmp = 0;
                            BtblITU[Y][Cb] = tmp;
                            tmp= (int)(1.164*(Y-16)+1.596*(Cr-128));
                            if (tmp > 255) tmp = 255;
                            if (tmp < 0) tmp = 0;
                            RtblITU[Y][Cr] = tmp;
                        }
                    }
                }
            }


            void	CImageProc::RawToBmp(int nColorOrder, BYTE *pRawBuffer, BYTE *pBmpBuffer, int size_x, int size_y)
            { 
                LPBYTE		pImageData, pImageTemp, pImageTemp1;
                BYTE		r, g, b;
                int			i, j;
                BOOL		bInterpolation = TRUE; //FALSE;

                pImageData = pRawBuffer + size_x*(size_y-1);
                //pImageData = pRawBuffer;

                switch (nColorOrder) {
                case 5:  //BlackWhite
                    for (i=0; i<size_y; i+=2) {
                        pImageTemp = pImageData;
                        for (j=0; j<size_x; j+=2) {
                            *pBmpBuffer++ = *pImageTemp;  // B
                            *pBmpBuffer++ = *pImageTemp;  // G
                            *pBmpBuffer++ = *pImageTemp;  // R
                            pImageTemp++; 

                            *pBmpBuffer++ = *pImageTemp;  // B
                            *pBmpBuffer++ = *pImageTemp;  // G
                            *pBmpBuffer++ = *pImageTemp;  // R
                            pImageTemp++; 
                        }

                        pImageData -= size_x;		//v1.1.7  , HVS
                        pImageTemp = pImageData;
                        for (j=0; j<size_x; j+=2) {
                            *pBmpBuffer++ = *pImageTemp;	// B
                            *pBmpBuffer++ = *pImageTemp;	// G
                            *pBmpBuffer++ = *pImageTemp;	// R
                            pImageTemp++;

                            *pBmpBuffer++ = *pImageTemp;	// B
                            *pBmpBuffer++ = *pImageTemp;	// G
                            *pBmpBuffer++ = *pImageTemp;	// R
                            pImageTemp++; 
                        }
                        pImageData -= size_x;		// v1.1.7 , HVS
                    }		
                    break;

                case 4:	//GRBG
                    //GRGR
                    //BGBG
                    //GRGR
                    //BGBG
                    for (i=0; i<size_y-2; i+=2) {
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		
                        for (j=0; j<size_x-2; j+=2) {
                            r = pImageTemp1[1];							
                            b = pImageTemp[0];
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp  += 1;   		
                            pImageTemp1 += 1;							

                            r =	pImageTemp1[0]; 					
                            b =  pImageTemp[1]; 
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;    
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {	//2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;	
                        }
                        pImageData -= size_x;
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		

                        for (j=0; j<size_x-2; j+=2) {
                            b = pImageTemp1[0];
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            r = pImageTemp[1];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;							
                            pImageTemp1 += 1;							

                            r = pImageTemp[0];
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            b = pImageTemp1[1];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1; 
                            pImageTemp1 += 1;

                        }
                        for (j=0; j<2; j++) {   //2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;	
                        }
                        pImageData -= size_x;
                    }
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    break;
                case 2:	//RGGB
                    //RGRG
                    //GBGB
                    //RGRG
                    //GBGB
                    for (i=0; i<size_y-2; i+=2) {
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		
                        for (j=0; j<size_x-2; j+=2) {
                            r = pImageTemp1[0];							
                            b = pImageTemp[1];
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp  += 1;   		
                            pImageTemp1 += 1;							

                            r =	pImageTemp1[1]; 					
                            b =  pImageTemp[0]; 
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;    
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {	//2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                        }
                        pImageData -= size_x;
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		

                        for (j=0; j<size_x-2; j+=2) {
                            b = pImageTemp1[1];
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            r = pImageTemp[0];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;							
                            pImageTemp1 += 1;							

                            r = pImageTemp[1];
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            b = pImageTemp1[0];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1; 
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {   //2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                        }
                        pImageData -= size_x;
                    }
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    break;

                case 1:	//BGGR
                    //BGBG
                    //GRGR
                    //BGBG
                    //GRGR
                    for (i=0; i<size_y-2; i+=2) {
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		
                        for (j=0; j<size_x-2; j+=2) {
                            r = pImageTemp[1];							
                            b = pImageTemp1[0];
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp  += 1;   		
                            pImageTemp1 += 1;							

                            r =	pImageTemp[0]; 					
                            b =  pImageTemp1[1]; 
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;    
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {	//2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                        }
                        pImageData -= size_x;
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		

                        for	(j=0; j<size_x-2; j+=2) {
                            b = pImageTemp[0];
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            r = pImageTemp1[1];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;							
                            pImageTemp1 += 1;							

                            r = pImageTemp1[0];
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            b = pImageTemp[1];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1; 
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {   //2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                        }
                        pImageData -= size_x;
                    }
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    break;
                case 3:	//GBRG
                    //GBGB
                    //RGRG
                    //GBGB
                    //RGRG
                    for (i=0; i<size_y-2; i+=2) {
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		
                        for (j=0; j<size_x-2; j+=2) {
                            r = pImageTemp[0];							
                            b = pImageTemp1[1];
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp  += 1;   		
                            pImageTemp1 += 1;							

                            r =	pImageTemp[1]; 					
                            b =  pImageTemp1[0]; 
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;    
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {	//2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                        }
                        pImageData -= size_x;
                        pImageTemp = pImageData;						
                        pImageTemp1 = pImageData - size_x;		

                        for (j=0; j<size_x-2; j+=2) {
                            b = pImageTemp[1];
                            g = (pImageTemp[0]+pImageTemp1[1]) >>1;
                            r = pImageTemp1[0];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1;							
                            pImageTemp1 += 1;							

                            r = pImageTemp1[1];
                            g = (pImageTemp[1]+pImageTemp1[0]) >>1;
                            b = pImageTemp[0];							  				
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                            pImageTemp += 1; 
                            pImageTemp1 += 1;
                        }
                        for (j=0; j<2; j++) {   //2's dummy
                            *pBmpBuffer++ = b;
                            *pBmpBuffer++ = g;
                            *pBmpBuffer++ = r;
                        }
                        pImageData -= size_x;
                    }
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    for (i=0; i<size_x*3; i++)	*pBmpBuffer++ = *(pBmpBuffer-size_x*3); //ÀÌÀü line data¸¦ copyÇÏ¸é¼­ Ã¤¿ò
                    break;
                }  	
            }


            void	CImageProc::YCbCrITUToBmp(int iMode, BYTE *pYuvBuffer, BYTE *pBmpBuffer, int size_x, int size_y)
            {
                DWORD		 Y0,Y1,Cb,Cr; //speed up!!!
                int			 tmp;
                int			 i,j;
                LPBYTE       pImageData, pImageTemp;

                pImageData = pYuvBuffer + size_x*2*(size_y - 1);

                switch (iMode) { 		 
                case 3: //CbYCrY
                    for (i=0; i<size_y; i++) {
                        pImageTemp = pImageData;
                        for (j=0; j<size_x; j+=2) {
                            Y0 = pImageTemp[1];
                            Cr = pImageTemp[2];
                            Y1 = pImageTemp[3];
                            Cb = pImageTemp[0];
                            *pBmpBuffer++ = BtblITU[Y0][Cb];    //B
                            tmp = (int)(1192*(Y0-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				//G
                            *pBmpBuffer++ = RtblITU[Y0][Cr];	//R

                            *pBmpBuffer++ = BtblITU[Y1][Cb];	 //B
                            tmp = (int)(1192*(Y1-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y1][Cr];	 //R
                            pImageTemp+=4;  //next pair.
                        }
                        pImageData -= size_x*2;
                    }
                    break;
                case 2: //YCrYCb
                    for (i=0; i<size_y; i++) {
                        pImageTemp = pImageData;
                        for (j=0; j<size_x; j+=2) {
                            Y0 = pImageTemp[0];
                            Cr = pImageTemp[1];
                            Y1 = pImageTemp[2];
                            Cb = pImageTemp[3];
                            *pBmpBuffer++ = BtblITU[Y0][Cb];		 //B
                            tmp = (int)(1192*(Y0-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y0][Cr];     //R

                            *pBmpBuffer++ = BtblITU[Y1][Cb];	 //B	
                            tmp = (int)(1192*(Y1-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y1][Cr];     //R
                            pImageTemp+=4;	//next  pair.
                        }
                        pImageData -= size_x*2;
                    }
                    break;
                case 1: //YCbYCr
                    for (i=0; i<size_y; i++) {
                        pImageTemp = pImageData;
                        for (j=0; j<size_x; j+=2) {
                            Y0 = pImageTemp[0];
                            Cr = pImageTemp[3];
                            Y1 = pImageTemp[2];
                            Cb = pImageTemp[1];
                            *pBmpBuffer++ = BtblITU[Y0][Cb];		 //B
                            tmp = (int)(1192*(Y0-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y0][Cr];	 //R

                            *pBmpBuffer++ = BtblITU[Y1][Cb];	 //B
                            tmp = (int)(1192*(Y1-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y1][Cr];	 //R
                            pImageTemp+=4;  //next YCbYCr pair.
                        }
                        pImageData -= size_x*2;
                    }
                    break;
                case 4: //CrYCbY
                    for (i=0; i<size_y; i++) {
                        pImageTemp = pImageData;
                        for (j=0; j<size_x; j+=2) {
                            Y0 = pImageTemp[1];
                            Cr = pImageTemp[0];
                            Y1 = pImageTemp[3];
                            Cb = pImageTemp[2];
                            *pBmpBuffer++ = BtblITU[Y0][Cb];     //B
                            tmp = (int)(1192*(Y0-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y0][Cr];	 //R

                            *pBmpBuffer++ = BtblITU[Y1][Cb];   //B
                            tmp = (int)(1192*(Y1-16) - 832*(Cr-128) - 400*(Cb-128)) >>10;
                            if (tmp > 255)  tmp = 255;
                            if (tmp < 0)    tmp = 0;
                            *pBmpBuffer++ = tmp;				 //G
                            *pBmpBuffer++ = RtblITU[Y1][Cr];	 //R
                            pImageTemp+=4;  //next  pair
                        }
                        pImageData -= size_x*2;
                    }
                    break;
                }
            }

            //-------------------------------------------------------------------------
            // Ä¿Ç°Ö»ÊµÏÖÁËYUV422 -> YUV24
            void CImageProc::BufferConv_YUVRawToYUV24(
                __in const BYTE *pYUVRawBuffer,
                __in int nWidth,
                __in int nHeight,
                __in int nMode,
                __out BYTE *pYUV24Buffer)
            {
                int    nY0Index, nY1Index, nCbIndex, nCrIndex;
                const BYTE *pImageData = pYUVRawBuffer;
                
                switch (nMode)
                { 		 
                case 1: //YCbYCr
                    nY0Index = 0; nCbIndex = 1; nY1Index = 2; nCrIndex = 3;
                    break;
                case 2: //YCrYCb
                    nY0Index = 0; nCrIndex = 1; nY1Index = 2; nCbIndex = 3;
                    break;
                case 3: //CbYCrY
                    nCbIndex = 0; nY0Index = 1; nCrIndex = 2; nY1Index = 3;
                    break;
                case 4: //CrYCbY
                    nCrIndex = 0; nY0Index = 1; nCbIndex = 2; nY1Index = 3;
                    break;
                default://CbYCrY
                    nCbIndex = 0; nY0Index = 1; nCrIndex = 2; nY1Index = 3;
                    break;
                }

                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x += 2)
                    {
                        *pYUV24Buffer++ = pImageData[nY0Index];
                        *pYUV24Buffer++ = pImageData[nCbIndex];
                        *pYUV24Buffer++ = pImageData[nCrIndex];
                        *pYUV24Buffer++ = pImageData[nY1Index];
                        *pYUV24Buffer++ = pImageData[nCbIndex];
                        *pYUV24Buffer++ = pImageData[nCrIndex];
                        pImageData += 4;  //next pair.
                    }
                }
            }

            //-------------------------------------------------------------------------
            // YUV24 -> BMP   with flip
            void CImageProc::BufferConv_YUV24ToBmp(
                __in const BYTE *pYUV24Buffer,
                __in int nWidth,
                __in int nHeight,
                __in int nStandard, // 0: BT709
                __out BYTE *pBmpBuffer)
            {
                assert(pYUV24Buffer != nullptr);
                assert(pBmpBuffer != nullptr);
                assert(nWidth > 0 && nHeight > 0);
                assert(nStandard == 0);

                double Y, U, V, B, G, R;
                Y = U = V = R = G = B = 0.0;
                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        int i = (y * nWidth + x) * 3;
                        Y = pYUV24Buffer[i + 0];
                        U = pYUV24Buffer[i + 1];
                        V = pYUV24Buffer[i + 2];

                        if (nStandard == 0) // BT709
                        {
                            B = Y + 1.8556 * (U - 128);
                            G = Y - 0.1873 * (U - 128) - 0.4681 * (V - 128);
                            R = Y + 1.5748 * (V - 128);
                        }

                        int j = ((nHeight - 1 - y) * nWidth + x) * 3;
                        pBmpBuffer[j + 0] = (BYTE)GET_VALUE_IN_RANGE(B, 0, 255);
                        pBmpBuffer[j + 1] = (BYTE)GET_VALUE_IN_RANGE(G, 0, 255);
                        pBmpBuffer[j + 2] = (BYTE)GET_VALUE_IN_RANGE(R, 0, 255);
                    }
                }
            }

            //-------------------------------------------------------------------------
            // BMP -> YUV24   with flip
            void CImageProc::BufferConv_BmpToYUV24(
                __in const BYTE *pBmpBuffer,
                __in int nWidth,
                __in int nHeight,
                __in int nStandard, // 0: BT709
                __out BYTE *pYUV24Buffer)
            {
                assert(pBmpBuffer != nullptr);
                assert(pYUV24Buffer != nullptr);
                assert(nWidth > 0 && nHeight > 0);
                assert(nStandard == 0 || nStandard == 1);

                double Y, U, V, B, G, R;
                Y = U = V = R = G = B = 0.0;
                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        int i = (y * nWidth + x) * 3;
                        B = pBmpBuffer[i + 0];
                        G = pBmpBuffer[i + 1];
                        R = pBmpBuffer[i + 2];

                        if (nStandard == 0) // BT709
                        {
                            Y = 0.2126 * R + 0.7152 * G + 0.0722 * B;
                            U = 0.5389 * (B - Y) + 128;
                            V = 0.6350 * (R - Y) + 128;
                        }

                        int j = ((nHeight - 1 - y) * nWidth + x) * 3;
                        pYUV24Buffer[j + 0] = (BYTE)GET_VALUE_IN_RANGE(Y, 0, 255);
                        pYUV24Buffer[j + 1] = (BYTE)GET_VALUE_IN_RANGE(U, 0, 255);
                        pYUV24Buffer[j + 2] = (BYTE)GET_VALUE_IN_RANGE(V, 0, 255);

                    }
                }
            }

            void CImageProc::SplitYUV24Buffer(
                __in const BYTE *pYUV24Buffer,
                __in int nWidth,
                __in int nHeight,
                __out BYTE *pYBuffer,
                __out BYTE *pCbBuffer,
                __out BYTE *pCrBuffer)
            {
                assert(pYUV24Buffer != nullptr);
                assert(nWidth > 0);
                assert(nHeight > 0);
                assert(pYBuffer != nullptr);
                assert(pCbBuffer != nullptr);
                assert(pCrBuffer != nullptr);

                int nBufferSize = nWidth * nHeight;
                int nIndex = 0;
                for (int i = 0; i < nBufferSize; i++)
                {
                    nIndex = i * 3;
                    pYBuffer[i] = pYUV24Buffer[nIndex + 0];
                    pCbBuffer[i] = pYUV24Buffer[nIndex + 1];
                    pCrBuffer[i] = pYUV24Buffer[nIndex + 2];
                }
            }

            void CImageProc::GetYUV24_YBuffer(
                __in const BYTE *pYUV24Buffer,
                __in int nWidth,
                __in int nHeight,
                __out BYTE *pYBuffer)
            {
                GetYUV24_OneChannelBuffer(pYUV24Buffer, nWidth, nHeight, 0, pYBuffer);
            }

            void CImageProc::GetYUV24_CbBuffer(
                __in const BYTE *pYUV24Buffer,
                __in int nWidth,
                __in int nHeight,
                __out BYTE *pCbBuffer)
            {
                GetYUV24_OneChannelBuffer(pYUV24Buffer, nWidth, nHeight, 1, pCbBuffer);
            }

            void CImageProc::GetYUV24_CrBuffer(
                __in const BYTE *pYUV24Buffer,
                __in int nWidth,
                __in int nHeight,
                __out BYTE *pCrBuffer)
            {
                GetYUV24_OneChannelBuffer(pYUV24Buffer, nWidth, nHeight, 2, pCrBuffer);
            }

            void CImageProc::GetYUV24_OneChannelBuffer(
                __in const BYTE *pYUV24Buffer,
                __in int nWidth,
                __in int nHeight,
                __in int nChannelOffset,
                __out BYTE *pOneChannelBuffer)
            {
                assert(pYUV24Buffer != nullptr);
                assert(nWidth > 0);
                assert(nHeight > 0);
                assert(0 <= nChannelOffset && nChannelOffset <= 2);
                assert(pOneChannelBuffer != nullptr);
                int nBufferSize = nWidth * nHeight;
                for (int i = 0; i < nBufferSize; i++)
                {
                    pOneChannelBuffer[i] = pYUV24Buffer[i * 3 + nChannelOffset];
                }
            }

            /**************************************************************************************************
            *   bit10_To_bit8 : 10 bit to 8bit convert  
            *      make 10bit raw data to 8bit -> use upper 8bit data    
            *
            **************************************************************************************************/
            // v1.1.7  , Rickson
            void	CImageProc::bit10_To_bit8(
                BYTE *pSrcBuffer, BYTE *pDestBuffer,
                unsigned int width, unsigned int height)
            {
                unsigned short	LowByte, HighByte;
                BYTE			*psrcTemp, *pdestTemp;

                psrcTemp = pSrcBuffer;
                pdestTemp = pDestBuffer;

                for (unsigned int i = 0; i < width * height; i++)
                {			
                    LowByte = 0;
                    HighByte = 0;
                    LowByte  = psrcTemp[0]; //8bit
                    HighByte = psrcTemp[1];
                    *pdestTemp = (((LowByte >> 2) & 0x3F ) + ((HighByte << 6) & 0xC0)); //10 bit convert
                    psrcTemp+=2;
                    pdestTemp++;
                }
            }

            /**************************************************************************************************
            *   Shift10BitMode : 10 bit to 8bit shift  
            *
            *   @param      pImage, [in] Image Pointer
            *   @param      nWidth, [in] Image Width
            *	@param		nHeight, [in] Image Height  
            *
            *   @return     Error Value
            **************************************************************************************************/
            void	CImageProc::Shift10BitMode(unsigned char* pImage, unsigned char* pDest, unsigned int nWidth, unsigned int nHeight)
            {
                unsigned int i,j;
                unsigned int nByteWidth;

                nByteWidth = nWidth * 5 / 4 ;
                if (nByteWidth%4) {  //if not divided by 4, increase 2 byte for consider dummy 2 byte       // D D D ... D   
                    nByteWidth += 2;																		// D D D     D
                }							// Rickson , v1.1.7																// D D D     X <-- here consider last dummy 2 byte if not divided by 4.
                // D D D     X

                for (i=0; i<nHeight; i++)
                {
                    for (j=0; j<nWidth; j+= 4)
                    {
                        memcpy(&pDest[i*nWidth+j], &pImage[i*nByteWidth+j*5/4], 4);
                    }
                }
            }


            //  v1.1.7 , Rickson
            void	CImageProc::Make10BitMode(unsigned char* pImage, WORD* pDest, unsigned int nWidth, unsigned int nHeight)
            {
                unsigned int i,j;
                unsigned int nByteWidth;

                nByteWidth = nWidth * 5 / 4 ;
                if (nByteWidth%4) {  //if not divided by 4, increase 2 byte for consider dummy 2 byte       // D D D ... D   
                    nByteWidth += 2;																		// D D D     D
                }																							// D D D     X <-- here consider last dummy 2 byte if not divided by 4.

                for (i=0; i<nHeight; i++) {
                    for (j=0; j<nWidth; j+= 4) {
                        pDest[i*nWidth+j+0] = (WORD)((pImage[i*nByteWidth+j*5/4+0]<<2) + ((pImage[i*nByteWidth+j*5/4+4]& 0xC0) >>6));
                        pDest[i*nWidth+j+1] = (WORD)((pImage[i*nByteWidth+j*5/4+1]<<2) + ((pImage[i*nByteWidth+j*5/4+4]& 0x30) >>4));
                        pDest[i*nWidth+j+2] = (WORD)((pImage[i*nByteWidth+j*5/4+2]<<2) + ((pImage[i*nByteWidth+j*5/4+4]& 0x0C) >>2));
                        pDest[i*nWidth+j+3] = (WORD)((pImage[i*nByteWidth+j*5/4+3]<<2) + (pImage[i*nByteWidth+j*5/4+4]& 0x03));			
                        //memcpy(&pDest[i*nWidth+j], &pImage[i*nByteWidth+j*5/4], 4);
                    }
                }
            }

			//-------------------------------------------------
			void CImageProc::Make10BitMode(unsigned char* _pRaw10Buffer,unsigned char* _pHLRaw10Buffer,unsigned int _Width,unsigned int _Height)
			{
				//	CSI-2 RAW10
				//	A9A8A7A6A5A4A3A2	B9B8B7B6B5B4B3B2	C9C8C7C6C5C4C3C2	D9D8D7D6D5D4D3D2	D1D0C1C0B1B0A1A0 
				unsigned int Raw_10_Buffer_LineSize = (_Width*5)/4;

				if (Raw_10_Buffer_LineSize%4)
				{
					Raw_10_Buffer_LineSize+=2;
				}

				for (int y=0;y<int(_Height);y++)
				{
					for (int x=0;x<int(_Width);x+=4)
					{
						int X = x*5/4;
						unsigned short PixelA = (unsigned short(_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+0])<<2) + (_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+4] & 0x03);
						unsigned short PixelB = (unsigned short(_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+1])<<2) + ((_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+4] & 0x0C)>>2);
						unsigned short PixelC = (unsigned short(_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+2])<<2) + ((_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+4] & 0x30)>>4);
						unsigned short PixelD = (unsigned short(_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+3])<<2) + ((_pRaw10Buffer[y*Raw_10_Buffer_LineSize+X+4] & 0xC0)>>6);

						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+0] = ((PixelA >> 8) & (0x03));
						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+1] = ((PixelA     ) & (0xFF));

						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+2] = ((PixelB >> 8) & (0x03));
						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+3] = ((PixelB     ) & (0xFF));

						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+4] = ((PixelC >> 8) & (0x03));
						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+5] = ((PixelC     ) & (0xFF));

						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+6] = ((PixelD >> 8) & (0x03));
						_pHLRaw10Buffer[(y*_Width*2)+(2*x)+7] = ((PixelD     ) & (0xFF));
					}
				}
			}


            /**************************************************************************************************
            *   Shift10BitMode : 12 bit to 8bit shift  
            *
            *   @param      pImage, [in] Image Pointer
            *   @param      nWidth, [in] Image Width
            *	@param		nHeight, [in] Image Height  
            *
            *   @return     Error Value
            **************************************************************************************************/
            void	CImageProc::Shift12BitMode(unsigned char* pImage, unsigned char* pDest, unsigned int nWidth, unsigned int nHeight)
            {
                unsigned int i,j;
                unsigned int nByteWidth;

                nByteWidth = nWidth * 3 / 2 ;
                if (nByteWidth%4) {  //if not divided by 4, increase 2 byte for consider dummy 2 byte       // D D D ... D   
                    nByteWidth += 2;																		// D D D     D
                }											// v1.1.7 , Rickson												// D D D     X <-- here consider last dummy 2 byte if not divided by 4.

                for (i=0; i<nHeight; i++)
                {
                    for (j=0; j<nWidth; j+= 2)
                    {
                        memcpy(&pDest[i*nWidth+j], &pImage[i*nByteWidth+j*3/2], 2);
                    }
                }
            }

            // v1.1.7 , Rickson
            void	CImageProc::Make12BitMode(unsigned char* pImage, WORD* pDest, unsigned int nWidth, unsigned int nHeight)
            {
                unsigned int i,j;
                unsigned int nByteWidth;

                nByteWidth = nWidth * 3 / 2 ;
                if (nByteWidth%4) {  //if not divided by 4, increase 2 byte for consider dummy 2 byte       // D D D ... D   
                    nByteWidth += 2;																		// D D D     D
                }																							// D D D     X <-- here consider last dummy 2 byte if not divided by 4.

                for (i=0; i<nHeight; i++) {
                    for (j=0; j<nWidth; j+= 2) {
                        pDest[i*nWidth+j+0] = (WORD)((pImage[i*nByteWidth+j*3/2+0]<<4) + ((pImage[i*nByteWidth+j*3/2+2]& 0xF0) >>4));
                        pDest[i*nWidth+j+1] = (WORD)((pImage[i*nByteWidth+j*3/2+1]<<4) + (pImage[i*nByteWidth+j*3/2+2]& 0x0F));
                    }
                }
            }

            //-------------------------------------------------------------------------
            // by Great
            // ¸ù¾ÝV5U¶ÁÈ¡µÄBuffer£¬×ª»»³öRaw8¸ñÊ½µÄºÍBmp¸ñÊ½µÄBuffer
            // ²ÎÊý£º  [IN]pDataBuffer     V5U¶ÁÈ¡µÄBuffer
            //         [IN]nImageWidth     Í¼Ïñ¿í
            //         [IN]nImageHeight    Í¼Ïñ¸ß
            //         [IN]nDataFormat     0:YUV, 1:8bit Bayer 2: 10bit Bayer 3: 12 bit Bayer
            //         [IN]nColorOrder     if YUV : 1 -YCbYCr, 2 - YCrYCb, 3 - CbYCrY,4 - CrYCbY 
            //                             if Bayer:1 - BGGR , 2 - RGGB  , 3 - GBRG  ,4 - GRBG 
            //         [IN]nMipiMode       0:1L, 1:2L, 3:3L, 7:4L, 9:parallel
            //         [OUT]pRaw8Buffer    ×ª»»³öRaw8¸ñÊ½µÄBuffer
            //         [OUT]pBmpBuffer     ×ª»»³öBmp¸ñÊ½µÄBuffer
            // ·µ»ØÖµ£ºTRUE     ³É¹¦
            //         FALSE    Ê§°Ü
            //-------------------------------------------------------------------------
            BOOL CImageProc::GetRaw8AndBmpBuffer(
                unsigned char *pDataBuffer,
                UINT nImageWidth,
                UINT nImageHeight,
                UINT nDataFormat,
                UINT nColorOrder,
                UINT nMipiMode,
                unsigned char *pRaw8Buffer,
                unsigned char *pBmpBuffer)
            {
                BOOL bRet = TRUE;
                if (NULL == pDataBuffer || NULL == pRaw8Buffer || NULL == pBmpBuffer)
                {
                    bRet = FALSE;
                }
                else
                {
                    switch (nDataFormat)
                    {
                    case 0: //YUV mode
                        YCbCrITUToBmp(
                            nColorOrder,
                            pDataBuffer,
                            pBmpBuffer,
                            nImageWidth,
                            nImageHeight);
                        break;
                    case 1: //8 bit bayer mode
                        memcpy(
                            pRaw8Buffer,
                            pDataBuffer,
                            nImageWidth * nImageHeight);
                        RawToBmp(
                            nColorOrder,
                            pRaw8Buffer,
                            pBmpBuffer,
                            nImageWidth,
                            nImageHeight);
                        break;
                    case 2: //10 bit bayer mode
                        if (9 == nMipiMode)   //parallel
                        {
                            bit10_To_bit8(
                                pDataBuffer,
                                pRaw8Buffer,
                                nImageWidth,
                                nImageHeight);
                        }
                        else
                        {
                            Shift10BitMode(
                                pDataBuffer,
                                pRaw8Buffer,
                                nImageWidth,
                                nImageHeight);
                        }
                        RawToBmp(
                            nColorOrder,
                            pRaw8Buffer,
                            pBmpBuffer,
                            nImageWidth,
                            nImageHeight);
                        break;
                    case 3: //12 bit bayer mode
                        Shift12BitMode(
                            pDataBuffer,
                            pRaw8Buffer,
                            nImageWidth,
                            nImageHeight);

                        RawToBmp(
                            nColorOrder,
                            pRaw8Buffer,
                            pBmpBuffer,
                            nImageWidth,
                            nImageHeight);
                        break;
					case 12:
						Shift10BitMode_2PD(
							pDataBuffer, 
							pRaw8Buffer, 
							nImageWidth, 
							nImageHeight);

						RawToBmp(
							nColorOrder,
							pRaw8Buffer,
							pBmpBuffer,
							nImageWidth,
							nImageHeight);
						break;
                    default:
                        bRet = FALSE;
                        break;
                    }
                }
                return bRet;
            }

            void CImageProc::GetFlipedBmpBuffer(
                const unsigned char* pBmpBuffer,
                int nWidth,
                int nHeight,
                unsigned char* pFlipedBmpBuffer)
            {
                for (int y = 0; y < nHeight; y++)
                {
                    memcpy(&pFlipedBmpBuffer[(nHeight - 1 - y) * nWidth * 3],
                        &pBmpBuffer[y * nWidth * 3],
                        nWidth * 3);
                }
            }

            void CImageProc::Cal_RGBtoYBuffer(
                const unsigned char* pBmpBuffer,
                int nWidth,
                int nHeight,
                unsigned char* pYbuffer)
            {
                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        int _index = (y * nWidth + x) * 3;
                        double _Y = YVALUE(pBmpBuffer[_index + 0], pBmpBuffer[_index + 1], pBmpBuffer[_index + 2]);
                        _Y = GET_VALUE_IN_RANGE(_Y, 0, 255);
                        //pYbuffer[_index / 3] = (int)_Y;
                        pYbuffer[_index / 3] = DOUBLE2INT(_Y);
                    }
                }	
            }

			void CImageProc::Cal_RGBtoGBuffer(
				const unsigned char* pBmpBuffer,
				int nWidth,
				int nHeight,
				unsigned char* pGbuffer)
			{
				for (int y = 0; y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						int _index = (y * nWidth + x) * 3;
						double _G =  pBmpBuffer[_index + 1] ;
						_G = GET_VALUE_IN_RANGE(_G, 0, 255);
						//pYbuffer[_index / 3] = (int)_Y;
						pGbuffer[_index / 3] = DOUBLE2INT(_G);
					}
				}	
			}

			void CImageProc::Cal_RGBtoRBuffer(
				const unsigned char* pBmpBuffer,
				int nWidth,
				int nHeight,
				unsigned char* pGbuffer)
			{
				for (int y = 0; y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						int _index = (y * nWidth + x) * 3;
						double _G =  pBmpBuffer[_index + 2] ;
						_G = GET_VALUE_IN_RANGE(_G, 0, 255);
						//pYbuffer[_index / 3] = (int)_Y;
						pGbuffer[_index / 3] = DOUBLE2INT(_G);
					}
				}	
			}

			void CImageProc::Cal_RGBtoBBuffer(
				const unsigned char* pBmpBuffer,
				int nWidth,
				int nHeight,
				unsigned char* pGbuffer)
			{
				for (int y = 0; y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						int _index = (y * nWidth + x) * 3;
						double _G =  pBmpBuffer[_index + 0] ;
						_G = GET_VALUE_IN_RANGE(_G, 0, 255);
						//pYbuffer[_index / 3] = (int)_Y;
						pGbuffer[_index / 3] = DOUBLE2INT(_G);
					}
				}	
			}

            void CImageProc::Cal_YBuffertoGrayBmpBuffer(
                const unsigned char* pYbuffer,
                int nWidth,
                int nHeight,
                unsigned char* pBmpBuffer)
            {
                for (int y = 0; y < nHeight; y++)
                {
                    for (int x = 0; x < nWidth; x++)
                    {
                        int _index = (y * nWidth + x) * 3;
                        pBmpBuffer[_index + 0] = pYbuffer[_index / 3];
                        pBmpBuffer[_index + 1] = pYbuffer[_index / 3];
                        pBmpBuffer[_index + 2] = pYbuffer[_index / 3];
                    }
                }
            }

            void CImageProc::GetFlipedYBlockBuffer(
                const unsigned char *pYBuffer,
                unsigned char *pFlipedYBlockBuffer,
                int _ImgWidth,
                int _ImgHeight,
                int _StartX,
                int _StartY,
                int _BlockWidth,
                int _BlockHeight)
            {
                for (int y=0;y<_BlockHeight;y++)
                {
                    for (int x=0;x<_BlockWidth;x++)
                    {
                        int ICurX	= _StartX+x;
                        int	ICurY	= _StartY+y;
                        //-------------------------------------------------------------------------
                        // add by Great.xu for crush
                        if (ICurX < 0)
                        {
                            ICurX = 0;
                        }
                        if (ICurY < 0)
                        {
                            ICurY = 0;
                        }
                        if (ICurX >= _ImgWidth)
                        {
                            ICurX = _ImgWidth - 1;
                        }
                        if (ICurY >= _ImgHeight)
                        {
                            ICurY = _ImgHeight - 1;
                        }
                        //-------------------------------------------------------------------------
                        int	IDest	= (x+y*_BlockWidth);
                        int ISour	= (ICurX+(_ImgHeight-1-ICurY)*_ImgWidth);

                        *(pFlipedYBlockBuffer+IDest) = *(pYBuffer+ISour);
                    }   // for x
                }   // for y
            }   // GetYBlockBuffer

            void CImageProc::GetYBlockBuffer(
                const unsigned char *pYBuffer,
                unsigned char *pYBlockBuffer,
                int _ImgWidth,
                int _ImgHeight,
                int _StartX,
                int _StartY,
                int _BlockWidth,
                int _BlockHeight)
            {
                for (int y=0;y<_BlockHeight;y++)
                {
                    for (int x=0;x<_BlockWidth;x++)
                    {
                        int ICurX	= _StartX+x;
                        int	ICurY	= _StartY+y;
                        //-------------------------------------------------------------------------
                        // add by Great.xu for crush
                        if (ICurX < 0)
                        {
                            ICurX = 0;
                        }
                        if (ICurY < 0)
                        {
                            ICurY = 0;
                        }
                        if (ICurX >= _ImgWidth)
                        {
                            ICurX = _ImgWidth - 1;
                        }
                        if (ICurY >= _ImgHeight)
                        {
                            ICurY = _ImgHeight - 1;
                        }
                        //-------------------------------------------------------------------------
                        int	IDest	= (x + y * _BlockWidth);
                        int ISour	= (ICurX + ICurY * _ImgWidth);

                        *(pYBlockBuffer+IDest) = *(pYBuffer+ISour);
                    }   // for x
                }   // for y
            }   // GetYBlockBuffer

            void CImageProc::GetBMPBlockBuffer(
                const unsigned char *ImgSource,
                unsigned char *ImgBlock,
                int _ImgWidth,
                int _ImgHeight,
                int _StartX,
                int _StartY,
                int _BlockWidth,
                int _BlockHeight)
            {
                for (int y=0;y<_BlockHeight;y++)
                {
                    for (int x=0;x<_BlockWidth;x++)
                    {
                        for (int i=0; i<3;i++)
                        {
                            int ICurX	= _StartX+x;
                            int	ICurY	= _StartY+y;
                            //-------------------------------------------------------------------------
                            // add by Great.xu for crush
                            if (ICurX < 0)
                            {
                                ICurX = 0;
                            }
                            if (ICurY < 0)
                            {
                                ICurY = 0;
                            }
                            if (ICurX >= _ImgWidth)
                            {
                                ICurX = _ImgWidth - 1;
                            }
                            if (ICurY >= _ImgHeight)
                            {
                                ICurY = _ImgHeight - 1;
                            }
                            //-------------------------------------------------------------------------
                            int	IDest	= 3*(x+y*_BlockWidth);
                            int ISour	= 3*(ICurX+(_ImgHeight-1-ICurY)*_ImgWidth);

                            *(ImgBlock+IDest+i)	= *(ImgSource+ISour+i);
                        }   // for i
                    }   // for x
                }   // for y
            }   // GetBMPBlockBuffer

            void CImageProc::GetBMPBlockBufferNoFlip(
                const unsigned char *ImgSource,
                unsigned char *ImgBlock,
                int _ImgWidth,
                int _ImgHeight,
                int _StartX,
                int _StartY,
                int _BlockWidth,
                int _BlockHeight)
            {
                for (int y=0;y<_BlockHeight;y++)
                {
                    for (int x=0;x<_BlockWidth;x++)
                    {
                        for (int i=0; i<3;i++)
                        {
                            int ICurX	= _StartX+x;
                            int	ICurY	= _StartY+y;
                            //-------------------------------------------------------------------------
                            // add by Great.xu for crush
                            if (ICurX < 0)
                            {
                                ICurX = 0;
                            }
                            if (ICurY < 0)
                            {
                                ICurY = 0;
                            }
                            if (ICurX >= _ImgWidth)
                            {
                                ICurX = _ImgWidth - 1;
                            }
                            if (ICurY >= _ImgHeight)
                            {
                                ICurY = _ImgHeight - 1;
                            }
                            //-------------------------------------------------------------------------
                            int	IDest = 3 * (x + y * _BlockWidth);
                            int ISour = 3 * (ICurX + ICurY * _ImgWidth);

                            *(ImgBlock+IDest+i) = *(ImgSource+ISour+i);
                        }   // for i
                    }   // for x
                }   // for y
            }   // GetBMPBlockBufferNoFlip

            void CImageProc::ThreasholdYBuffer(
                unsigned char* pYBuffer,
                int nWidth,
                int nHeight,
                unsigned char cThreashold)
            {
                for (int j = 0; j < nHeight; j++)
                {
                    for (int i = 0; i < nWidth; i++)
                    {
                        if (pYBuffer[j * nWidth + i] < cThreashold)
                        {
                            pYBuffer[j * nWidth + i] = 0;
                        }
                        else
                        {
                            pYBuffer[j * nWidth + i] = 255;
                        }
                    }
                }
            }

            void CImageProc::BMPBufferQuarteReduce(
                unsigned char *pImgSource,
                unsigned char *pImgReduce,
                int nImgWidth,
                int nImgHeight)
            {
                int half_Width = nImgWidth / 2;
                int half_Height = nImgHeight / 2;
                for (int y = 0; y < half_Height; y++)
                {
                    for (int x = 0; x < half_Width; x++)
                    {
                        int ICurX = x * 2;
                        int	ICurY = y * 2;
                        int	IDest = 3 * (x + y * half_Width);
                        int ISour = 3 * (ICurX + ICurY * nImgWidth);
                        for (int i = 0; i < 3; i++)
                        {
                            *(pImgReduce + IDest + i) = *(pImgSource + ISour + i);
                        }
                    }
                }
            }

			void CImageProc::GetFlipedBmpBGRtoRGBBuffer(
				const unsigned char* pBmpBuffer,
				int nWidth,
				int nHeight,
				unsigned char* pFlipedBmpBuffer)
			{
				for (int j = 0; j < nHeight; j++)
				{
					for (int i = 0; i < nWidth; i++)
					{
						pFlipedBmpBuffer[(nHeight - 1 - j) * nWidth * 3 + i*3 + 0] =  pBmpBuffer[j * nWidth * 3 + i*3 + 2];
						pFlipedBmpBuffer[(nHeight - 1 - j) * nWidth * 3 + i*3 + 1] =  pBmpBuffer[j * nWidth * 3 + i*3 + 1];
						pFlipedBmpBuffer[(nHeight - 1 - j) * nWidth * 3 + i*3 + 2] =  pBmpBuffer[j * nWidth * 3 + i*3 + 0];
					}
				}
			}

			#ifndef ALIGNED_SIZE
						//origin define at <align.h>, input alignment the less 1 number than you want it. if you want to 4 alignment, input to alignment '3'.
			#define ALIGNED_SIZE(size, alignment) (((size) + (alignment)) & ~(alignment))
			#endif

			//HVS Fix 2PD issue
			void CImageProc::Shift10BitMode_2PD(unsigned char* pImage, unsigned char* pDest, unsigned int nWidth, unsigned int nHeight)
			{
				unsigned int i,j;
				unsigned int nByteWidthSrc = ALIGNED_SIZE(((nWidth * 5) >> 1), 3);
				unsigned int nDst, nSrc;
				WORD wTmp[4];
				WORD wTmp1;
				for(i=0; i<nHeight; i++) {
					for(j=0; j<nWidth; j+= 2) {
						nDst = i*nWidth+j;
						nSrc = i*nByteWidthSrc+((j*5) >> 1);
						wTmp[0] = (WORD)((pImage[nSrc+0]));	
						wTmp[1] = (WORD)((pImage[nSrc+1]));
						wTmp[2] = (WORD)((pImage[nSrc+2]));
						wTmp[3] = (WORD)((pImage[nSrc+3]));

						wTmp1 = (wTmp[0] + wTmp[1])/* >> 1*/;
						if(wTmp1 > 255)	wTmp1 = 255;
						pDest[nDst] = (wTmp1);

						wTmp1 = (wTmp[2] + wTmp[3])/* >> 1*/;
						if(wTmp1 > 255)	wTmp1 = 255;
						pDest[nDst+1] = (wTmp1);
					}
				}
			}
        }
    }
}

