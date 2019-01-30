//********************************************************************************
//
//		<< LC898111 Evaluation Soft >>
//		Program Name	:AfIni.c
//		Design			: Y.Yamada
//		History			: First edition						2009.07.31 Y.Tashita
//		History			: LC898211 changes					2012.06.11 YS.Kim
//********************************************************************************
//**************************
//	Include Header File		
//**************************
#define		AFINI

#include	"Main.h"
#include	"Cmd.h"
#include	"Af.h"
#include	"AfFil.h"
#include	"AfDef.h"
#include	"Af_EEPROM_MEM.H"
#include	<stdio.h>
#include 	<stdlib.h>

//**************************
//	Local Function Prottype	
//**************************
void			IniCmd( void ) ;							// Command Execute Process Initial
void			IniDrv( void ) ;							// Initialze Driver by Memory MAP
unsigned char*	SetFil( unsigned char* ) ;					// Initialze Filter
void 			IniFil( void ) ;
void			IniSvo( void ) ;							// Servo On


//#define	EEPROM_FORMAT										// Initialize Funcition Select

//********************************************************************************
// Function Name 	: IniSet
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Initial Setting Function
// History			: First edition 						2009.07.30 Y.Tashita
//********************************************************************************
void	IniSet( void )
{
	unsigned char	UcParMod ;
	
	TRACE("Initial CMD 123 \n") ;
	
	//Initalize Stepmove Parameter( Default Setting )
	StSmvPar.UsSmvSiz	= STMV_SIZE ;
	StSmvPar.UcSmvItv	= STMV_INTERVAL ;
	StSmvPar.UcSmvEnb	= STMCHTG_SET | STMSV_SET | STMLFF_SET ;
	
	IniCmd() ;
	IniDrv() ;
	IniSvo() ;
	
	WitTim( 100 ) ;
	
	Stmv211Set( StSmvPar ) ;
	UcParMod =	Stmv211( 0x1000 ) ;					// Move to Base Position
}



#ifndef EEPROM_FORMAT	// Initialize LSI
//********************************************************************************
// Function Name 	: IniDrv
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Initial Driver(NORMAL)
// History			: First edition 						2012.06.11 YS.Kim
//********************************************************************************
void IniDrv( void )
{
	RegWriteA( CLKSEL_211	, 0x34 );				// CLKSEL 1/1, CLKON
	RegWriteA( ADSET_211	, 0x20 );				// AD 4Time
	RegWriteA( SWTCH_211	, 0x2C );				// RZ OFF, STSW=mx2x2, MS1IN OFF, MS2IN=RZ, FFIN AFTER, DSW ag
	RegWriteA( STBY_211		, 0xE0 );				// AD ON, DA ON, OP ON, DRMODE L, DRENA STBY
	RegWriteA( ENBL_211		, 0x05 );				// PIDSW OFF, AF ON, MS2 ON
	RegWriteA( ANA2_211		, 0xE5 );				// Internal OSC Setup (No01 = 24.18MHz)
	RegWriteA( DSSEL_211	, 0x60 );				// DSSEL 1/16 INTON
	
	RegWriteA( ANA1_211		, 0xC0  );				// Hall Bias:4k Hall Amp:25x Setup
	//RegWriteA( ANA1_211	, 0xD0  );				// Hall Bias:4k Hall Amp:50x Setup //100427
	//RegWriteA( ANA1_211	, 0xE0  );	            // Hall Bias:4k Hall Amp:75x Setup //100429  
	
	IniFil();
	
	// Settling time & Judge Level Setting-------------------------------//
	RamWriteA( CHTGX_211H	, CHTGX_THRESHOLD );	// Settling Threshold Value
	RegWriteA( CHTGN_211H	, CHTGOKN_TIME );		// Settling Time
	
	RegWriteA( MSSET_211	, INI_MSSET_211 );		// ms12b = -ms12a, ms22b = -ms22a
	
	RamWriteA( DAHLXO_211H	, 0x5AA9 );				// Hall Offset Bias
	RamWriteA( gain1_211H	, 0x4030 );				// Loop Gain
	
	RamWriteA( OFFSET_211H	, 0x0000 );				// OFFSET Clear
	RamWriteA( RZ_211H		, 0x0000 );				// RZ Clear(Target Value)
	RamWriteA( PIDZO_211H	, 0x0000 );				
	
	RegWriteA( PWMZONE2_211	, 0x7E );				// PWMZONE2
	RegWriteA( PWMZONE1_211	, 0x7E );				// PWMZONE1
	RegWriteA( PWMZONE0_211	, 0x7E );				// PWMZONE0
	RegWriteA( ZONE3_211	, 0x2C );				// ZONE3 Full
	RegWriteA( ZONE2_211	, 0x2C );				// ZONE2 Full
	RegWriteA( ZONE1_211	, 0x2C );				// ZONE1 Full
	RegWriteA( ZONE0_211	, 0x2C );				// ZONE0 Full
	RegWriteA( GCTIM_211	, 0x00 );				// Zone Change Timing
	RegWriteA( PWMSEL_211	, 0x00 );				// PWM 375KHz
	RegWriteA( STBY_211		, 0xF0 );				// PWM to Analog OUTPUT  ON
	RegWriteA( PWMSEL2_211	, 0x00 );				// PWMSEL2
	RegWriteA( PWMLIMIT_211	, 0x7F );				// PWM Limitter
	
	RegWriteA( CLR_211		, 0xC0 );				// AF filter, MS1 Clr
	WitTim(1);
	
	//RegWriteA( ENBL_211		, 0x85 );				// PIDSW ON, AF ON, MS2 ON
	//Stmv211Set();  								//Step Move Setup Function
}



//********************************************************************************
// Function Name 	: IniFil
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Hall Filter Initial Coefficient Setting(NORMAL)
// History			: First edition 						2012.06.11 YS.Kim
//********************************************************************************
void IniFil( void )
{
	unsigned char	UcAryId ;
	
	unsigned short	UsDatX12;
	unsigned short	UsDatX03;
	unsigned char	UcDatDss;
	unsigned short	UsDatAoe;
	
	UsDatX12	= (unsigned short)((unsigned short)(CsHalReg[0].UcRegDat) << 8 | (unsigned short)(CsHalReg[1].UcRegDat) | (unsigned short)(CsHalReg[4].UcRegDat) << 7);
	UsDatX03	= (unsigned short)CsHalReg[2].UcRegDat << 8 ;
	RegReadA( DSSEL_211 , & UcDatDss ) ;
	UcDatDss	= ( UcDatDss & 0x30 ) | CsHalReg[3].UcRegDat ;
	UsDatAoe	= (unsigned short)(CsHalFil[0].UsRamDat) | ((unsigned short)(CsHalReg[5].UcRegDat) << 12 ) ;
	
	RamWriteA( CsHalReg[0].UsRegAdd , UsDatX12 );
	RamWriteA( CsHalReg[2].UsRegAdd , UsDatX03 );
	RegWriteA( CsHalReg[3].UsRegAdd , UcDatDss );
	RamWriteA( CsHalFil[0].UsRamAdd , UsDatAoe );
	
	UcAryId	= 1 ;
	while( CsHalFil[ UcAryId ].UsRamAdd != 0xFFFF )
	{
		RamWriteA( CsHalFil[ UcAryId ].UsRamAdd , CsHalFil[ UcAryId ].UsRamDat ) ;
		UcAryId++ ;
	}
}
#endif



#ifdef EEPROM_FORMAT	// Initialize LSI by using EEPROM Data Format
//********************************************************************************
// Function Name 	: IniDrv
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Initial Driver(EEPROM FORMAT)
// History			: First edition 						2012.06.11 YS.Kim
//********************************************************************************
void IniDrv( void )
{
	unsigned short	End;
	unsigned char	*UcPtrHed;
	unsigned char	*UcPtrMem;
	unsigned char	*UcPtrTmp;
	
	End	= 0x0001;
	
	UcPtrHed = &IniSet_Mem[0][0];	// Start Address in EEPROM
	UcPtrMem = UcPtrHed;
	
	while( End ){
		End++;
		switch( *UcPtrMem )
		{
			case 0xFD:																						//Setting Filter
				UcPtrTmp	= UcPtrMem + 2;
				UcPtrMem	= UcPtrHed	+ *(UcPtrMem+1);													//Start Address for Filter coefficient Write
				UcPtrMem	= SetFil( UcPtrMem );
				break;
			case 0xFE:																						//Setting Calibration Data
				UcPtrTmp	= UcPtrMem + 2;
				UcPtrMem	= UcPtrHed	+ *(UcPtrMem+1);													//Start Address for Calribration data Write
				break;
			case 0xFF:																						//Diverge Next or Finish
				if( *(UcPtrMem+1) == 0xFF ){
					UcPtrMem	= UcPtrTmp;
				}else if( *(UcPtrMem+1) == 0x00 ){
					End = 0x0000;
				}
				break;
			default:
				if( *UcPtrMem > 0x7F ){																		//Register Address
					RegWriteA( *UcPtrMem,	*(UcPtrMem+1) );
					UcPtrMem += 2;
				}else{																						//RAM Address
					RamWriteA( *UcPtrMem,	( *(UcPtrMem+1) << 8 ) | *(UcPtrMem+3) );
					UcPtrMem += 4;
				}
			break;
		}
		
		if( End > 1024 ){
			End = 0x0000;
		}
	}
}




//********************************************************************************
// Function Name 	: SetFil
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Hall Filter Initial Coefficient Setting(EEPROM FORMAT)
// History			: First edition 						2012.06.11 YS.Kim
//********************************************************************************
unsigned char* SetFil( unsigned char* UcPtrFil )
{
	unsigned char	idx;
	unsigned char	UcStaAdr = 0x40;
	unsigned char	UcEndAdr = 0x75;
	
	unsigned short	UsDatX12;
	unsigned short	UsDatX03;
	unsigned char	UcDatDss;
	unsigned short	UsDatAoe;
	
	UsDatX12	= ( *(UcPtrFil+1) << 8 | *(UcPtrFil+3) ) | ( *(UcPtrFil+9) << 7 ) ;
	UsDatX03	= *(UcPtrFil+5) << 8 ;
	RegReadA( 0x86 , & UcDatDss ) ;
	UcDatDss	= ( UcDatDss & 0x30 ) |  *(UcPtrFil+5) ;
	UsDatAoe	= ( *(UcPtrFil+13) << 8	| *(UcPtrFil+15) ) | ( *(UcPtrFil+11) << 12 ) ;
	
	RamWriteA( *UcPtrFil , UsDatX12 );
	RamWriteA( *(UcPtrFil+4) , UsDatX03 );
	RegWriteA( *(UcPtrFil+6) , UcDatDss );
	RamWriteA( *(UcPtrFil+12) , UsDatAoe );
	
	UcPtrFil = UcPtrFil + 16;
	
	for( idx = UcStaAdr ; idx < UcEndAdr ; idx+=2 ){
		RamWriteA( idx, *UcPtrFil << 8 | *(UcPtrFil+1) );
		UcPtrFil += 2;
	}
	
	return UcPtrFil;
}
#endif



//********************************************************************************
// Function Name 	: IniSvo
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Servo ON
// History			: First edition 						2012.06.11 YS.Kim
//********************************************************************************
void	IniSvo( void )
{
	unsigned short	UsRedAof;
	
	//WitTim(5);
	
	RamReadA( 0x3C	,	&UsRedAof );
	RamWriteA( 0x18	,	UsRedAof );
	RamWriteA( 0x04	,	UsRedAof );
	
	WitTim(5);
	
	RegWriteA( 0x87, 0x85 );
}



//********************************************************************************
// Function Name 	: IniCmd
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Command Execute Process Initial
// History			: First edition 						2009.07.30 Y.Tashita
//********************************************************************************
void	IniCmd( void )
{

	MemClr( ( unsigned char * )&StAdjPar, sizeof( stAdjPar ) ) ;	// Adjust Parameter Clear
}



//********************************************************************************
// Function Name 	: MemClr
// Retun Value		: void
// Argment Value	: Clear Target Pointer, Clear Byte Number
// Explanation		: Memory Clear Function
// History			: First edition 						2009.07.30 Y.Tashita
//********************************************************************************
void	MemClr( unsigned char	*NcTgtPtr, unsigned short	UsClrSiz )
{
	unsigned short	UsClrIdx ;

	for ( UsClrIdx = 0 ; UsClrIdx < UsClrSiz ; UsClrIdx++ )
	{
		*NcTgtPtr	= 0 ;
		NcTgtPtr++ ;
	}
}



//********************************************************************************
// Function Name 	: WitTim
// Retun Value		: NON
// Argment Value	: Wait Time(ms)
// Explanation		: Timer Wait Function
// History			: First edition 						2009.07.31 Y.Tashita
//********************************************************************************
void	WitTim( unsigned short	UsWitTim )
{
	unsigned long	UlLopIdx, UlWitCyc ;

	UlWitCyc	= ( unsigned long )( ( float )UsWitTim / NOP_TIME / ( float )12 ) ;

	for( UlLopIdx = 0 ; UlLopIdx < UlWitCyc ; UlLopIdx++ )
	{
		;
	}
}
