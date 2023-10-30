#pragma  once 
#define		WIN_OS					1

#ifdef WIN_OS
#define _WINSOCKAPI_
#include <windows.h>

//#include <WinSock2.h>

/*
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"

#ifdef D64
#pragma comment(lib, "GLOGLIB/D64/libglog_static.lib")
#elif defined(R64)
#pragma comment(lib, "GLOGLIB/R64/libglog_static.lib")
#elif defined(D32)
#pragma comment(lib, "GLOGLIB/D32/libglog_static.lib")
#elif defined(R32)
#pragma comment(lib, "GLOGLIB/R32/libglog_static.lib")
#endif
*/


#elif define LINUX_OS

#endif

#define		BROADCAST_DEFAULT_PORT   3041


#define		PROGRAM_MAJOR_VERSION	1
#define		PROGRAM_MINOR_VERSION	0

#define		DEFAULT_PORT			"2077"

#define		MAX_PACKET_SIZE					1000000

#define		PARAM_START_OFFSET		16


#define		BUFFER_LENGTH			512
#define		DIR_OUT					1
#define		DIR_IN					0

#define		MAX_PATH_LENGTH			256

#define		CALL_NOT_ANSWERED		0
#define		CALL_ANSWERED			1

#define		RETENTION_MAX_CNTR		360




//#define	LINUX_OS				1




#define		MAX_CHANNEL_NUMBER		4


#define		SUCCESS							 0
#define		FAILURE							-1
#define		FILE_NOT_FOUND					-2
#define		DLL_FUNCTION_ERROR				-3
#define		USB_TIMEOUT						-4
#define		WAVE_FILE_TIME_LIMIT			-5
#define		USER_DELETE_NUMBER_ERROR		-6


#define		NAMED_MUTEX_CONSTANT_TEXT	"pocoWebServer1.00"


#define		SETTING_FILE_NAME			"setting.db"

#define SAMPLE_RATE					8000
#define BYTES_PER_SAMPLE			2
#define WAVE_HEADER_SIZE			44
#define WAVE_FILE_LIMIT				(20*60*SAMPLE_RATE*BYTES_PER_SAMPLE)+WAVE_HEADER_SIZE

typedef unsigned char				u8;
typedef unsigned short				u16;
typedef unsigned int				u32;
typedef unsigned long long			u64;

#define		MAX_STR					256
#define		MAX_VOICE				1024

union params_t
{
	int					state;
	char				str	[MAX_STR];
	u8					vbuf[MAX_VOICE];
};

struct networkSendPacket
{
	/*
	u32			preambleA;
	u32			preambleB;
	*/
	u32			serialNo;
	u32			opcode;
	u32			resp;
	u32			paramLen;
	params_t	param;
};

struct query_t
{
	u32     seqID;
	u32		numberOfFrames;
	u32		startTimeUTC;// it is file name.
	u32   	videoType;
	u32		framesPerSecond;
	u32		seqType;
};



#define		RING_THRESHOLD			64
#define		ONHOOK_THRESHOD			16
#define		OFFHOOK_THRESHOLD		16

#define LINE_OFFHOOK			128
#define LINE_ONHOOK				129
#define LINE_RINGING			130
#define LINE_DISCONNECT			131
#define LINE_DTMF				132
#define LINE_CALLERID			133

#define USB_CONNECTED			144
#define	USB_DISCONNECTED		145



#define	DRIVE_FREE_MARGIN		100*1024*1024


#define  BLACK			0
#define  DARKBLUE		FOREGROUND_BLUE
#define  DARKGREEN		FOREGROUND_GREEN
#define  DARKCYAN		FOREGROUND_GREEN | FOREGROUND_BLUE
#define  DARKRED		FOREGROUND_RED
#define  DARKMAGENTA	FOREGROUND_RED | FOREGROUND_BLUE
#define  DARKYELLOW		FOREGROUND_RED | FOREGROUND_GREEN
#define  DARKGRAY		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define  GRAY			FOREGROUND_INTENSITY
#define  BLUE			FOREGROUND_INTENSITY | FOREGROUND_BLUE
#define  GREEN			FOREGROUND_INTENSITY | FOREGROUND_GREEN
#define  CYAN			FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE
#define  RED			FOREGROUND_INTENSITY | FOREGROUND_RED
#define  MAGENTA		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE
#define  YELLOW			FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
#define  WHITE			FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE


#define		COMMAND_LENGTH					256


#define		ALIVE							128	
#define		SEND_CHANNEL_STATE				129
#define		SEND_CHANNEL_CID				130
#define		SEND_CHANNEL_DIAL				131
#define		SEND_CHANNEL_VOICE				132

enum rec_state_t
{
	REC_IDLE = 0,
	REC_RECORDING_IN = 1,
	REC_RECORDING_OUT = 2,
	REC_WAITING = 3,
	REC_DISCONNECT = 4,
	REC_UNKNOWN = 5,
	REC_DISABLED = 6
};


int color_printf(int color, char* string, ...);
int GetProgramVersionInfo(int &maj,int &min, int &build , int &revision);
void gregorian_to_jalali(/* output */ int *j_y, int *j_m, int *j_d,/*  input */ int  g_y, int  g_m, int  g_d);
void jalali_to_gregorian(/* output */ int *g_y, int *g_m, int *g_d,/*  input */ int  j_y, int  j_m, int  j_d);




