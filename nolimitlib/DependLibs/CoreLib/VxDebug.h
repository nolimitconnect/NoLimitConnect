
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include "VxDefs.h"

#include <stdarg.h>

#define LOG_NONE    (0x0000)
#define LOG_VERBOSE (0x0001)
#define LOG_FATAL	(0x0002)
#define LOG_SEVERE	(0x0004)
#define LOG_ASSERT	(0x0008)
#define LOG_ERROR	(0x0010)
#define LOG_WARN	(0x0020)
#define LOG_DEBUG	(0x0040)
#define LOG_INFO	(0x0080)
#define LOG_STATUS	(0x0100)
#define LOG_PRIORITY_MASK	    0x000001ff

// defines so less work converting Linux code
#define LOG_WARNING		LOG_WARN
#define LOG_ERR			LOG_ERROR
#define LOG_CRIT		LOG_FATAL

typedef void ( *LOG_FUNCTION )( void *, uint32_t, const char * );

// protected by #ifdef __cplusplus so can be included in .c code
#ifdef __cplusplus

#include <string>
#include <vector>

enum ELogModule
{
    eLogNone = 0,

	eLogStartup			= 0x0001, // 1
	eLogThread          = 0x0002, // 2
	eLogStorage         = 0x0004, // 3
	eLogAssets			= 0x0008, // 4

	eLogUdp 		= 0x0010, // 5
	eLogConnect			= 0x0020, // 6 
	eLogAcceptConn		= 0x0040, // 7
	eLogRelay           = 0x0080, // 8

	eLogSkt				= 0x0100, // 9
	eLogSktData         = 0x0200, // 10
	eLogPkt				= 0x0400, // 11
	eLogUserEvent		= 0x0800, // 12 

	eLogNetAccessStatus = 0x1000, // 13
	eLogNetworkState	= 0x2000, // 14
	eLogNetworkMgr		= 0x4000, // 15
	eLogNetService		= 0x8000, // 16

	eLogIsPortOpenTest 	= 0x00010000, // 17
	eLogPortForward		= 0x00020000, // 18
	eLogHostConnect		= 0x00040000, // 19
	eLogRunTest			= 0x00080000, // 20

	eLogPlugins			= 0x00100000, // 21
	eLogHosts			= 0x00200000, // 22
	eLogClients			= 0x00400000, // 23
	eLogHostedUser		= 0x00800000, // 24

    eLogHostSearch      = 0x01000000, // 25
	eLogHackers			= 0x02000000, // 26
    eLogMediaStream     = 0x04000000, // 27
	eLogPlayerNlc		= 0x08000000, // 28

	eLogFfmpeg			= 0x10000000, // 29
	eLogAudioIo			= 0x20000000, // 30
	eLogVideoIo			= 0x40000000, // 31
	eLogMembership		= 0x80000000, // 31

    eMaxLogModule
};

void EnableLogTimer( bool enable );

void LogModule( ELogModule eLog, uint32_t u32MsgType, const char* msg, ... );
/// @brief no vargs version
void LogModule2( ELogModule eLog, uint32_t u32MsgType, const char* msg );

/// @brief return true if should log the given module
bool IsLogEnabled( ELogModule logModule );
/// @brief enable/disable the given module
void VxSetLogModuleEnable( ELogModule logModule, bool enable );

/// @brief enable/disable ALL logging
void VxSetDebugLoggingEnable( bool enableDebug );
/// @brief return true if ANY logging is allowed
bool VxGetDebugLoggingEnable( void );

class ILogCallbackInterface
{
public:
    virtual void                onLogEvent( uint32_t u32LogFlags, const char* logMsg ) = 0;
};

// add a log handler
void							VxAddLogHandler( ILogCallbackInterface * logHandler );
// remove a log handler
void							VxRemoveLogHandler( ILogCallbackInterface * logHandler );
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

// set logging priority mask or 0 to disable logging example  VxSetLogPriorityMask( LOG_VERBOSE ) or VxSetLogPriorityMask( LOG_PRIORITY_MASK )
void                            VxSetLogPriorityMask( uint32_t flags );
uint32_t                        VxGetLogPriorityMask( void );

// get module enable logging flags defined above
void                            VxSetModuleLogFlags( uint64_t flags );
uint64_t                        VxGetModuleLogFlags( void );

// enable log to file
void							VxSetLogToFile( const char* pFileName );
void							VxSetLogFlags( uint32_t u32LogFlags );

// enable/disable default log handler
void                            VxEnableDefaultLogHandler( bool enableDefaultHandler );

void							LogMsg( uint32_t u32MsgType, const char* msg, ...);

/// a version of LogModule that can be called from c code
void							LogCModule( int logModuleType, uint32_t u32MsgType, const char* msg, ...);

/// @brief no vargs version
void							LogMsg2( uint32_t u32MsgType, const char* msg );
void							LogMsgVarg(uint32_t u32MsgType, const char*fmt, va_list vargs);
void							VxHandleLogMsg(uint32_t u32MsgType, const char * msg);
void							HexDump( uint32_t u32MsgType, unsigned char* data, int dataLen, int instance, char * msg );
void							DumpInt8( uint32_t u32MsgType, int8_t* data, int dataLen, int instance, char * msg );
void							DumpInt16( uint32_t u32MsgType, int16_t* data, int dataLen, int instance, char * msg );
void							DumpFloat( uint32_t u32MsgType, float* data, int dataLen, int instance, char * msg );

// Convenience macros for logging
#define LogVerboseMsg(msg, ...) LogMsg(LOG_VERBOSE, (msg), __VA_ARGS__)
#define LogInfoMsg(msg, ...) LogMsg(LOG_VERBOSE, (msg), __VA_ARGS__)
#define LogDebugMsg(msg, ...) LogMsg(LOG_DEBUG, (msg), __VA_ARGS__)
#define LogWarnMsg(msg, ...) LogMsg(LOG_WARN, (msg), __VA_ARGS__)
#define LogErrorMsg(msg, ...) LogMsg(LOG_ERROR, (msg), __VA_ARGS__)
#define LogFatalMsg(msg, ...) LogMsg(LOG_FATAL, (msg), __VA_ARGS__)
#define LogStatusMsg(msg, ...) LogMsg(LOG_STATUS, (msg), __VA_ARGS__)


#if defined( _DEBUG ) || defined( DEBUG )
	/// This function is called by vx_assert() when the assertion fails.
	void  vx_error_output( uint32_t u32LogLevel, char* exp, char * file, int line);

	// debug mode throws message if expression is false
	#define vx_assert(exp) { if (!(exp)) vx_error_output(LOG_ASSERT,(char*)#exp,(char*)__FILE__,__LINE__); }
	#define vx_verify(exp) { if (!(exp)) vx_error_output(LOG_ASSERT,(char*)#exp,(char*)__FILE__,__LINE__); }
	#define vx_fatal(exp) { if (!(exp)) vx_error_output(LOG_FATAL,(char*)#exp,(char*)__FILE__,__LINE__); }
#else
	// release mode evaluates vx_assert ect to null
	#define vx_assert(exp)
	#define vx_verify(exp) (exp)
	#define vx_fatal(exp) (exp)
#endif // RELEASE_MODE


#ifdef __cplusplus
}
#endif


