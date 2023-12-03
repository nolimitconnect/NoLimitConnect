//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <NlcDependLibrariesConfig.h>
#include <CoreLib/VxDefs.h>
#include <CoreLib/VxFileUtil.h>

class ICurlCallback
{
public:
	virtual void				curlCommandResultData( const char * data ) = 0;
};

class CurlTool
{
public:
	CurlTool( ICurlCallback& curlCallback );
	virtual ~CurlTool();

	virtual int					executeCurl( const char * cmd );

protected:
	int							parseIntoArgsList( const char * cmd, std::vector<std::string>& argList );

	VxFileUtil					m_FileUtil;
	ICurlCallback&				m_CurlCallback;
};

