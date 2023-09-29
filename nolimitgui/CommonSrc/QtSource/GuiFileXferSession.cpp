//============================================================================
// Copyright (C) 2013 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiFileXferSession.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <PktLib/VxSearchDefs.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxFileInfo.h>

#include <QObject>

//============================================================================
GuiFileXferSession::GuiFileXferSession( EXferDirection xferDir, EPluginType	pluginType, GuiUser* guiUser, FileInfo& fileInfo )
	: m_ePluginType( pluginType )
	, m_Ident( guiUser )
	, m_FileInfo( fileInfo )
	, m_XferDirection( xferDir )
{
}

//============================================================================
GuiFileXferSession::GuiFileXferSession( EXferDirection xferDir, EPluginType	pluginType, GuiUser* guiUser, VxGUID& lclSessionId, FileInfo& fileInfo )
: m_ePluginType( pluginType )
, m_Ident( guiUser )
, m_LclSessionId( lclSessionId )
, m_FileInfo( fileInfo )
, m_XferDirection( xferDir )
{
}

//============================================================================
GuiFileXferSession::GuiFileXferSession( const GuiFileXferSession &rhs )
    : QObject()
{
	*this = rhs;
}

//============================================================================
GuiFileXferSession& GuiFileXferSession::operator =( const GuiFileXferSession &rhs )
{
	if( this != &rhs )   
	{
		m_ePluginType		= rhs.m_ePluginType;
		m_Ident				= rhs.m_Ident;
		m_LclSessionId		= rhs.m_LclSessionId;
		m_FileInfo			= rhs.m_FileInfo;
		m_XferState			= rhs.m_XferState;
		m_XferError			= rhs.m_XferError;		
		m_XferDirection		= rhs.m_XferDirection;
		m_XferProgress		= rhs.m_XferProgress;
		// do not copy 		m_Widget
	}

	return *this;
}

//============================================================================
void GuiFileXferSession::updateSession( GuiFileXferSession* otherSession )
{
	setFileInfo( otherSession->getFileInfo() );
	setXferState( otherSession->getXferState(), otherSession->getXferError(), otherSession->getXferProgress() );
}

//============================================================================
QString GuiFileXferSession::describeFileType()
{
	return GuiParams::describeFileType( m_FileInfo.getFileType() );
}

//============================================================================
QString GuiFileXferSession::describeFileLength()
{
	return GuiParams::describeFileLength( m_FileInfo.getFileLength() );
}

//============================================================================
bool GuiFileXferSession::getIsCompleted( void )
{
	return ( eXferStateCompletedUpload == m_XferState ) || ( eXferStateCompletedDownload == m_XferState );
}

//============================================================================
bool GuiFileXferSession::getIsInError( void )
{
	return eXferStateDownloadError == m_XferState || eXferStateUploadError == m_XferState || eXferErrorNone != getXferError();
}

//============================================================================
QString GuiFileXferSession::describeXferState( void )
{
	return GuiParams::describeEXferState( m_XferState );
}
