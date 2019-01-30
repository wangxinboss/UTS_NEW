/**
*	@file
*	@brief	計測?イブ??ー							Ver 1.0.9.x
*/
/*============================================================================*/
#ifndef MEASUREMENT_LIBRARY_H_
#define MEASUREMENT_LIBRARY_H_


/*----------------------------------------------------------------------*/
/**
*	@brief	Mixing coefficient（mlCalMixCoef関?）用の入力値
*/
struct tagMlMixingValue
{
	double	radianX;
	double	radianY;

	double	hx45x;
	double	hy45x;
	double	hy45y;
	double	hx45y;

	UINT_8	hxsx;
	UINT_8	hysx;

	INT_32	hx45xL;		//! for Fixed point
	INT_32	hy45xL;		//! for Fixed point
	INT_32	hy45yL;		//! for Fixed point
	INT_32	hx45yL;		//! for Fixed point
};
/**
*	@brief	Mixing coefficient（mlCalMixCoef関?）用の入力値
*/
typedef	struct tagMlMixingValue		mlMixingValue;

/*----------------------------------------------------------------------*/
/**
*	@brief	Lineaity correction（mlCalLinearCorr関?）用の入力値
*/
struct tagMlLinearityValue
{
	INT_32	measurecount;	//! input parameter
	UINT_32	*dacX;			//! input parameter
	UINT_32	*dacY;			//! input parameter

	double	*positionX;
	double	*positionY;
	UINT_16	*thresholdX;
	UINT_16	*thresholdY;

	UINT_32	*coefAXL;		//! for Fixed point
	UINT_32	*coefBXL;		//! for Fixed point
	UINT_32	*coefAYL;		//! for Fixed point
	UINT_32	*coefBYL;		//! for Fixed point
};
/**
*	@brief	Linearity correction（mlCalLinearCorr関?）用の入力値
*/
typedef	struct tagMlLinearityValue		mlLinearityValue;


/*----------------------------------------------------------------------*/
/**
*	@brief	?イブ??ーエ?ーコード
*/
enum tagErrorCode
{
	/**! エ?ー無しで正常終了 */
	ML_OK,

	/**! ???不足????ー関連のエ?ー */
	ML_MEMORY_ERROR,
	/**! 引?指定のエ?ー */
	ML_ARGUMENT_ERROR,
	/**! 引?にNULLが指令されているエ?ー */
	ML_ARGUMENT_NULL_ERROR,

	/**! 指定されたディ?クト?が存在しないエ?ー */
	ML_DIRECTORY_NOT_EXIST_ERROR,
	/**! 画?ファイ?が存在しないエ?ー */
	ML_FILE_NOT_EXIST_ERROR,
	/**! ファイ?IOエ?ー */
	ML_FILE_IO_ERROR,
	/**! 未?出のマークが有り */
	ML_UNDETECTED_MARK_ERROR,
	/**! 同じ位置を示すマークが多重?出した */
	ML_MULTIPLEX_DETECTION_MARK_ERROR,
	/**! 必要なDLLが見つからないなど実行不可な状態 */
	ML_NOT_EXECUTABLE,

	/**! 未解析の画?が有りエ?ー */
	ML_THERE_UNANALYZED_IMAGE_ERROR,

	/**! 上記以外のエ?ー */
	ML_ERROR,
};

/**
*	@brief	?イブ??ーエ?ーコード
*/
typedef	enum tagErrorCode	mlErrorCode;

#endif /* #ifndef MEASUREMENT_LIBRARY_H_ */
