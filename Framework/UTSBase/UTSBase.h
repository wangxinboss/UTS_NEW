#pragma once

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (pop)
#endif

//------------------------------------------------------------------------------
// 内存泄露检查
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//------------------------------------------------------------------------------

#include <regex>
#include <afxcontrolbars.h>
#include <atlimage.h>
#include <atlconv.h>
#include <assert.h>
#include <afxmt.h>
#include <vector>
#include <stdint.h>
using namespace std;

#pragma comment(lib, "Version.lib")

//------------------------------------------------------------------------------
// disable warning
#pragma warning (disable:4482)  // warning C4482: 使用了非标准扩展: 限定名中使用了枚举
#pragma warning (disable:4996)  // warning C4996: '_wfopen': This function or variable may be unsafe.

namespace UTS
{

#define PI          (3.1415926535897932384626)
#define EMPTY_STR   _T("")

#ifdef UNICODE
#define UTS_FN  __FUNCTIONW__
#else
#define UTS_FN  __FUNCTION__
#endif
    //------------------------------------------------------------------------------
    // Color
#define COLOR_WHITE             RGB(255, 255, 255)  // 白
#define COLOR_BLACK             RGB(0  , 0  , 0  )  // 黑
#define COLOR_RED               RGB(255, 0  , 0  )  // 红
#define COLOR_LIME              RGB(0  , 255, 0  )  // 柠檬
#define COLOR_BLUE              RGB(0  , 0  , 255)  // 蓝
#define COLOR_YELLOW            RGB(255, 255, 0  )  // 黄
#define COLOR_ORANGE            RGB(255, 165, 0  )  // 橙
#define COLOR_GOLD              RGB(255, 215, 0  )  // 金 
#define COLOR_SILVER            RGB(192, 192, 192)  // 银白
#define COLOR_DARKGRAY          RGB(169, 169, 169)  // 深灰
#define COLOR_LIGHTGREY         RGB(211, 211, 211)  // 浅灰
#define COLOR_MEDIUMSEAGREEN    RGB(60 , 179, 113)  // 适中的海洋绿
#define COLOR_SEAGREEN          RGB(46 , 139, 87 )  // 海洋绿
#define COLOR_GREEN             COLOR_MEDIUMSEAGREEN


    //------------------------------------------------------------------------------
    // Macro Function
#define OFFSET_VALUE(src, k, b)         ((double)((double)(src) * (double)(k) + (double)(b)))
#define ROUND(a)                        (((a) >= (0.0f)) ? (int((a)+0.5f)):(int((a)-0.5f)))
#define DOUBLE2LONG(a)                  (((a) >= (0.0f)) ? (long((a)+0.5f)):(long((a)-0.5f)))
#define DOUBLE2INT(a)                   ROUND(a)
#define RELEASE_ARRAY(a)                (((a)!=0) ? (delete[] (a),(a) = 0):((a)=0))
#define RELEASE_POINTER(a)              (((a)!=0) ? (delete (a),(a) = 0):((a)=0))
#define SWAP(type, a, b)                {type temp;temp=(a);(a)=(b);(b)=temp;}
#define FLOAT_ZERO_THRESHOLD            (0.000001)
#define GET_VALUE_IN_RANGE(a, min, max) (((a) >= (max))?(max):(((a) <= (min))?(min):(a)))
#define Angle2Radian(angle)             ((angle) * PI / 180.0)
#define Radian2Angle(radian)            ((radian) * 180.0 / PI)
#define YVALUE(b, g, r)                 (0.299 * (r) + 0.587 * (g) + 0.114 * (b))
#define PIXEL_R(x, y, w, h)             ((((h)-(y)-1)*(w)+(x)) * 3 + 2)
#define PIXEL_G(x, y, w, h)             ((((h)-(y)-1)*(w)+(x)) * 3 + 1)
#define PIXEL_B(x, y, w, h)             ((((h)-(y)-1)*(w)+(x)) * 3 + 0)
#define MAKESURE_NOT_ZERO(a)            ((abs(a) < FLOAT_ZERO_THRESHOLD) ? FLOAT_ZERO_THRESHOLD : (a))

#ifndef GET_BITS
#define GET_BITS(field,offset,mask)     (((field) >> (offset)) & (mask))
#endif

#ifndef GET_BIT
#define GET_BIT(field,offset)           (GET_BITS(field,offset,0x1))
#endif

#ifndef IS_IN_RANGE
#define IS_IN_RANGE(a, mina, maxa)      ((mina) <= (a) && (a) <= (maxa))
#endif

#ifndef UTSMAX
#define UTSMAX(a,b)                     (((a) > (b)) ? (a) : (b))
#endif

#ifndef UTSMIN
#define UTSMIN(a,b)                     (((a) < (b)) ? (a) : (b))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)               (sizeof(array) / sizeof(*array))
#endif

    //------------------------------------------------------------------------------
    // Common Functions
    // "aaa, bbb, ccc" => Vector("aaa", "bbb", "ccc")
    void SplitString(LPCTSTR lpString, vector<CString> &vecString, LPCTSTR lpSplitMark = _T(","), BOOL bTrimResult = TRUE);
    // "1, 2, 3" => Vector(1, 2, 3)
    void SplitInt(LPCTSTR lpString, vector<int> &vecInt);
    // "0.1, 0.2, 0.3" => Vector(0.1, 0.2, 0.3)
    void SplitDouble(LPCTSTR lpString, vector<double> &vecDouble);
    // "=" * 4 => "===="
    CString GetTimesString(LPCTSTR lpString, unsigned int unTimes = 1);
    //------------------------------------------------------------------------------

    // Class
    template <typename T>
    class MinMax
    {
    public:
        T min;
        T max;
        inline bool IsCover(T x) { return IS_IN_RANGE(x, min, max); }
    };

    //------------------------------------------------------------------------------
    // 操作系统处理
    class OSUtil
    {
    public:
        static CString GetParentDir(LPCTSTR lpCurrentDir);
        static CString GetNoBSPath(LPCTSTR lpInputDir);
        static CString GetBSPath(LPCTSTR lpInputDir);
        static CString GetFileName(LPCTSTR lpPathFileName);
        static CString GetMainFileName(LPCTSTR lpFileName);
        static CString GetExtFileName(LPCTSTR lpFileName);
        static BOOL IsFileExist(LPCTSTR lpFileName);
        static BOOL IsDirExist(LPCTSTR lpDirName);
        static void CreateMultipleDirectory(LPCTSTR lpPath);
        static BOOL DirReplace(LPCTSTR lpInputDir, LPCTSTR lpSrcStr, LPCTSTR lpDestStr);
        static void GetFiles(LPCTSTR lpDir, CStringList &list);
        static void GetDirectorys(LPCTSTR lpDir, CStringList &list);
        static void MessageBoxLastError();
        static void ExcuteCommand(LPCTSTR lpCommand, BOOL bWaitSyncEnd = TRUE);
        static void ZipFile(LPCTSTR lpDestFileName, LPCTSTR lpSourceFileName);
        static BOOL GetFileVersion(LPCTSTR strFilePath, CString &strVersion);
        static BOOL GetFileTimeStr(LPCTSTR lpFilePathName, CString &strCreateTime, CString &strAccessTime, CString &strWriteTime);
        static void OpenFolderWindow(LPCTSTR lpDir);
        static BOOL IsDllExportFunction(LPCTSTR lpDllFile, LPCSTR lpFunctionName);
        static void ErrorExit(LPTSTR lpszFunction);
        static BOOL FindProcess(LPCTSTR lpProcessName);
        static long GetFileSize(LPCTSTR lpFileName);
    private:
        OSUtil(void);
        ~OSUtil(void);
    };

    //------------------------------------------------------------------------------
    // 高精度计时器
    class HighPrecisionTimer
    {
    public:
        HighPrecisionTimer(void);
        ~HighPrecisionTimer(void);

        // 记录采集开始时间
        int SetStartTime(void);
        // 获得经过的时间值(ms)
        double GetPassTime();
        // 终止采集时间
        void EndCollectTime();

    private:
        LONGLONG m_llCollectStartTime;
    };

    //------------------------------------------------------------------------------
    // 文件日志
    typedef enum _e_log_level_
    {
        LogLevel_Error = 0,
        LogLevel_Warning,
        LogLevel_Info,
        LogLevel_Debug
    } eLogLevel;

    class FileLog
    {
    public:
        FileLog();
        ~FileLog();

        BOOL Init(LPCTSTR lpFileName, int nLevel = LogLevel_Debug);
        void DebugLog(LPCTSTR lpFormat, ...);
        void InfoLog(LPCTSTR lpFormat, ...);
        void WarningLog(LPCTSTR lpFormat, ...);
        void ErrorLog(LPCTSTR lpFormat, ...);

    private:
        int m_nLevel;
        CString m_strLogFileName;
        CFile m_file;
        void FileLog::OutputLog(eLogLevel type, LPCTSTR lpText);
    };

    //------------------------------------------------------------------------------
    // 随机数
    class Random
    {
    public:
        Random();
        ~Random();

        double GetRandomDouble();               // 返回0-1之间的随机数
        int GetRandomInt(int nMax = RAND_MAX);  // 返回0-nMax之间的随机数
    };

    //------------------------------------------------------------------------------
    // 计算FPS
    class FPSCounter
    {
    public:
        FPSCounter();
        ~FPSCounter();
        double GetFPS(void);

    private:
        double dFPS;
        BOOL fps_first;
        LARGE_INTEGER start_time;
        LARGE_INTEGER end_time;
        LARGE_INTEGER freq;
        __int16	display_count;
    };

    //------------------------------------------------------------------------------
    // CColorListBox window
    typedef struct _LISTBOX_COLOR_
    {
        CString strText;
        COLORREF fgColor;
        COLORREF bgColor;
        _LISTBOX_COLOR_()
        {
            strText.Empty();
            fgColor = COLOR_BLACK;
            bgColor = COLOR_WHITE;
        }
    } LISTBOX_COLOR, *PLISTBOX_COLOR;

    class CColorListBox : public CListBox
    {
        // Construction
    public:
        CColorListBox();

        // Attributes
    public:
        void AppendString(LPCTSTR lpszText, COLORREF fgColor, COLORREF bgColor);
        // Operations
    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CColorListBox)
    public:
        virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
        virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        //}}AFX_VIRTUAL

        // Implementation
    public:
        virtual ~CColorListBox();

        // Generated message map functions
    protected:
        //{{AFX_MSG(CColorListBox)
        afx_msg void OnDestroy();
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

    //------------------------------------------------------------------------------
    class Resolution
    {
    public:
        void SetOldResolution(int nWidth, int nHeight);
        void SetNewResolution(int nWidth, int nHeight);
        int GetNewX(int nOldX);
        int GetNewY(int nOldY);
        int GetOldX(int nNewX);
        int GetOldY(int nNewY);
        int GetNewXByOldRatio(int nNewY);
        int GetNewYByOldRatio(int nNewX);
        int GetOldXByNewRatio(int nOldY);
        int GetOldYByNewRatio(int nOldX);

    private:
        int nOldX;
        int nOldY;
        int nNewX;
        int nNewY;
        double dScaleX_Old_New; // oldx / newx
        double dScaleY_Old_New; // oldy / newy
        double dOldRatioX_Y;    // oldx / oldy
        double dNewRatioX_Y;    // newx / newy
    };

    //-------------------------------------------------------------------------
    // 串口操作封装
    typedef struct _ComSeriesParam_
    {
        int nlenInput;  /*输入缓冲*/
        int nlenOutput; /*输出缓冲*/
        int nportNo;    /*串口号*/
        int nbaud;      /*波特率*/
        int nparity;    /*奇偶校验*/
        int ndatabits;  /*数据位*/
        int nstopbits;  /*停止位*/
    } COM_SERIES_PARAM;
    // 定义串口接收数据函数类型
    typedef void (CALLBACK* ONSERIESREAD)(CWnd*,BYTE* buf,int bufLen);
    // 串口通讯类
    class ComSeries
    {
    public:
        ComSeries(BOOL bAutoReceive=TRUE);
        ~ComSeries(void);
    public:
        //清空输入缓冲
        void PurgeInputBuffer();
        //清空输出缓冲
        void PurgeOutputBuffer();
        //设置串口读取、写入超时
        BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);
        //打开串口
        BOOL OpenPort(void* pOwner,     /*指向父指针*/
            int lenInput,              /*输入缓冲*/
            int lenOutput,             /*输出缓冲*/
            int portNo = 1,            /*串口号*/
            int baud = 9600,           /*波特率*/
            int parity = NOPARITY,     /*奇偶校验*/
            int databits = 8,          /*数据位*/
            int stopbits = ONESTOPBIT  /*停止位*/
            );
        //关闭串口
        void ClosePort();
        //同步读取数据
        int ReadSyncPort(BYTE* buf, DWORD bufLen, int nRetry=10);

		int ReadSyncPort(char* buf, DWORD bufLen, int nRetry=10);

        //同步写入数据
        BOOL WriteSyncPort(const BYTE *buf, DWORD bufLen);

		BOOL WriteSyncPort(const char *buf , DWORD bufLen);

        //得到串口是否打开
        BOOL GetComOpened();
    private:
        //串口读线程函数
        static DWORD WINAPI ReadThreadFunc(LPVOID lparam);
    private:
        //关闭读线程
        void CloseReadThread();
    private:
        //串口是否自动接收数据
        BOOL m_bAutoReceive;
        //已打开的串口句柄
        HANDLE m_hComm;
        //读线程句柄
        HANDLE m_hReadThread;
        //读线程ID标识
        DWORD m_dwReadThreadID;
        //读线程退出事件
        HANDLE m_hReadCloseEvent;
        //串口是否已打开
        BOOL m_bOpened; 
        //指定父对象指针
        void *m_pOwner; 
    public:
        ONSERIESREAD m_OnSeriesRead; //串口读取回调函数
    };
}


