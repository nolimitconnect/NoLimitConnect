#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#ifdef __cplusplus

#include "VxFileInfo.h"
#include "VxFileTypeMasks.h"

#include <vector>

class VFile;
class VxKey;

namespace VxFileUtil
{

    const int64_t				SIZE_1GB = 1000000000;

	RCODE						getCurrentWorkingDirectory( std::string strRetDir );
	RCODE						setCurrentWorkingDirectory( const char* pDir );

	bool						isDotDotDirectory( const char* fileName );
	bool						isDotDotDirectory( const wchar_t * fileName );

    //! append file name to path.. account for url etc
	std::string					addFileToFolder( std::string& strFolder, std::string& strFile );

	bool						fileIsProviderFile( const char* fileName ); // return true if is an android content storage provider file
	//! returns file size or 0 if doesn't exist
	uint64_t					fileExists( const char* pFileName, bool printLogIfDoesNotExist = true );
	uint64_t					getFileLen( const char* pFileName, bool printLogIfDoesNotExist = true );
    bool						getFileTypeAndLength( const char* pFileName, uint64_t& retFileLen, uint8_t& retFileType, bool printLogIfDoesNotExist = true );
	bool						directoryExists( const char* pDir );

	//! Make all directories that don't exist in a given path
	RCODE						makeDirectory( const char* pDirectoryPath );
	//! Make all directories that don't exist in a given path
	RCODE						makeDirectory( std::string& strDirectoryPath );
    //! strip last directory in string
    std::string					moveUpADirectory( std::string& folderPath );
	void						assureTrailingDirectorySlash( std::string& strDirectoryPath );
	void						removeTrailingDirectorySlash( std::string& strDirectoryPath );
	std::string					makeUniqueFileName( const char* fileName );

	//! read a line from file into buffer and null terminate it
	RCODE						readLine( VFile *pgFile, char *pBuf, int iBufLen );

	VFile *						fileOpen( const char* pFileName, const char* pFileMode );
	RCODE						fileSeek ( VFile * poFile, uint32_t u32Pos );

	//! File seek..NOTE: only seeks from beginning of file
	RCODE						fileSeek ( VFile * poFile, uint64_t u64Pos );
	RCODE						copyFile( const char* pOldPath, const char* pNewPath );
	RCODE						deleteFile( const char* pFileName );
	RCODE						renameFile( const char* pFileOldName, const char* pFileNewName );

    //! copy all files and directories to destination directory
    RCODE						recursiveCopyDirectory( const char* pSrcDir, const char* pDestDir, int64_t& totalCopied );
    //! copy files to destination directory then delete the source files
	RCODE						moveFiles( char * pDestDir, char * pSrcDir );
	//! move a file from one directory to another
	RCODE						moveAFile( const char* srcFile, const char* destFile );

	//! separate Path and file name into separate strings
    RCODE						seperatePathAndFile(	const char*     pFullPath,		// path and file name
                                                        std::string&	strRetPath,		// return path to file
                                                        std::string&	strRetFile );	// return file name

	//! separate Path and file name into separate strings
    RCODE						seperatePathAndFile(	std::string&	strFullPath,	// path and file name
                                                        std::string&	strRetPath,		// return path to file
                                                        std::string&	strRetFile );	// return file name

	//! separate file name into file name and extension strings
    void						seperateFileNameAndExtension(	std::string&	fileNameWithExt,		// file name with extension
                                                                std::string&	strRetFileNamePart,		// return file name part without .ext
                                                                std::string&	strRetExtensionPart );	// return .ext part

	//! remove the path and return just the file name
    void						getJustFileName(	const char*     pFullPath,	                // file name may be full or just file name
													std::string&	strRetJustFileName );		// return file name
    //! remove the file name and return just the path
    std::string					getJustPath( std::string fullPath );	// file name and path

	//! get the . extension of file name
	void						getFileExtension(	std::string&	strFileName,	    // file name with extension
													std::string&	strRetExt );		// return extension ( ie "myfile.etm" would return etm"
	//! flip back slashes into forward slashes
	void						makeForwardSlashPath( std::string & csFilePath );
	//! flip back slashes into forward slashes
	void						makeForwardSlashPath( char * pFilePath );
	//! flip forward slashes into backward slashes
	void						makeBackwardSlashPath( std::string & csFilePath );
	//! flip forward slashes into backward slashes
	void						makeBackwardSlashPath( char * pFilePath );

	//! return true if last char is '/' else '\\'
	bool						doesPathEndWithSlash( const char* pFileName );
    //! append slash if needed
    void						assurePathEndWithSlash( std::string &csFileName );

	//! return true if is a root path like C:\dir or /dir
	bool						isFullPath( const char* pFileName );
	//! Make full path to execute directory if full path was not specified
	//! NOTE: be careful .. assumes pFileName has enough space for full path and file name
	void						makeFullPath( char * pFileName );
	//! Make full path to given directory if full path was not specified.. make path if does not exist
	void						makeFullPath( const char* pShortFileName, const char* pDownloadDir, std::string & strRetPath );
	//! Make short FileName.. if pDownloadDir and full path contains pDownloadDir then will be path in that dir else just filename
	//! return true if FullFileName contained the download directory
	bool 						makeShortFileName( const char* pFullFileName, std::string & strRetShortName, const char* pDownloadDir = NULL );

	//! Get execution full path
	RCODE						getExecuteFullPathAndName( std::string& strRetExePathAndFileName );
	//! Get directory we execute from
	RCODE						getExecuteDirectory( std::string& strRetExeDir );
	//! Get execution path and file name
	RCODE						getExecutePathAndName( std::string& strRetExeDir, std::string& strRetExeFileName );
	//match file names using dos style wild chars
	 bool						fileNameWildMatch(const char  * pMatchName, const char* pWildName);
	//! read whole file of known length into existing buffer
	//! NOTE assumes buffer has enough room for the whole file
    RCODE						readWholeFile(	const char*     pFileName,					// file to read
												void *			pvBuf,						// buffer to read into
												uint32_t		u32LenToRead,				// length to read ( assumes is same as file length
												uint32_t *		pu32RetAmountRead = NULL );	// return length actually read if not null
	//! allocate memory and read whole file into memory
    //! NOTE: USER MUST DELETE THE RETURED POINTER OR MEMORY LEAK WILL OCCUR
    RCODE						readWholeFile(	const char*     pFileName,			// file to read
												void **			ppvRetBuf,			// return allocated buffer it was read into
												uint32_t *		pu32RetLenOfFile );	// return length of file
	//! allocate memory and read whole file into memory and decrypt
    //! NOTE: USER MUST DELETE THE RETURED POINTER OR MEMORY LEAK WILL OCCUR
	RCODE						readWholeFile(	VxKey *			poKey,				// key to decrypt with
                                                const char*     pFileName,			// file to read
												void **			ppvRetBuf,			// return allocated buffer it was read into
												uint32_t *		pu32RetLenOfFile );	// return length of file

	//! write all of data to a file
    RCODE						writeWholeFile(	const char*     pFileName,			// file to write to
												void *			pvBuf,				// data to write
                                                uint32_t		u32LenOfData );		// data length
	//! encrypt and write all of data to a file
	RCODE						writeWholeFile(	VxKey *			poKey,				// key to encrypt with
                                                const char*     pFileName,			// file to write to
												void *			pvBuf,				// data to write
												uint32_t		u32LenOfData );		// data length

    RCODE						listFilesInDirectory(	const char*                 pSrcDir,
														std::vector<std::string>&	fileList );

    RCODE						listFilesAndFolders(	const char*                 pSrcDir,
														std::vector<VxFileInfo>&	fileList,
														uint8_t						fileFilterMask = VXFILE_TYPE_ANY | VXFILE_TYPE_DIRECTORY );

	bool						deleteFilesInFolder( std::string fileFolder, bool folderNameEndsWithOnlineId );

	uint8_t						fileExtensionToFileTypeFlag( const char* fileName );
	bool						incrementFileName( std::string& strFileName );

	bool						getDiskSpace( const char* filePath, uint64_t& totalDiskSpace, uint64_t& diskSpaceAvail );
    uint64_t					getDiskFreeSpace( const char* filePath );

    bool						u64ToHexAscii( uint64_t fileLen, std::string& retHexAscii );
    bool						hexAsciiToU64( const char* hexAscii, uint64_t& retFileLen );

    bool						dataToHexAscii( uint8_t * dataBuf, int dataLen, std::string& retHexAscii );
    bool						hexAsciiToData( const char* hexAscii, uint8_t * retDataBuf, int dataLen );

	uint8_t						charToHexBinary( char cVal );
	char						binaryToHexChar( uint8_t u8Val );

	std::string					makeKodiPath( const char* path );

    std::string					readVersionFile( std::string& versionFileName );

	//! return false if a test file cannot be written to the given path
	bool						testIsWritablePath( std::string writeablePath ); 

	std::string					describeDiskSpace( std::string pathOnDisk );

	std::string					describeFileSize( uint64_t fileLen );
} // namespace VxFileUtil

size_t FindLastPathSeperator( std::string& path );
bool   AddExtraLongPathPrefix( std::string& path );
bool   RemoveExtraLongPathPrefix( std::string& path );

#endif // __cplusplus

#ifdef TARGET_OS_WINDOWS

std::string WindowsRelativeToAbsolutePath( std::string& path );

#ifdef __cplusplus
extern "C" {
#endif
 bool WindowsRelativeToAbsolutePath( char * pathBuf,  int bufLen );
#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif // TARGET_OS_WINDOWS
