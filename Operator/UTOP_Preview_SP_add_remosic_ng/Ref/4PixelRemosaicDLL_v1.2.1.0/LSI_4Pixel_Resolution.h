
#ifndef __DEFINE_LSI_4PIXEL_RESOLUTION_API__
#define __DEFINE_LSI_4PIXEL_RESOLUTION_API__


#define __LSI_4PIXEL_API__	__declspec(dllimport)

// For Resolution Normal Mode
extern "C" __LSI_4PIXEL_API__ int Resolution_Process_Normal_Mode(
	unsigned short* pInputBuffer,
	int iWidth,
	int iHeight,
	int roiCount,
	unsigned short** ppOutputROIBufferArray,
	int* pROIStartX,
	int* pROIStartY,
	int* pROIEndX,
	int* pROIEndY,
	const char* setting_file_name,
	int bayer_order);


extern "C" __LSI_4PIXEL_API__ int Resolution_Process_Normal_Mode_OverwriteROI(
	unsigned short* pInputBuffer,
	unsigned short* pOutputBuffer,
	int iWidth,
	int iHeight,
	int roiCount,
	int* pROIStartX,
	int* pROIStartY,
	int* pROIEndX,
	int* pROIEndY,
	const char* setting_file_name,
	int bayer_order);

// For Resolution Fast Mode
extern "C" __LSI_4PIXEL_API__ int Resolution_Process_Fast_Mode(
	unsigned short* pInputBuffer,
	int iWidth,
	int iHeight,
	int roiCount,
	unsigned short** ppOutputROIBufferArray,
	int* pROIStartX,
	int* pROIStartY,
	int* pROIEndX,
	int* pROIEndY,
	const char* setting_file_name,
	int bayer_order);


extern "C" __LSI_4PIXEL_API__ int Resolution_Process_Fast_Mode_OverwriteROI(
	unsigned short* pInputBuffer,
	unsigned short* pOutputBuffer,
	int iWidth,
	int iHeight,
	int roiCount,
	int* pROIStartX,
	int* pROIStartY,
	int* pROIEndX,
	int* pROIEndY,
	const char* setting_file_name,
	int bayer_order);

#endif // __DEFINE_LSI_4PIXEL_RESOLUTION_API__
