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

class VxFileFinder
{
public:
	bool m_bAbort;

	void Abort( void ){ m_bAbort = true; }

	//find files matching the extension (file type) list
	RCODE FindFilesByExtension(	std::string					csPath,								//start path to search in
								std::vector<std::string> &	acsExtensionList,					//Extensions of files to find
								std::vector<VxFileInfo> &	aoFileList,							//return FileInfo in array
								bool						bRecurse = false,					//recurse subdirectories if true
								bool						bUseFilterListToExclude = false		//if true dont return files matching filter else return files that do
								);
	//find files matching the extension (file type) list
	RCODE FindFilesByName(		std::string					csPath,								//start path to search in
								std::vector<std::string> &	acsWildNameList,					//Wildcard Name match strings
								std::vector<VxFileInfo> &	aoFileList,							//return FileInfo in array
								bool						bRecurse = false,					//recurse subdirectories if true
								bool						bUseFilterListToExclude = false		//if true dont return files matching filter else return files that do
								);
protected:
	bool HasSameExtension( std::string csCurrentNode,
							std::vector<std::string> &acsExtensionList );
	bool HasMatchingName( std::string csCurrentNode,
							std::vector<std::string> &acsWildNameList );
};

//! search for files
RCODE VxFindFilesByExtension(	std::string					csPath,							//start path to search in
								std::vector<std::string>&	acsExtensionList,				//Extensions ( file extentions )
								std::vector<VxFileInfo>&	aoFileList,						//return FileInfo in array
								bool						bRecurse = false,				//recurse subdirectories if true
								bool						bUseFilterListToExclude = false	//if true dont return files matching filter else return files that do
								);

RCODE VxFindFilesByName(	std::string					csPath,							//start path to search in
							std::vector<std::string>&	acsWildNameList,				//filters
							std::vector<VxFileInfo>&	aoFileList,						//return FileInfo in array
							bool						bRecurse = false,				//recurse subdirectories if true
							bool						bUseFilterListToExclude = false	//if true dont return files matching filter else return files that do
							);




