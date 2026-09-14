#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.org
//============================================================================
//
// A minimal <unistd.h> for MSVC, which has no POSIX layer of its own.
//
// This used to redirect to libgnu (gnulib). gnulib is a mixture, and 734 of its
// source files are GPL-only rather than LGPL -- a large licence liability for
// the sake of a handful of functions. Written from the POSIX names and their
// documented MSVC equivalents; it shares no code with gnulib.
//
// Only what is actually used is provided. Add to it when something needs more,
// rather than trying to be a complete POSIX emulation -- that is what pulling
// in gnulib was.
//
//============================================================================

#ifdef _MSC_VER

# include <io.h>          // _close, _read, _write, _dup, _isatty, _access, _lseek
# include <process.h>     // _getpid
# include <direct.h>      // _chdir, _getcwd, _rmdir
# include <stdlib.h>

// MSVC spells the POSIX file functions with a leading underscore. These are the
// same functions, not reimplementations.
//
// Deliberately inline functions rather than macros. As macros these names also
// rewrite struct members and designated initialisers -- ffmpeg has a
// RTPDynamicProtocolHandler::close, and ".close = x" becomes "._close = x",
// which fails to compile a long way from here. A function only matches a call.
//
// Each is guarded by #ifndef: ffmpeg's libavformat/os_support.h macro-defines
// several of these names itself (access, lseek, unlink, rmdir). Whoever gets
// there first wins, and we never redefine a name someone else has claimed.
# ifdef __cplusplus
#  define NLC_UNISTD_INLINE inline
# else
#  define NLC_UNISTD_INLINE static __inline
# endif

# ifndef close
NLC_UNISTD_INLINE int close( int fileDescriptor )                       { return _close( fileDescriptor ); }
# endif
# ifndef read
NLC_UNISTD_INLINE int read( int fileDescriptor, void* buf, unsigned int count )        { return _read( fileDescriptor, buf, count ); }
# endif
# ifndef write
NLC_UNISTD_INLINE int write( int fileDescriptor, const void* buf, unsigned int count ) { return _write( fileDescriptor, buf, count ); }
# endif
# ifndef dup
NLC_UNISTD_INLINE int dup( int fileDescriptor )                         { return _dup( fileDescriptor ); }
# endif
# ifndef dup2
NLC_UNISTD_INLINE int dup2( int fileDescriptor1, int fileDescriptor2 )  { return _dup2( fileDescriptor1, fileDescriptor2 ); }
# endif
# ifndef isatty
NLC_UNISTD_INLINE int isatty( int fileDescriptor )                      { return _isatty( fileDescriptor ); }
# endif
# ifndef access
NLC_UNISTD_INLINE int access( const char* path, int mode )              { return _access( path, mode ); }
# endif
# ifndef lseek
NLC_UNISTD_INLINE long lseek( int fileDescriptor, long offset, int origin ) { return _lseek( fileDescriptor, offset, origin ); }
# endif
# ifndef unlink
NLC_UNISTD_INLINE int unlink( const char* path )                        { return _unlink( path ); }
# endif
# ifndef getpid
NLC_UNISTD_INLINE int getpid( void )                                    { return _getpid(); }
# endif
# ifndef chdir
NLC_UNISTD_INLINE int chdir( const char* path )                         { return _chdir( path ); }
# endif
# ifndef getcwd
NLC_UNISTD_INLINE char* getcwd( char* buf, int size )                   { return _getcwd( buf, size ); }
# endif
# ifndef rmdir
NLC_UNISTD_INLINE int rmdir( const char* path )                         { return _rmdir( path ); }
# endif

// access() mode bits. MSVC defines the values but not the POSIX names.
# ifndef F_OK
#  define F_OK 0
# endif
# ifndef X_OK
#  define X_OK 0   // Windows has no execute bit; existence is the closest thing
# endif
# ifndef W_OK
#  define W_OK 2
# endif
# ifndef R_OK
#  define R_OK 4
# endif

// Standard descriptor numbers.
# ifndef STDIN_FILENO
#  define STDIN_FILENO  0
# endif
# ifndef STDOUT_FILENO
#  define STDOUT_FILENO 1
# endif
# ifndef STDERR_FILENO
#  define STDERR_FILENO 2
# endif

// usleep() has no MSVC equivalent. Sleep() takes milliseconds and rounds up, so
// a sub-millisecond request yields rather than busy-waiting -- which is the
// behaviour callers of usleep(0) actually want.
# ifdef __cplusplus
#  include <chrono>
#  include <thread>
inline int usleep( unsigned long microSeconds )
{
    std::this_thread::sleep_for( std::chrono::microseconds( microSeconds ) );
    return 0;
}

inline unsigned int sleep( unsigned int seconds )
{
    std::this_thread::sleep_for( std::chrono::seconds( seconds ) );
    return 0;
}
# else
#  include <windows.h>
static __inline int usleep( unsigned long microSeconds )
{
    Sleep( (DWORD)( ( microSeconds + 999 ) / 1000 ) );
    return 0;
}

static __inline unsigned int sleep( unsigned int seconds )
{
    Sleep( (DWORD)seconds * 1000 );
    return 0;
}
# endif // __cplusplus

#else

# include_next <unistd.h>

#endif // _MSC_VER
