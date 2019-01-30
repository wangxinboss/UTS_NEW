#include "StdAfx.h"
#include "OtpCali_S5K4H7_HNE5030.h"
#include "VCM.h"
#include "algorithm.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

#pragma comment(lib, "UTS_Sensor.lib")

#pragma comment(lib, "usb-frm11.lib")
//-----------------------------------------------------------------------------

namespace OtpCali_S5K4H7_HNE5030 {

	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K4H7_HNE5030::OtpCali_S5K4H7_HNE5030(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_OTP;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = 1768;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;

	
		CString strSection;
		if (!uts.dbCof.GetDeviceFilename(uts.info.nDeviceId, strSection))
		{
			uts.log.Error(_T("[%s]GetDeviceFilename Fail. nDeviceId = %d"), UTS_FN, uts.info.nDeviceId);
			return;
		}
		uts.log.Info(_T("%s"),strSection);

		uts.dbCof.GetDeviceSingleSetting(strSection, _T("BOARD_SET_INDEX_NUM"), BOARD_SET_INDEX_NUM, 0, _T("Initial Device index."));
		uts.log.Info(_T("BOARD_SET_INDEX_NUM: %d"),BOARD_SET_INDEX_NUM);

		wHVS_SENSOR_SLAVEADDR = 80;

// 		uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_SLAVEADDR"), strHVS_SENSOR_SLAVEADDR, _T("0x00"), _T("IIC Address"));
// 		_stscanf_s(strHVS_SENSOR_SLAVEADDR, _T("%x"), &wHVS_SENSOR_SLAVEADDR);

		uts.log.Info(_T("wHVS_SENSOR_SLAVEADDR: 0x%02X"),wHVS_SENSOR_SLAVEADDR);
	}

	int OtpCali_S5K4H7_HNE5030::LscCali(void *args)
	{
		uts.log.Info(_T("OtpCali_S5K4H7_HNE5030:LscCali"));
		return do_qulcomm_lsc_awb_cali();   //高通U版
	}

	int OtpCali_S5K4H7_HNE5030::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		uts.log.Info(_T("OtpCali_S5K4H7_HNE5030:do_qualcomm_LSCAWBCali"));
		PDAF_QulComm_RevL pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}

	int OtpCali_S5K4H7_HNE5030::get_wb_cali_data(WB_GAIN *gain, void *out)
	{
		uint8_t *buf = (uint8_t *)out;
		put_be_val(gain->RGain, buf, 2);
		put_be_val(gain->GGain, buf+2, 2);
		put_be_val(gain->BGain, buf+4, 2);
		return 6;
	}

	int OtpCali_S5K4H7_HNE5030::do_prog_otp()
	{
		//Dump OTP-IN-DB Data==========================================
		uint8_t Otp_data_DB_data[7158] = {0};
		for(int i = 0 ; i < 7158 ; i++)
		{
			Otp_data_DB_data[i] = otp_data_in_db[i];
		}

		FILE *input = fopen("otp-buffer_OTP-data_IN_DB.txt","w");
		if(input != NULL)
		{
			for(int i = 0; i < 7158; i++)
			{
				fprintf(input,"Otp data [0x%04x]:[0x%02x]\n", i,Otp_data_DB_data[i]);
			}
		
		}
		fclose(input);

		DW9763_Write(0x0000,Otp_data_DB_data,7158);

// 		if(sensor->do_prog_otp(4,0x0a34,otp_data_in_db+0,16) < 0)
// 			return SET_ERROR(OTPCALI_ERROR_PROGERR);
// 
// 		for(int i = 0; i < 5; i++)
// 		{
// 			if(sensor->do_prog_otp(i+5,0x0a04,otp_data_in_db+16+i*64,64) < 0)
// 			return SET_ERROR(OTPCALI_ERROR_PROGERR);
// 		}
// 
// 		if(sensor->do_prog_otp(10,0x0a04,otp_data_in_db+336,24) < 0)
// 			return SET_ERROR(OTPCALI_ERROR_PROGERR);
// 		
// 		//Moudle Info flag
// 		if(sensor->do_prog_otp(14,0x0a04,otp_data_in_db+360,26) < 0)
// 			return SET_ERROR(OTPCALI_ERROR_PROGERR);
// 
// 		//Check sum
// 		if(sensor->do_prog_otp(15,0x0a12,otp_data_in_db+386,1) < 0)
// 			return SET_ERROR(OTPCALI_ERROR_PROGERR);

		//////////////////////////////////////////////////////////////////////////

		return SET_ERROR(OTPCALI_ERROR_NO);
	}

	int OtpCali_S5K4H7_HNE5030::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;
		USES_CONVERSION;

		//module info
		int ret = get_minfo_from_db(&otp->mInfo);

		//LSC
		for(int i = 0;i<MTK_LSC_LEN_HNE5030; i++)
		{
			otp->lsc.lscbuffer[i] = 0;
		}

		uint8_t _lscBuffer[1768] = {0};
		ret = get_lsc_from_raw_data(_lscBuffer, sizeof(_lscBuffer));

		for(int i = 0; i<1768; i++)
		{
			otp->lsc.lscbuffer[i] = _lscBuffer[i];
		}
		if (ret < 0) return ret;

		//otp->CheckSumLsc = ((CheckSum(&otp->lsc.lscbuffer, 1768)) % 255) + 1;

		uts.log.Error(_T("LSC sum = %d") , (CheckSum(&otp->lsc.lscbuffer,1768)));
		return sizeof(OTPData);
	}

	int OtpCali_S5K4H7_HNE5030::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		unsigned char otpbuf[7158];

		DW9763_Read(0x0000,otpbuf,7158);

		memcpy(otp,otpbuf,sizeof(OTPData));


// 		int m_addr,l_addr,w_addr;
// 		char otpbuf[387];
// 		memset(otpbuf,0,387);
// 
// 		//Moudle flag
// 		if(sensor->do_read_otp(4,0x0a34,otpbuf,16))
// 			return SET_ERROR(OTPCALI_ERROR_EEPROM);
// 
// 		for(int i = 0; i < 5; i++)
// 		{
// 			if(sensor->do_read_otp(i+5,0x0a04,otpbuf+16+i*64,64))
// 				return SET_ERROR(OTPCALI_ERROR_EEPROM);
// 		}
// 
// 		if(sensor->do_read_otp(10,0x0a04,otpbuf+336,24))
// 			return SET_ERROR(OTPCALI_ERROR_EEPROM);
// 
// 		//Moudle Info
// 		if(sensor->do_read_otp(14,0x0a04,otpbuf+360,26))
// 			return SET_ERROR(OTPCALI_ERROR_EEPROM);
// 
// 		//Moudle Info flag
// 		if(sensor->do_read_otp(15,0x0a12,otpbuf+386,1) < 0)
// 			return SET_ERROR(OTPCALI_ERROR_PROGERR);
// 
// 		memcpy(otp,otpbuf,sizeof(OTPData));

		return sizeof(otp);
	}

	int OtpCali_S5K4H7_HNE5030::get_minfo_from_db(void *args)
	{
		USES_CONVERSION;
		MINFO *m = (MINFO *)args;

		m->Flag = 0x01;
		
		m->VendorID = 0x07;

		SYSTEMTIME stime;
		time_t time;
		otpDB->get_otp_data_lock_time(mid, &time);
		if (time < 0)
		{
			GetLocalTime(&stime);
			m->year  = stime.wYear % 100;
			m->month = (uint8_t)stime.wMonth;
			m->day   = (uint8_t)stime.wDay;
			m->hour  = (uint8_t)stime.wHour;
			m->minu  = (uint8_t)stime.wMinute;
			m->sec =  (uint8_t)stime.wSecond;
		}
		else
		{
			struct tm today;
			_localtime64_s( &today, &time );

			m->year  = today.tm_year % 100;
			m->month = (uint8_t)today.tm_mon + 1;
			m->day   = (uint8_t)today.tm_mday;
			m->hour  = (uint8_t)today.tm_hour;
			m->minu  = (uint8_t)today.tm_min;
			m->sec =  (uint8_t)today.tm_sec;
		}
		
		m->LnesID = 0x4B;
		m->VCMID = 0x30; 
		m->DriverICID = 0x01;
		m->OIS_FW_VER = 0xFF;
		m->SensorID = 0x1C;
		m->ProductID = 0x31;
		m->VersionID = 0x04;
		m->HistoryID = 0x01;

		//////////////////////////////////////////////////////////////////////////
		//获取sensorID
		CString sensorid = uts.info.strSensorId;

		sensorid.MakeLower();

		char buffer[16];

		for (int i = 0; i < sensorid.GetLength(); i++)
		{
			buffer[i] = sensorid.GetAt(i);
			if (buffer[i] <= 57)
			{
				buffer[i] = buffer[i]-48;
			}
			else if (buffer[i] >= 97 ||  buffer[i] <= 122)
			{
				buffer[i] = buffer[i]-87;
			}
		}

		char FuseIdBuf[16];

		//0x0010-0x0017 数据替换成 0x0017-0x0010
		for (int index1=0; index1<16; index1++)
		{
			FuseIdBuf[index1] = buffer[15-index1];
		}

		for (int index2=0; index2<16; index2++)
		{
			buffer[index2] = FuseIdBuf[index2];
		}

		for (int i = 0; i < sensorid.GetLength(); i+=2)
		{
			m->FuseID[i/2] = (int)(buffer[i+1]*16+ buffer[i]);
		}
		//////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 11; i++)
		{
			m->Reserved[i] = 0xff;
		}

		m->EEPOROTP = 1;

		int sum = CheckSum(&m->Flag+1, 35);

		uts.log.Error(_T("mInfo sum = %d") , sum);

		m->InfoCheckSum = (sum % 0xff) + 1;

		uts.log.Error(_T("mInfo CheckSum = %d") , m->InfoCheckSum);


		//WB
// 		WB_DATA_USHORT wb[2];
// 		int ret = get_wb_from_raw_data(&wb[0], &wb[1]);
// 		if (ret < 0) 
// 		{ 
// 			uts.log.Info(_T("get wb fail"));
// 			return ret;
// 		}
// 		uts.log.Info(_T("get wb success"));
// 	
// 		uts.log.Error(_T("Get WB[0].R = %d") ,  wb[0].R);
// 		uts.log.Error(_T("Get WB[0].Gr = %d") ,  wb[0].Gr);
// 		uts.log.Error(_T("Get WB[0].Gb = %d") ,  wb[0].Gb);
// 		uts.log.Error(_T("Get WB[0].B = %d") ,  wb[0].B);
// 
// 		m->WB_R = (wb[0].R/4 -16);
// 		m->WB_Gr = (wb[0].Gr/4 -16);
// 		m->WB_Gb = (wb[0].Gb/4 -16);
// 		m->WB_B = (wb[0].B/4 -16);
// 
// 		m->WB_GoldenR =87;//86;
// 		m->WB_GoldenGr = 162;//147;
// 		m->WB_GoldenGb = 163;//147;
// 		m->WB_GoldenB = 106;//101;	
// 
// 		int sum = CheckSum(&m->flag+1, 24);
// 		uts.log.Error(_T("Mininfo sum = %d") , sum);
// 		m->m_CheckSum = sum% 0xff+1;

		return sizeof(MINFO);
	}

	int OtpCali_S5K4H7_HNE5030::get_uniform_otp_data(void *in, void *out, int maxlen)
	{
		OTPData *otp = (OTPData*)in;
		OtpDataHeader *hdr = (OtpDataHeader*)out;
		hdr->len = 0;
		
		return sizeof(OtpDataHeader) + hdr->len;
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K4H7_HNE5030::get_otp_group(void)
	{
		return 0;
	}

	int OtpCali_S5K4H7_HNE5030::do_check_otp()
	{
		uts.log.Info(_T("OtpCali_S5K5E8_MF868：do_check_otp"));

// 		uint8_t m_CheckSum[25];
// 		uint8_t lsc_CheckSum[360];
// 		memset(m_CheckSum,0,25);
// 		memset(lsc_CheckSum,0,360);
// 
// 		uint8_t Info_m_CheckSum;
// 		uint8_t Info_lsc_CheckSum;
// 
// 		uint8_t CheckSum2;
// 
// 		for(int i = 0; i < 360; i++)
// 		{
// 			lsc_CheckSum[i] = otp_data_in_sensor[i];
// 		}
// 		Info_lsc_CheckSum = CheckSum(lsc_CheckSum,360) %255 + 1;
// 
// 
// 		for(int i = 0; i < 25; i++)
// 		{
// 			m_CheckSum[i] = otp_data_in_sensor[i+1];
// 		}
// 		Info_m_CheckSum = CheckSum(m_CheckSum,24) %255 + 1;
// 
// 		if((Info_lsc_CheckSum != otp_data_in_sensor[386]) || (Info_m_CheckSum != otp_data_in_sensor[385]) ) {
// 			uts.log.Error(_T("Info LSC check sum : OTP check sum = %d : %d") , Info_lsc_CheckSum , otp_data_in_sensor[386]);
// 			uts.log.Error(_T("Info m check sum : OTP check sum = %d : %d") , Info_m_CheckSum , otp_data_in_sensor[3856]);
// 			return -OTPCALI_ERROR_CHKERR;
// 		}

 		return OTPCALI_ERROR_NO;
	}

	int OtpCali_S5K4H7_HNE5030::do_check_otp(void *args)
	{
		uts.log.Info(_T("OtpCali_S5K5E8_MF868：do_check_otp(void *args)"));

		USHORT address = 0x00;

		unsigned char Data[0x1BF6];



		BL24C64A_Read(address,Data,0x1BF6);

		OnOTPRead(address,(char*)Data,0x1BF6);

		return OTPCALI_ERROR_NO;
	}

	BOOL OtpCali_S5K4H7_HNE5030::DW9763_Read(USHORT address,unsigned char *Data,int size)
	{
		int i=0;
		int group=0;
		int num=0;

		int Count= address%32;
		Count=32-Count;
		if(size-Count>0)
		{
			group = (size-Count)/32;
			num=(size-Count)%32;
		}
		else
		{
			group=0;
			num=size;
			Count=0;
		}

		if(Count>0)
		{
			I2C_SYS_Read_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address, Count, Data);
			Sleep(30);
		}

		for (i=0;i<group;i++)
		{
			I2C_SYS_Read_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+(Count+i*32), 32, Data+(Count+i*32));
			Sleep(10);
		}

		I2C_SYS_Read_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+(Count+i*32), num, Data+(Count+i*32));
		Sleep(30);

		return TRUE;
	}

	BOOL OtpCali_S5K4H7_HNE5030::DW9763_Write(USHORT address,unsigned char *Data,int size)
	{
		BYTE dataBuf[10];

		//关闭写保护
		dataBuf[0] = 0x7F;
		I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, 0x18<<1, 1, 0xF9, 1, dataBuf);

		dataBuf[0] = 0x04;
		I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, 0x18<<1, 1, 0xA0, 1, dataBuf);

		//擦除所有数据
		dataBuf[0] = 0xEE;
		I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 1, 0x81, 1, dataBuf);

		for (int i = 0; i < size; i++)
		{
			if ( (i == 0) || (i == 1))
			{
				I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+i, 1, &Data[i]);
				Sleep(10);
			}
			I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+i, 1, &Data[i]);
			//Sleep(10);
		}

		//打开写保护
		dataBuf[0] = 0x00;
		I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, 0x18<<1, 1, 0xA0, 1, dataBuf);

		dataBuf[0] = 0x9D;
		I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, 0x18<<1, 1, 0xED, 1, dataBuf);

		return TRUE;
	}

	BOOL OtpCali_S5K4H7_HNE5030::BL24C64A_Read(USHORT address,unsigned char *Data,int size)
	{
		uts.log.Info(_T("OtpCali_S5K5E8_MF868：BL24C64A_Read :0x%02X "),BOARD_SET_INDEX_NUM);
		uts.log.Info(_T("OtpCali_S5K5E8_MF868：BL24C64A_Read :0x%02X "),wHVS_SENSOR_SLAVEADDR);

		int i=0;
		int group=0;
		int num=0;

		int Count= address%32;
		Count=32-Count;
		if(size-Count>0)
		{
			group = (size-Count)/32;
			num=(size-Count)%32;
		}
		else
		{
			group=0;
			num=size;
			Count=0;
		}

		if(Count>0)
		{
			I2C_SYS_Read_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address, Count, Data);
			//I2C_SYS_Read_Mul(gDeviceIndex, m_CHVSBoardCnt.GetDeviceID()<<1, 2, address, Count, Data);
			Sleep(30);
		}

		for (i=0;i<group;i++)
		{
			I2C_SYS_Read_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+(Count+i*32), 32, Data+(Count+i*32));
			//I2C_SYS_Read_Mul(gDeviceIndex, m_CHVSBoardCnt.GetDeviceID()<<1, 2, address+(Count+i*32), 32, Data+(Count+i*32));
			Sleep(10);
		}

		I2C_SYS_Read_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+(Count+i*32), num, Data+(Count+i*32));
		//I2C_SYS_Read_Mul(gDeviceIndex, m_CHVSBoardCnt.GetDeviceID()<<1, 2, address+(Count+i*32), 32, Data+(Count+i*32));
		Sleep(30);

		return TRUE;
	}

	BOOL OtpCali_S5K4H7_HNE5030::BL24C64A_Write(USHORT address,unsigned char *Data,int size)
	{
		int i=0;
		int group=0;
		int num=0;

		int Count= address%32;
		Count=32-Count;
		if(size-Count>0)
		{
			group = (size-Count)/32;
			num=(size-Count)%32;
		}
		else
		{
			group=0;
			num=size;
			Count=0;
		}
		if(Count>0)
		{
			I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address, Count, Data);
			Sleep(10);
		}
		for (i=0;i<group;i++)
		{
			I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+(Count+i*32), 32, Data+(Count+i*32));
			Sleep(10);
		}

		I2C_SYS_Write_Mul(BOARD_SET_INDEX_NUM, wHVS_SENSOR_SLAVEADDR<<1, 2, address+(Count+i*32), num, Data+(Count+i*32));
		Sleep(10);
		return TRUE;
	}

	CString OtpCali_S5K4H7_HNE5030::GetModulePath()
	{
		int  i    = 0;
		int  len  = 0;
		bool flag = false;
		char ModulePath[512] = {0};
		char path[512]={0};
		CString myStr;

		GetModuleFileName(NULL, (LPWSTR)ModulePath, 512);//return  real  lenghth
		len = strlen(ModulePath);

		for (i = len - 1; i >= 0; i--)
		{
			if (ModulePath[i] == '\\')
			{
				ModulePath[i + 1] = 0;
				flag = true;
				strcpy(path, ModulePath);	
				break;
			}
		}

		if (!flag)
		{
			strcpy(path, "");
		}
		myStr.Format(_T("%s"),path);

		return myStr;
	}

	void OtpCali_S5K4H7_HNE5030::OnOTPRead(int startAddr,char* value,int lens)
	{
		FILE *fp = fopen("ReadBackValue.ini","w");

		if(fp != NULL)
		{ 
			for (int i = 0; i < lens; i++)
			{
				fprintf(fp,"0x%04X,",startAddr+i);
				fprintf(fp,"0x%02X",value[i]);
				fprintf(fp,"\n");
			}

			fclose (fp);
			uts.log.Info(_T("buffer save ok"));
		}
		else
		{
			uts.log.Error(_T("buffer save error"));
		}
	}
          
}
