//********************************************************************************
//
//		<< LC898211 Evaluation Soft >>
//	    Program Name	: AfCmd.c
//		Design			: Y.Yamada
//		History			: First edition						2009.07.31 Y.Tashita
//		History			: LC898211 changes					2012.06.11 YS.Kim
//********************************************************************************
//**************************
//	Include Header File		
//**************************
#define		AFCMD
#include	<stdio.h>
#include 	<stdlib.h>

#include	"Main.h"
#include	"Cmd.h"
#include	"Af.h"
#include	"AfDef.h"
#include "Io.h"
#include "spi.h"
#include "Timer.h"
//**************************
//	Local Function Prottype	
//**************************

//**************************
//	define					
//**************************

//Hall Adjustment
#define		TNE 			30								// Waiting Time For Movement
#define		OFFSET_DIV		2								// Divide Difference For Offset Step
#define		BIAS_ADJ_BORDER	0x15FF							// HALL_MAX_GAP < BIAS_ADJ_BORDER < HALL_MIN_GAP

#define		TIME_OUT		100
#define		MARGIN			0x0300							// Margin
#define		HALL_MAX_GAP	BIAS_ADJ_BORDER - MARGIN
#define		HALL_MIN_GAP	BIAS_ADJ_BORDER + MARGIN

//**************************
//	Global Variable			
//**************************
unsigned short	UsStpSiz	= 0 ;							// Bias Step Size

#define	LC898211_fs	23.4375
//**************************
//	Const					
//**************************

//********************************************************************************
// Function Name 	: TnePtpAf
// Retun Value		: Hall Top & Bottom Gaps
// Argment Value	: X,Y Direction, Adjust Before After Parameter
// Explanation		: Measuring Hall Paek To Peak
// History			: First edition 						2012.6.111 YS.Kim
//********************************************************************************
unsigned long TnePtpAf ( unsigned char	UcDirSel , unsigned char UcBfrAft )
{
	UnDwdVal			StTneVal;
	unsigned short		UsHigSmp[3], UsLowSmp[3];
	
	if(UcDirSel){
		RamWriteA( PIDZO_211H, 0x8001 );
	}else{
		RamWriteA( PIDZO_211H, 0x7fff );
	}
	
	WitTim( TNE ) ;	
	RamReadA( ADOFFSET_211H ,	&UsHigSmp[0] ) ;
	WitTim(5) ;	
	RamReadA( ADOFFSET_211H ,	&UsHigSmp[1] ) ;
	WitTim(5) ;	
	RamReadA( ADOFFSET_211H ,	&UsHigSmp[2] ) ;
	WitTim(5) ;	
	
	if( (signed short)UsHigSmp[0] < (signed short)0x8040){
		UsHigSmp[0] = 0x8040;
	}
	if( (signed short)UsHigSmp[1] < (signed short)0x8040){
		UsHigSmp[1] = 0x8040;
	}
	if( (signed short)UsHigSmp[2] < (signed short)0x8040){
		UsHigSmp[2] = 0x8040;
	}
	
	if(UcDirSel){
		RamWriteA( PIDZO_211H, 0x7fff );
	}else{
		RamWriteA( PIDZO_211H, 0x8001 );
	}
	
	WitTim( TNE ) ;	
	RamReadA( ADOFFSET_211H ,	&UsLowSmp[0] ) ;
	WitTim(5) ;	
	RamReadA( ADOFFSET_211H ,	&UsLowSmp[1] ) ;
	WitTim(5) ;	
	RamReadA( ADOFFSET_211H ,	&UsLowSmp[2] ) ;
	WitTim(5) ;
	
	if( (signed short)UsLowSmp[0] < (signed short)0x8040){
		UsLowSmp[0] = 0x8040;
	}
	if( (signed short)UsLowSmp[1] < (signed short)0x8040){
		UsLowSmp[1] = 0x8040;
	}
	if( (signed short)UsLowSmp[2] < (signed short)0x8040){
		UsLowSmp[2] = 0x8040;
	}
	
	StTneVal.StDwdVal.UsHigVal	=((unsigned long)UsHigSmp[0]+(unsigned long)UsHigSmp[1]+(unsigned long)UsHigSmp[2])/3;
	StTneVal.StDwdVal.UsLowVal	=((unsigned long)UsLowSmp[0]+(unsigned long)UsLowSmp[1]+(unsigned long)UsLowSmp[2])/3;
	
	if( UcBfrAft == 0 ) {
			StAdjPar.StHalAdj.UsHlbCen	= ( ( signed short )StTneVal.StDwdVal.UsHigVal + ( signed short )StTneVal.StDwdVal.UsLowVal ) / 2 ;
			StAdjPar.StHalAdj.UsHlbMax	= StTneVal.StDwdVal.UsHigVal ;
			StAdjPar.StHalAdj.UsHlbMin	= StTneVal.StDwdVal.UsLowVal ;
	} else {
			StAdjPar.StHalAdj.UsHlaCen	= ( ( signed short )StTneVal.StDwdVal.UsHigVal + ( signed short )StTneVal.StDwdVal.UsLowVal ) / 2 ;
			StAdjPar.StHalAdj.UsHlaMax	= StTneVal.StDwdVal.UsHigVal ;
			StAdjPar.StHalAdj.UsHlaMin	= StTneVal.StDwdVal.UsLowVal ;
	}
	
	if( (signed short)(StTneVal.StDwdVal.UsHigVal) <= 0 ){
		StTneVal.StDwdVal.UsHigVal = (unsigned short)0x7fff - (signed short)StTneVal.StDwdVal.UsHigVal;
	}else{
		StTneVal.StDwdVal.UsHigVal	= (unsigned short)0x7fff - StTneVal.StDwdVal.UsHigVal;
	}
	
	if( (signed short)(StTneVal.StDwdVal.UsLowVal) >= 0 ){
		StTneVal.StDwdVal.UsLowVal = (unsigned short)0x7fff + (signed short)StTneVal.StDwdVal.UsLowVal;
	}else{
		StTneVal.StDwdVal.UsLowVal	= StTneVal.StDwdVal.UsLowVal - (unsigned short)0x7fff;
	}
	
	return( StTneVal.UlDwdVal ) ;
}



//********************************************************************************
// Function Name 	: TneOffAf
// Retun Value		: Hall Top & Bottom Gaps
// Argment Value	: Hall Top & Bottom Gaps , X,Y Direction
// Explanation		: Hall Offset Tuning Function
// History			: First edition 						2012.6.11 YS.Kim
//********************************************************************************
unsigned long	TneOffAf( UnDwdVal StTneVal )
{
	signed long		SlSetOff ;
	unsigned short	UsSetOff ;
	
	RamReadA( DAHLXO_211H , &UsSetOff ) ;															// 0x0028	Offset Read
	SlSetOff	= (signed long)(UsSetOff & 0xFF00) ;
	
	SlSetOff	+= ( (signed long)( StTneVal.StDwdVal.UsHigVal - StTneVal.StDwdVal.UsLowVal ) / ( 0xDE << 1 ) ) << 8 ;	// Calculating Value For Increase Step
	
	if( SlSetOff > (signed long)0x0000FFFF ) {
		SlSetOff	= 0x0000FFFF ;
	} else if( SlSetOff < (signed long)0x00000000 ) {
		SlSetOff	= 0x00000000 ;
	}
	
	UsSetOff = (unsigned short)( ( SlSetOff & 0xFF00 ) | ( UsSetOff & 0x00FF ) );
	RamWriteA( DAHLXO_211H , UsSetOff ) ;															// 0x0028	Offset Write
	
	StTneVal.UlDwdVal	= TnePtpAf( PTP_DIR , PTP_AFTER ) ;
	
	return( StTneVal.UlDwdVal ) ;
}



//********************************************************************************
// Function Name 	: TneBiaAf
// Retun Value		: Hall Top & Bottom Gaps
// Argment Value	: Hall Top & Bottom Gaps , X,Y Direction
// Explanation		: Hall Bias Tuning Function
// History			: First edition 						2012.6.11 YS.Kim
//********************************************************************************
unsigned long TneBiaAf( UnDwdVal StTneVal )
{
	signed long		SlSetBia, UlSumAvr;
	unsigned short	UsSetBia ;
	
	UlSumAvr =	(unsigned long)( StTneVal.StDwdVal.UsHigVal + StTneVal.StDwdVal.UsLowVal ) / 2;
	
	RamReadA( DAHLXO_211H ,	&UsSetBia );
	SlSetBia	= (signed long)UsSetBia & 0xFF;
	
	UsStpSiz	= UsStpSiz >> 1 ;																// From 0x100	// Calculatiton For Hall BIAS 1/2 Searching
	
	if ( UlSumAvr > BIAS_ADJ_BORDER ){		
		if( ( SlSetBia + UsStpSiz ) < (signed short)0x0100 ){
			SlSetBia +=	UsStpSiz ;
		}
	} else {
		if( ( SlSetBia - UsStpSiz ) >= (signed short)0x0000 ){
			SlSetBia -=	UsStpSiz ;
		}
	}
	
	UsSetBia = (unsigned short)( ( SlSetBia & 0x00FF ) | ( UsSetBia & 0xFF00 ) ) ;
	RamWriteA( DAHLXO_211H,	UsSetBia );
	
	StTneVal.UlDwdVal	= TnePtpAf( PTP_DIR, PTP_AFTER );
	
	return( StTneVal.UlDwdVal );
}



//********************************************************************************
// Function Name 	: TneCenAf
// Retun Value		: Hall Center Tuning Result
// Argment Value	: X,Y Direction, Hall Top & Bottom Gaps
// Explanation		: Hall Center Tuning Function
// History			: First edition 						2012.6.11 YS.Kim
//********************************************************************************
unsigned char	TneCenAf( void )
{
	UnDwdVal		StTneVal;
	unsigned char 	UcTneRst, UcTofRst, UcTmeOut;
	unsigned short	UsOffDif;
	
	UsStpSiz	= 0x0100 ;
	UcTmeOut	= 1 ;
	UcTneRst	= FAILURE ;
	UcTofRst	= FAILURE ;
	
	StTneVal.UlDwdVal	= TnePtpAf( PTP_DIR, PTP_AFTER );	
	StTneVal.UlDwdVal	= TnePtpAf( PTP_DIR, PTP_BEFORE );
	
	while( UcTneRst && UcTmeOut )
	{
		if( UcTofRst == FAILURE ){
			StTneVal.UlDwdVal	= TneOffAf( StTneVal ) ;
		}else{
			StTneVal.UlDwdVal	= TneBiaAf( StTneVal ) ;
			UcTofRst	= FAILURE ;
		}
		
		UsOffDif	= abs( StTneVal.StDwdVal.UsHigVal - StTneVal.StDwdVal.UsLowVal ) / 2;					// Check Offset Tuning Result
		
		if( UsOffDif < MARGIN ){
			UcTofRst	= SUCCESS ;
		}else{
			UcTofRst	= FAILURE ;
		}
		
		if( ( StTneVal.StDwdVal.UsHigVal < HALL_MIN_GAP && StTneVal.StDwdVal.UsLowVal < HALL_MIN_GAP )		// Check Tuning Result 
		&&  ( StTneVal.StDwdVal.UsHigVal > HALL_MAX_GAP && StTneVal.StDwdVal.UsLowVal > HALL_MAX_GAP ) ){
			UcTneRst	= SUCCESS ;
			break ;
		}else if( UsStpSiz == 0 ){
			UcTneRst	= SUCCESS ;
			break ;
		}else{
			UcTneRst	= FAILURE ;
			UcTmeOut++ ;
		}
		
		if ( UcTmeOut == TIME_OUT ){
			UcTmeOut	= 0 ;
		}		 																							// Set Time Out Count
	}
	
	if( UcTneRst == FAILURE ){
		UcTneRst	= EXE_HADJ ;
	}else{
		UcTneRst	= EXE_END ;
	}
	
	return( UcTneRst ) ;
}



//********************************************************************************
// Function Name 	: TneRunAf
// Retun Value		: Hall Tuning SUCCESS or FAILURE
// Argment Value	: NON
// Explanation		: Hall System Auto Adjustment Function
// History			: First edition 						2012.06.12 YS.Kim
//********************************************************************************
unsigned char	TneRunAf( void ){
	unsigned char	UcHalSts ;
	unsigned char	UcTmpEnb;
	unsigned short	UsTmpHxb;
	
	RamReadA( DAHLXO_211H ,	&UsTmpHxb ) ;
	StAdjPar.StHalAdj.UsHlbOff	= UsTmpHxb >> 8 ;
	StAdjPar.StHalAdj.UsHlbBia	= UsTmpHxb & 0x00FF ;
	RamReadA( gain1_211H, &StAdjPar.StLopGan.UsLgbVal ) ;
	RamReadA( OFFSET_211H, &StAdjPar.StHalAdj.UsAdbOff ) ;
	
	
	RegReadA( 0x87 ,	&UcTmpEnb );
	RegWriteA( 0x87 ,	0x00 );
	
	UcHalSts	= TneCenAf() ;
	
	RegWriteA( 0x87 ,	UcTmpEnb );
	
	RamReadA( DAHLXO_211H ,	&UsTmpHxb ) ;
	StAdjPar.StHalAdj.UsHlaOff	= UsTmpHxb >> 8 ;
	StAdjPar.StHalAdj.UsHlaBia	= UsTmpHxb & 0x00FF ;
	RamReadA( gain1_211H, &StAdjPar.StLopGan.UsLgaVal ) ;
	RamReadA( OFFSET_211H, &StAdjPar.StHalAdj.UsAdaOff ) ;
	
	return UcHalSts;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------
    Function Name   : Stmv211Set, Stmv211, Stmv211Fin
	Description     : StepMove Setting, Execute, Finish, Current Limit 
    Arguments       : Stepmove Mode Parameter
	Return			: Stepmove Mode Parameter
-----------------------------------------------------------*/
//Stmv211Set -> Stmv211 -> Stmv21Fin -> Stmv211 -> Stmv211Fin ->ÅEÅEÅE

//********************************************************************************
// Function Name 	: Stmv211Set
// Retun Value		: NON
// Argment Value	: NON
// Explanation		: Stpmove Setting Function
// History			: First edition 						2012.06.12 YS.Kim
//********************************************************************************
void Stmv211Set( stSmvPar StSetSmv )
{
	unsigned char	UcSetEnb;
	unsigned char	UcSetSwt;
	unsigned short	UsParSiz;
	unsigned char	UcParItv;
	
	RegWriteA( AFSEND_211	, 0x00 );										// StepMove Enable Bit Clear
	
	RegReadA( ENBL_211 ,	&UcSetEnb );
	UcSetEnb 	&= (unsigned char)0xFD ;
	RegWriteA( ENBL_211	,	UcSetEnb );										// Measuremenet Circuit1 Off
	
	RegReadA( SWTCH_211 ,	&UcSetSwt );
	UcSetSwt	&= (unsigned char)0x7F ;
	RegWriteA( SWTCH_211 , UcSetSwt );										// RZ1 Switch Cut Off
	
	RamReadA( RZ_211H ,	&SsParStt );										// Get Start Position
	UsParSiz	= StSetSmv.UsSmvSiz ;										// Get StepSize
	UcParItv	= StSetSmv.UcSmvItv ;										// Get StepInterval
	
	RamWriteA( ms11a_211H	, (unsigned short)0x0800 );						// Set Coefficient Value For StepMove
	RamWriteA( MS1Z22_211H	, (unsigned short)SsParStt );					// Set Start Positon
	RamWriteA( MS1Z12_211H	, UsParSiz );									// Set StepSize
	RegWriteA( STMINT_211	, UcParItv );									// Set StepInterval
	
	UcSetSwt	|= (unsigned char)0x80;
	RegWriteA( SWTCH_211, UcSetSwt );										// RZ1 Switch ON
}



//********************************************************************************
// Function Name 	: Stmv211
// Retun Value		: Stepmove Parameter
// Argment Value	: Stepmove Parameter, Target Position
// Explanation		: Stpmove Function
// History			: First edition 						2012.06.12 YS.Kim
//********************************************************************************
unsigned char Stmv211( short SsSmvEnd )
{
	unsigned short	UsSmvTim;
	unsigned short	UsSmvDpl;
	
	//PIOA_SetOutput(_PIO_PA29);													// Monitor I/O Port
	
	RamReadA( RZ_211H ,	&SsParStt );											// Get Start Position
	UsSmvDpl = abs( SsParStt - SsSmvEnd );
	
	if( ( UsSmvDpl <= StSmvPar.UsSmvSiz ) && (( StSmvPar.UcSmvEnb & STMSV_ON ) == STMSV_ON ) ){
		if( StSmvPar.UcSmvEnb & STMCHTG_ON ){
			RegWriteA( MSSET_211	, INI_MSSET_211 | (unsigned char)0x01 );
		}
		RamWriteA( MS1Z22_211H, SsSmvEnd );										// Handling Single Step For ES1
		StSmvPar.UcSmvEnb	|= STMVEN_ON;										// Combine StepMove Enable Bit & StepMove Mode Bit
	} else {
		if( SsParStt < SsSmvEnd ){												// Check StepMove Direction
			RamWriteA( MS1Z12_211H	, StSmvPar.UsSmvSiz );
		} else if( SsParStt > SsSmvEnd ){
			RamWriteA( MS1Z12_211H	, -StSmvPar.UsSmvSiz );
		}
		
		RamWriteA( STMVENDH_211, SsSmvEnd );									// Set StepMove Target Positon
		StSmvPar.UcSmvEnb	|= STMVEN_ON;										// Combine StepMove Enable Bit & StepMove Mode Bit
		RegWriteA( STMVEN_211	, StSmvPar.UcSmvEnb );							// Start StepMove
	}
	
	UsSmvTim=(UsSmvDpl/STMV_SIZE)*((STMV_INTERVAL+1) / LC898211_fs);			// Stepmove Operation time
	WitTim( UsSmvTim );
	//TRACE("STMV Operation Time = %d \n", UsSmvTim ) ;
	
	return Stmv211Fin( StSmvPar.UcSmvEnb );
}



//********************************************************************************
// Function Name 	: Stmv211Fin
// Retun Value		: Stepmove Parameter
// Argment Value	: Stepmove Parameter
// Explanation		: Stpmove Finish Check Function
// History			: First edition 						2012.06.12 YS.Kim
//********************************************************************************
unsigned char Stmv211Fin( unsigned char UcParMod )
{
	unsigned char	UcChtGst;
	
	while( UcParMod & (unsigned char)STMVEN_ON )								// Wait StepMove operation end
	{
		RegReadA( STMVEN_211 , &UcParMod );
	}
	
	TimerStart(TIMER_2, CHTGOKN_WAIT);
	
	if( ( UcParMod & (unsigned char)0x08 ) == (unsigned char)STMCHTG_ON ){		// If Convergence Judgement is Enabled
		RegReadA( MSSET_211, &UcChtGst ) ;
		while( (UcChtGst & 0x01) && !(TimerUpCounter(TIMER_2)))					// Check Settling condition for [CHTGOKN_WAIT]ms
		{
			RegReadA( MSSET_211, &UcChtGst ) ;
		}
	}
	
	if( UcChtGst & 0x01 ){
		UcParMod	|= (unsigned char)0x80 ;									// STMV Success But Settling Time Over
		//PIOA_ClearOutput(_PIO_PA29);											// Monitor I/O Port
	}else{
		UcParMod	&= (unsigned char)0x7F ;									// STMV Success 
	}
	
	return UcParMod;															// Bit0:0 Successful convergence Bit0:1 Time Over
}


