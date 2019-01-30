//********************************************************************************
//
//		<< LC898211 Evaluation Soft>>
//		Program Name	: Af.h
// 		Explanation		: LC8982119 Global Declaration & ProtType Declaration
//		Design			: Y.Yamada
//		History			: First edition						2009.07.30 Y.Tashita
//		History			: LC898211 changes					2012.06.11 YS.Kim
//********************************************************************************
#define	FW_VER			0x0000

#ifdef	AFINI
	#define	AFINI__
#else
	#define	AFINI__		extern
#endif

#ifdef	AFCMD
	#define	AFCMD__
#else
	#define	AFCMD__		extern
#endif



// Command Status
#define		EXE_END		0x02		// Execute End (Adjust OK)
#define		EXE_HADJ	0x06		// Adjust NG : Hall NG (Gain or Offset)
#define		EXE_LADJ	0x0A		// Adjust NG : X Loop NG (Gain)
#define		EXE_OCADJ	0x42		// Adjust NG : OSC Clock NG
#define		EXE_ERR		0x99		// Execute Error End

// Common Define
#define	SUCCESS			0x00		// Success
#define	FAILURE			0x01		// Failure

#ifndef ON
 #define	ON				0x01		// ON
 #define	OFF				0x00		// OFF
#endif

#define	NOP_TIME		0.00004166F



struct STHALREG {
	unsigned short	UsRegAdd ;
	unsigned char	UcRegDat ;
} ;													// Hall Register Data Table

struct STHALFIL {
	unsigned short	UsRamAdd ;
	unsigned short	UsRamDat ;
} ;													// Hall Filter Coefficient Table

struct STCMDTBL{
	unsigned short Cmd ;
	unsigned int UiCmdStf ;
	void ( *UcCmdPtr )( void ) ;
} ;

/*** caution [little-endian] ***/

// Word Data Union
union	WRDVAL{
	unsigned short	UsWrdVal ;
	unsigned char	UcWrkVal[ 2 ] ;
	struct {
		unsigned char	UcLowVal ;
		unsigned char	UcHigVal ;
	} StWrdVal ;
} ;

typedef union WRDVAL	UnWrdVal ;

union	DWDVAL {
	unsigned long	UlDwdVal ;
	unsigned short	UsDwdVal[ 2 ] ;
	struct {
		unsigned short	UsLowVal ;
		unsigned short	UsHigVal ;
	} StDwdVal ;
	struct {
		unsigned char	UcRamVa0 ;
		unsigned char	UcRamVa1 ;
		unsigned char	UcRamVa2 ;
		unsigned char	UcRamVa3 ;
	} StCdwVal ;
} ;

typedef union DWDVAL	UnDwdVal;

// Float Data Union
union	FLTVAL {
	float			SfFltVal ;
	unsigned long	UlLngVal ;
	unsigned short	UsDwdVal[ 2 ] ;
	struct {
		unsigned short	UsLowVal ;
		unsigned short	UsHigVal ;
	} StFltVal ;
} ;

typedef union FLTVAL	UnFltVal ;


typedef struct STADJPAR {
	struct {
		//Before
		unsigned short	UsHlbCen ;				// Hall Center Value after Hall Adjust
		unsigned short	UsHlbMax ;				// Hall Max Value
		unsigned short	UsHlbMin ;				// Hall Min Value
		unsigned short	UsHlbOff ;				// Hall Offset Value
		unsigned short	UsHlbBia ;				// Hall Gain Value
		unsigned short	UsAdbOff ;				// Hall A/D Offset Value
		
		//After
		unsigned short	UsHlaCen ;				// Hall Center Value after Hall Adjust
		unsigned short	UsHlaMax ;				// Hall Max Value
		unsigned short	UsHlaMin ;				// Hall Min Value
		unsigned short	UsHlaOff ;				// Hall Offset Value
		unsigned short	UsHlaBia ;				// Hall Gain Value
		unsigned short	UsAdaOff ;				// Hall A/D Offset Value
	} StHalAdj ;
	
	struct {
		unsigned short	UsLgbVal ;				// Loop Gain befre
		unsigned short	UsLgaVal ;				// Loop Gain after
	} StLopGan ;
	
	struct {
		unsigned char		UcOsbVal ;			// OSC value Before
		unsigned char		UcOsaVal ;			// OSC value After
	} StOscAdj ;
	
} stAdjPar ;

AFCMD__	stAdjPar	StAdjPar ;				// Execute Command Parameter

typedef struct STMVPAR {
	unsigned short	UsSmvSiz ;
	unsigned char	UcSmvItv ;
	unsigned char	UcSmvEnb ;
} stSmvPar ;

AFCMD__	stSmvPar	StSmvPar ;				// Execute Command Parameter

// Prottype Declation
AFINI__ void	IniSet( void ) ;													// Initial Top Function
AFINI__ void	WitTim( unsigned short ) ;											// Wait
AFINI__ void	MemClr( unsigned char *, unsigned short ) ;							// Memory Clear Function

AFCMD__ unsigned long	TnePtpAf( unsigned char, unsigned char ) ;
AFCMD__ unsigned long	TneOffAf( UnDwdVal ) ;
AFCMD__ unsigned long	TneBiaAf( UnDwdVal ) ;
AFCMD__ unsigned char	TneCenAf( void ) ;
AFCMD__ unsigned char	TneRunAf( void ) ;
AFCMD__ unsigned short	UsStpSiz ;

#define		PTP_DIR			1
#define		PTP_BEFORE		0
#define		PTP_AFTER		1


AFCMD__ void			Stmv211Set( stSmvPar ) ;						// Set STMV
AFCMD__ unsigned char	Stmv211( short ) ;
AFCMD__ unsigned char	Stmv211Fin( unsigned char ) ;
AFCMD__ short SsParStt;	// StepMove Start Position

// Convergence Judgement
#define INI_MSSET_211		(unsigned char)0x00						// Initialize Value For [8Fh]
#define CHTGX_THRESHOLD		(unsigned short)0x0200						// Convergence Judge Threshold
#define CHTGOKN_TIME		(unsigned char)0x80						// 64 Sampling Time 1.365msec( EQCLK=12MHz )
#define CHTGOKN_WAIT		3							// CHTGOKN_WAIT(3ms) > CHTGOKN_TIME(2.732msec) ( CHTGOKN_WAIT has to be longer than CHTGOKN_TIME)

// StepMove
#define STMV_SIZE			(unsigned short)0x0180						// StepSize(MS1Z12)
#define STMV_INTERVAL		(unsigned char)0x08						// Step Interval(STMVINT)

#define	STMCHTG_ON			(unsigned char)0x08						// STMVEN Register Set
#define STMSV_ON			(unsigned char)0x04
#define STMLFF_ON			(unsigned char)0x02 
#define STMVEN_ON			(unsigned char)0x01 
#define	STMCHTG_OFF			(unsigned char)0x00
#define STMSV_OFF			(unsigned char)0x00
#define STMLFF_OFF			(unsigned char)0x00
#define STMVEN_OFF			(unsigned char)0x00

#define	STMCHTG_SET			STMCHTG_ON					// Convergence Judgement On
#define STMSV_SET			STMSV_ON					// Setting Target Position = End Position
#define STMLFF_SET			STMLFF_OFF

