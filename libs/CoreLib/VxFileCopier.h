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

#include "VxFileUtil.h"

class VxFileCopier
{
public:
	bool m_bAbort;

	void Abort( void ){ m_bAbort = true; }

	//find files matching the extension (file type) list
    RCODE                       copyDirectory(  std::string					curPath,                            // current file or path to copy
                                                std::string&				srcPath,						    // src path to copy from
                                                std::string&				destPath,       					// dest path to copy to
							                    std::vector<VxFileInfo> &	aoFileList,							// return FileInfo in array
							                    bool						bRecurse = false					// recurse subdirectories if true
							                    );
protected:
    RCODE                       substitutePath( std::string&		curPath,
                                                  std::string&		srcPath, 
                                                  std::string&		destPath, 
                                                  std::string&		resultPath );

    RCODE                       copyFileToDestDir( std::string& srcFileName, std::string& srcPath, std::string& destPath );

    RCODE                       createDestDirectoryIfNeeded( std::string&		curPath,
                                                             std::string&		srcPath,
                                                             std::string&		destPath );
};



