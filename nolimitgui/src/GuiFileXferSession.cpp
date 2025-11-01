//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiFileXferSession.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <AssetBase/AssetBaseInfo.h>

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
		m_StreamingEnable	= rhs.m_StreamingEnable;
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

//============================================================================
bool GuiFileXferSession::getAssetInfo( AssetBaseInfo& assetInfo, bool asStream )
{
	AssetBaseInfo assetBaseInfo( m_FileInfo );
	if( asStream )
	{
		assetBaseInfo.setIsStream( true );
	}

	assetInfo = assetBaseInfo;
	return assetInfo.isValid();
}

//============================================================================
void GuiFileXferSession::resetXferState( void )
{
	m_XferState = eXferStateUnknown;
	m_XferError = eXferErrorNone;
	m_XferProgress = 0;
}
