#include "StdAfx.h"
#include "Algorithm.h"

#include <fftw3.h>

#include <Windows.h>
//-----------------------------------------------------------------------------
typedef fftw_plan (* pfftw_plan_dft_r2c_1d)(int n,double *in,fftw_complex *out,unsigned int flags);
typedef void (* pfftw_execute)(fftw_plan _p);
typedef void (* pfftw_destroy_plan)(fftw_plan _p);
//-----------------------------------------------------------------------------


namespace UTS
{
	namespace Algorithm
	{
		class FastFouriertTansform::FFTImplement
		{
		public:
			FFTImplement()
				:hInstance(nullptr)
			{
				this->hInstance = ::LoadLibrary(L"libfftw3-3.dll");
				if (nullptr != this->hInstance)
				{
					if((fftw_plan_dft_r2c_1d = (pfftw_plan_dft_r2c_1d)GetProcAddress(this->hInstance,"fftw_plan_dft_r2c_1d"))){}else{};
					if((fftw_execute = (pfftw_execute)GetProcAddress(this->hInstance,"fftw_execute"))){}else{};
					if((fftw_destroy_plan = (pfftw_destroy_plan)GetProcAddress(this->hInstance,"fftw_destroy_plan"))){}else{};
				}
			}
			~FFTImplement()
			{
				FreeLibrary(this->hInstance);
			}

			pfftw_plan_dft_r2c_1d fftw_plan_dft_r2c_1d;
			pfftw_execute fftw_execute;
			pfftw_destroy_plan fftw_destroy_plan;
		private:
			HINSTANCE hInstance;
		};
		//-------------------------------------------------------------------------
		FastFouriertTansform::FastFouriertTansform()
			:FFTImpl(new FFTImplement)
		{
		}
		FastFouriertTansform::~FastFouriertTansform()
		{
		}
		FastFouriertTansform* FastFouriertTansform::GetInstance()
		{
			static FastFouriertTansform* FFTInstance;
			if (FFTInstance == nullptr)
			{
				FFTInstance = new FastFouriertTansform();
			}
			return FFTInstance;
		}
		bool FastFouriertTansform::dft_r2c_1d(int nn,double *_in,double** _out)
		{
			bool bResult = true;

			fftw_complex * _outData = new fftw_complex[nn];
			memset(_outData,0,sizeof(fftw_complex)*nn);

			fftw_plan plan1;
			plan1 = this->FFTImpl->fftw_plan_dft_r2c_1d(nn,_in,_outData,FFTW_ESTIMATE);
			this->FFTImpl->fftw_execute(plan1);
			this->FFTImpl->fftw_destroy_plan(plan1);

			for (int j=0;j<2;j++)
			{
				for (int i=0;i<nn;i++)
				{
					_out[i][j] = _outData[i][j];
				}
			}
			return bResult;
		}
	}
}