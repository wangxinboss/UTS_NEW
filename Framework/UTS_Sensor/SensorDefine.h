#pragma once

namespace UTS
{
    typedef enum _e_sensor_type_
    {
        Sensor_Virtual = 0,
		Sensor_AR1337,
		Sensor_GC2385,
		Sensor_GC5024,
		Sensor_GC5025,
		Sensor_GC5034,
		Sensor_GC8024,
		Sensor_Hi546,
		Sensor_Hi556,
		Sensor_Hi846,
		Sensor_Hynix1333,
		Sensor_Hynix843,
		Sensor_IMX214,
		Sensor_IMX362,
		Sensor_IMX363,
		Sensor_IMX386,
		Sensor_IMX499,
		Sensor_IMX519,
		Sensor_OV12870,
		Sensor_OV12A10,
		Sensor_OV13855,
		Sensor_OV13858,
		Sensor_OV16885,
		Sensor_OV4688,
		Sensor_OV5648,
		Sensor_OV5675,
		Sensor_OV5695,
		Sensor_OV8856,
		Sensor_OV9750,
		Sensor_S5K2L7,
		Sensor_S5K2P7,
		Sensor_S5K3L6,
		Sensor_S5K3L8XX,
		Sensor_S5K3P3,
		Sensor_S5K3P8,
		Sensor_S5K3P9SP,
		Sensor_S5K3P9SX,
		Sensor_S5K4H7YX,
		Sensor_S5K4H8YX,
		Sensor_S5K5E2YA,
		Sensor_S5K5E8YX,
		Sensor_S5K5E9,
		Sensor_SP0A09,
		Sensor_SP2509V,
		Sensor_SP250A,
		Sensor_SUPPORT_NUM,
    } eSensorType;

#define SENSOR_DISCRIPTION		\
    _T("0: Sensor_Virtual,")	 \
	_T("01:Sensor_AR1337, ")	\
	_T("02:Sensor_GC2385, ")	\
	_T("03:Sensor_GC5024, ")	\
	_T("04:Sensor_GC5025, ")	\
	_T("05:Sensor_GC5034, ")	\
	_T("06:Sensor_GC8024, ")	\
	_T("07:Sensor_Hi546, ")		\
	_T("08:Sensor_Hi556, ")		\
	_T("09:Sensor_Hi846, ")		\
	_T("10:Sensor_Hynix1333, ")	\
	_T("11:Sensor_Hynix843, ")	\
	_T("12:Sensor_IMX214, ")	\
	_T("13:Sensor_IMX362, ")	\
	_T("14:Sensor_IMX363, ")	\
	_T("15:Sensor_IMX386, ")	\
	_T("16:Sensor_IMX499, ")	\
	_T("17:Sensor_IMX519, ")	\
	_T("18:Sensor_OV12870, ")	\
	_T("19:Sensor_OV12A10, ")	\
	_T("20:Sensor_OV13855, ")	\
	_T("21:Sensor_OV13858, ")	\
	_T("22:Sensor_OV16885, ")	\
	_T("23:Sensor_OV4688, ")	\
	_T("24:Sensor_OV5648, ")	\
	_T("25:Sensor_OV5675, ")	\
	_T("26:Sensor_OV5695, ")	\
	_T("27:Sensor_OV8856, ")	\
	_T("28:Sensor_OV9750, ")	\
	_T("29:Sensor_S5K2L7, ")	\
	_T("30:Sensor_S5K2P7, ")	\
	_T("31:Sensor_S5K3L6, ")	\
	_T("32:Sensor_S5K3L8XX, ")	\
	_T("33:Sensor_S5K3P3, ")	\
	_T("34:Sensor_S5K3P8, ")	\
	_T("35:Sensor_S5K3P9SP, ")	\
	_T("36:Sensor_S5K3P9SX, ")	\
	_T("37:Sensor_S5K4H7YX, ")	\
	_T("38:Sensor_S5K4H8YX, ")	\
	_T("39:Sensor_S5K5E2YA, ")	\
	_T("40:Sensor_S5K5E8YX, ")	\
	_T("41:Sensor_S5K5E9, ")	\
	_T("42:Sensor_SP0A09, ")	\
	_T("43:Sensor_SP2509V, ")	\
	_T("44:Sensor_SP250A, ")	\
    EMPTY_STR

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;

    enum {
        I2CWRITE,
        I2CREAD,
        I2CSLEEP,
        I2CREWRAND,
        I2CREWROR,
    };

#define i2c_getattr(a)  ((a)>>16)
#define i2creg(a)       ((a)&0xFFFF)
#define i2c_setattr(a)  ((a)<<16)

    struct regval 
    {
        u32 reg;
        u16 value;
    };

}