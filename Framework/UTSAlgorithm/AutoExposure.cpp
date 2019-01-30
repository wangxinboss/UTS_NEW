#include "StdAfx.h"
#include "Algorithm.h"


namespace UTS
{
	namespace Algorithm
	{
		namespace AutoExposure
		{
			/*AE_Param SetDevice(BaseDevice *_dev,SensorDriver *_sensor)
			{
				dev = _dev;
				sensor = _sensor;
			}


			BOOL Auto_exposure(AE_Param *ae)
			{
				RGBTRIPLE rgb;

				int val = sensor->get_exposure();

				//ListLog("get_exposure = %x ", val);
				//printk("get_exposure: %x \n", val);

				if (val < 0) {
					return -1;
				}

				int reg_delta = 0;
				double p_last = -1.0;

				for (int i = 0; i < ae->trycnt; i++) 
				{
					//ReCapture(m_CommonIni.DummyFrame, m_CommonIni.AvgFrame);
					dev->ReCapture(1);
					unsigned char *pBmp = dev->get_bmp_buf();
					UTS::Algorithm::GetROIAvgRGB(pBmp, dev->GetFrameWidth(), dev->GetFrameHeight(), ae->filter, ae->rect, rgb);

					switch (ae->chn) {
					case AE_CHN_R: m_avg = rgb.rgbtRed; break;
					case AE_CHN_G: m_avg = rgb.rgbtGreen; break;
					case AE_CHN_B: m_avg = rgb.rgbtBlue; break;
					case AE_CHN_Y:
					default: m_avg = YVALUE(rgb.rgbtBlue, rgb.rgbtGreen, rgb.rgbtRed); break;
					}
					//printk("AE: E[%.1lf]\n", m_avg);
					//ListLog("AE: E[%.1lf]", m_avg);

					// bmpµÄbuffer»­µ½ÆÁÄ»ÉÏ
					DrawImage();

					if (i == 0 && abs( m_avg - ae->target) <= ae->limit) return 0;

					if (i > ae->trycnt-3 && abs( m_avg - ae->target) <= ae->limit) goto done;

					double limit = ae->limit * 0.2;
					if (limit < 2) limit = 2;

					if (abs( m_avg - ae->target) <= limit) goto done;
					reg_delta = get_next_exposure(m_avg, p_last, ae->target, reg_delta);   
					val += reg_delta;
					if (val < 0) val = 0;
					if (sensor->set_exposure(val) < 0) {
						//printk("set_exposure error: %d", val);
						//ListLog("set_exposure error: %d", val);
						return FALSE;
					}
					//printk("AE: Try[%d], W[%x]\n", i, val);
					//ListLog("AE: Try[%d], W[%x]",  i, val);
					Sleep(200);

					p_last = m_avg;
				}
				if (i == ae->trycnt) return -1;

done:
				map<int, int> regs;
				if (sensor->get_exposure_settings(val, regs) < 0) {
					//printk("get_exposure_settings : [%x = %s]\n",regs,val);
					return -1;
				}
				for (auto iter = regs.begin(); iter != regs.end(); ++iter) {
					char reg[0x20], valbuf[0x20]; 
					sprintf(reg, "0x%04x", iter->first);
					sprintf(valbuf, "0x%x", iter->second);

					CString sFile,sSection;
					sFile.Format(_T("%s\\%s.ini"), m_pPfMemory->GetModelPath(), "Register");
					sSection.Format(_T("%s_2"), ae->section);
					INIWrite_File(sFile,sSection, reg, valbuf);
				}
				return TRUE;
				
			}*/

		}
	}
}