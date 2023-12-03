//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileListReplySession.h"

//============================================================================
FileListReplySession::FileListReplySession()
: QObject()
{
}

//============================================================================
FileListReplySession::FileListReplySession(	EPluginType	pluginType, GuiUser* guiUser, FileInfo& fileInfo )                                       
: QObject()
, m_PluginType(ePluginTypeInvalid)
, m_Ident( guiUser )
, m_FileInfo(fileInfo)
{
}

//============================================================================
FileListReplySession::FileListReplySession(const FileListReplySession& rhs)
: QObject()
{
	*this = rhs;
}

//============================================================================
FileListReplySession& FileListReplySession::operator=(const FileListReplySession& rhs)
{
	if( this != &rhs )
	{
		m_PluginType			= rhs.m_PluginType;
		m_Ident					= rhs.m_Ident;
        m_FileInfo              = rhs.m_FileInfo;
	}

	return *this;
}
