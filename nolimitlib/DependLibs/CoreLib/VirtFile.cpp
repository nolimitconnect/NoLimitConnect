//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VirtFile.h"

#include "VxDebug.h"


class VirtFile
{
public:
	VirtFile();


protected:
	//=== vars ===//

};


//============================================================================
VirtFile::VirtFile()
{ 
}

extern "C" {

	VFile* VFileOpen( const char* fileName, const char* fileMode )
	{
		FILE* fp = fopen( fileName, fileMode );
		if( fp )
		{
			return new VFile( fp );
		}

		return nullptr;
	}

	int VFileClose( VFile* fp )
	{
		int result = fclose( fp->m_FILE );
		delete fp;
		return result;
	}

	int VFileEof( VFile* fp )
	{
		int result = feof( fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFileError( VFile* fp )
	{
		return fp->m_Error;
	}

	int VFileFlush( VFile* fp )
	{
		int result = fflush( fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	size_t VFileRead( void* buf, size_t size, size_t count, VFile* fp )
	{
		int result = fread( buf, size, count, fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}


	size_t VFileWrite( const void* buf, size_t size, size_t count, VFile* fp )
	{
		int result = fwrite( buf, size, count, fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFileGetC( VFile* fp )
	{
		int result = fgetc( fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	char* VFileGetS( char* buf, int size, VFile* fp )
	{
		char* result = fgets( buf, size, fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFilePutC( int ch, VFile* fp )
	{
		int result = fputc( ch, fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFilePutS( const char* s, VFile* fp )
	{
		int result = fputs( s, fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFileGetPos( VFile* fp, fpos_t* pos )
	{
		int result = fgetpos( fp->m_FILE, pos );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFileSetPos( VFile* fp, const fpos_t* pos )
	{
		int result = fsetpos( fp->m_FILE, pos );
		fp->m_Error = VxGetLastError();
		return result;
	}

	int VFileSeek( VFile* fp, size_t offset, int whence )
	{
		int result = fseek( fp->m_FILE, offset, whence );
		fp->m_Error = VxGetLastError();
		return result;
	}

	fpos_t VFileTell( VFile* fp )
	{
		fpos_t result = ftell( fp->m_FILE );
		fp->m_Error = VxGetLastError();
		return result;
	}

};