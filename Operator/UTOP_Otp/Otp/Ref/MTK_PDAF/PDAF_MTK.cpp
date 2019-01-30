#include "StdAfx.h"
#include "PDAF_MTK.h"

#include "../../3rdparty/PDAF/MTK/DLL_3.0.1.0322/pd_cali_dll.h"
#pragma comment(lib,"pd_cali_dll.lib")

int g_cap_num = 0;
int g_output_size_max = 0;
bool init_mtfpdaf = false;

int pdaf_init(char *filename, int& cap_num, int& output_size_max)
{
	int res = PD_ERR_OK;

	if(init_mtfpdaf == false)
	{   
		res = pd_cali_init(filename);
		if (res == PD_ERR_OK)	res = pd_cali_get_capture_num(cap_num);
		if (res == PD_ERR_OK)	res = pd_cali_get_max_output_size(output_size_max);

		g_cap_num = cap_num;
		g_output_size_max = output_size_max;
		init_mtfpdaf =true;
	}else
	{
		cap_num = g_cap_num;
		output_size_max = g_output_size_max;
	}
	return res;
}

int pdaf_spc(unsigned short*raw, int& output_size1, unsigned char *output1)
{
	int res = PD_ERR_OK;

	res = pd_cali_proc1(raw, (char*)output1, g_output_size_max, output_size1);

	return res;
}

int pdaf_dcc(char **raw_seq, int *lensPos, int& output_size2, unsigned char *output2)
{
	int res = PD_ERR_OK;

	res = pd_cali_proc2(raw_seq, lensPos, g_cap_num, (char*)output2, g_output_size_max, output_size2);

	return res;
}

int pdaf_dcc(char **raw_seq, int *lensPos, char* spctable,int& output_size2, unsigned char *output2)
{
	int res = PD_ERR_OK;

	res = pd_cali_proc2(raw_seq, lensPos, g_cap_num,spctable, (char*)output2, g_output_size_max, output_size2);

	return res;
}

int pdaf_generate_eeprom(int output_size1, unsigned char *output1, int output_size2, unsigned char *output2, int& eeprom_size, unsigned char *eeprom)
{
	int res = PD_ERR_OK;

	res = pd_cali_verify_proc1(output1);
	if (res == PD_ERR_OK)	res = pd_cali_verify_proc2(output2);

	if (res == PD_ERR_OK)
	{
		eeprom_size = output_size1 + output_size2 ;
		memcpy(eeprom, output1, output_size1);
		memcpy(eeprom+output_size1, output2, output_size2);
	}

	if (res == PD_ERR_OK)	res = pd_cali_verify(eeprom);

	return res;
}

int pdaf_verify_eeprom(unsigned char *eeprom)
{
	int res = PD_ERR_OK;

	res = pd_cali_verify(eeprom);

	return res;
}

int pdaf_verify_spc(unsigned char *output1)
{
	int res = PD_ERR_OK;

	res = pd_cali_verify_proc1(output1);
	return res;
}

int pdaf_verify_dcc(unsigned char *output2)
{
	int res = PD_ERR_OK;

	res = pd_cali_verify_proc2(output2);

	return res;
}



int pdaf_get_DAC_dcc(unsigned char *output2, int& DAC)
{
	int res = PD_ERR_OK;

	res = pd_cali_get_DAC_proc2(output2, DAC);

	return res;
}

int pdaf_get_DAC_dcc(unsigned short*raw,unsigned char *output1,unsigned char *output2,int currentdac ,int& DAC)
{
	int res = PD_ERR_OK;

	res = pd_cali_get_target_DAC(raw, output1, output2, currentdac, DAC);

	return res;
}

int pdaf_get_LinearitySize(unsigned char *output2, int& GridX,int& GridY)
{
	int res = PD_ERR_OK;

	res = pd_cali_get_linearity_size(output2, GridX, GridY);

	return res;
}

int pdaf_get_Linearity(unsigned char *output2,double *LinearityBuf)
{
	int res = PD_ERR_OK;

	res = pd_cali_get_linearity(output2, LinearityBuf);

	return res;
}