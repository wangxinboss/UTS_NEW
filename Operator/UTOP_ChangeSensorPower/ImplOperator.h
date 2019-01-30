#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _operator_param_
    {
		double dSensor_PowerVolt[7];
		int nSensor_VCLK;
		int nSensor_VSYNC;
		int nSensor_ENB;
		int nSensor_RESET;
		int nSensor_SlaveAddr;
		int nSensor_ImageSizeX;
		int nSensor_ImageSizeY;
		int nSensor_DataFormat;
		int nSensor_OutputMode;
		int nSensor_MIPIMode;
		int nSensor_I2CMode;
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);

    private:
        //------------------------------------------------------------------------------
        // ²ÎÊý
        OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

