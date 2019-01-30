/*Filter Calculator Version 3.03*/
/*the time and date : 2012/6/12 18:31:55*/
/*FC filename : micom_8M_EQ100323-LPF*/
/*fs,23438Hz*/
/*LSI No.,LC898211*/
/*Comment,Original*/

/* 8bit */
AFINI__ const struct STHALREG CsHalReg[] = {
	{ 0x0076, 0x30},	/*30,0076,6dB*/
	{ 0x0077, 0x50},	/*50,0077,30dB*/
	{ 0x0078, 0x60},	/*60,0078,18dB*/
	{ 0x0086, 0x40},	/*40,0086*/
	{ 0x00F0, 0x00},	/*00,00F0,Through,0dB,fs/1,invert=0*/
	{ 0x00F1, 0x00},	/*00,00F1,LPF,700Hz,0dB,fs/1,invert=0*/
{ 0xFFFF,	0xFF }
} ;
/* 16bit */
AFINI__ const struct STHALFIL	CsHalFil[]	= {
	{ 0x0030, 0x0000},	/*0000,0030,LPF,700Hz,0dB,fs/1,invert=0*/
	{ 0x0040, 0x7FF0},	/*7FF0,0040,0dB,invert=0*/
	{ 0x0042, 0x7850},	/*7850,0042,HBF,20Hz,500Hz,0dB,fs/1,invert=0*/
	{ 0x0044, 0x8850},	/*8850,0044,HBF,20Hz,500Hz,0dB,fs/1,invert=0*/
	{ 0x0046, 0x6FF0},	/*6FF0,0046,HBF,20Hz,500Hz,0dB,fs/1,invert=0*/
	{ 0x0048, 0x7FF0},	/*7FF0,0048,0dB,invert=0*/
	{ 0x004A, 0x7FF0},	/*7FF0,004A,0dB,invert=0*/
	{ 0x004C, 0x4030},	/*4030,004C,-6dB,invert=0*/
	{ 0x004E, 0x7FF0},	/*7FF0,004E,0dB,invert=0*/
	{ 0x0050, 0x04F0},	/*04F0,0050,LPF,300Hz,0dB,fs/1,invert=0*/
	{ 0x0052, 0x7610},	/*7610,0052,LPF,300Hz,0dB,fs/1,invert=0*/
	{ 0x0054, 0x2870},	/*2870,0054,DI,-10dB,fs/16,invert=0*/
	{ 0x0056, 0x0000},	/*0000,0056,DI,-10dB,fs/16,invert=0*/
	{ 0x0058, 0x7FF0},	/*7FF0,0058,DI,-10dB,fs/16,invert=0*/
	{ 0x005A, 0x0680},	/*0680,005A,LPF,400Hz,0dB,fs/1,invert=0*/
	{ 0x005C, 0x72F0},	/*72F0,005C,LPF,400Hz,0dB,fs/1,invert=0*/
	{ 0x005E, 0x7F70},	/*7F70,005E,HPF,35Hz,0dB,fs/1,invert=0*/
	{ 0x0060, 0x7ED0},	/*7ED0,0060,HPF,35Hz,0dB,fs/1,invert=0*/
	{ 0x0062, 0x7FF0},	/*7FF0,0062,Through,0dB,fs/1,invert=0*/
	{ 0x0064, 0x0000},	/*0000,0064,Through,0dB,fs/1,invert=0*/
	{ 0x0066, 0x0000},	/*0000,0066,Through,0dB,fs/1,invert=0*/
	{ 0x0068, 0x4C50},	/*4C50,0068,HPF,900Hz,-3.5dB,fs/1,invert=0*/
	{ 0x006A, 0x6470},	/*6470,006A,HPF,900Hz,-3.5dB,fs/1,invert=0*/
	{ 0x006C, 0x8010},	/*8010,006C,0dB,invert=1*/
	{ 0x006E, 0x7FF0},	/*7FF0,006E,0dB,invert=0*/
	{ 0x0070, 0x0000},	/*0000,0070,Cutoff,invert=0*/
	{ 0x0072, 0x0AF0},	/*0AF0,0072,LPF,700Hz,0dB,fs/1,invert=0*/
	{ 0x0074, 0x6A10},	/*6A10,0074,LPF,700Hz,0dB,fs/1,invert=0*/
{ 0xFFFF,	0xFFFF }
} ;
