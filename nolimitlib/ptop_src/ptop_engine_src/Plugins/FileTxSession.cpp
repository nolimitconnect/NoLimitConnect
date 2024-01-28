//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileTxSession.h"

#include <CoreLib/VirtFileMgr.h>
#include <CoreLib/VxFileUtil.h>

#include <stdio.h>

//============================================================================
FileTxSession::FileTxSession()
: FileShareXferSession()
, m_iOutstandingAckCnt( 0 )
, m_bSendingPkts( false )
, m_bViewingFileList( false )
, m_QuePosition( 0 )
{
	setXferDirection( eXferDirectionTx );
}

//============================================================================
FileTxSession::FileTxSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID& sendToId )
: FileShareXferSession( sktBase, sendToId )
, m_iOutstandingAckCnt( 0 )
, m_bSendingPkts( false )
, m_bViewingFileList( false )
, m_QuePosition( 0 )
{
	setXferDirection( eXferDirectionTx );
}

//============================================================================
FileTxSession::FileTxSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& sendToId )
: FileShareXferSession( lclSessionId, sktBase, sendToId )
, m_iOutstandingAckCnt(0)
, m_bSendingPkts(false)
, m_bViewingFileList(false)
, m_QuePosition( 0 )
{
	setXferDirection( eXferDirectionTx );
}

//============================================================================
void FileTxSession::reset( void )
{
	FileShareXferSession::reset();
	m_iOutstandingAckCnt = 0;
	m_bSendingPkts = false;
	m_bViewingFileList = false;
	m_strOfferFile = "";
	m_strViewDirectory = "";
}

//============================================================================
void FileTxSession::cancelUpload( VxGUID& lclSessionId )
{
	if( m_FileXferInfo.m_hFile )
	{
		VFileClose( m_FileXferInfo.m_hFile );
	}

	std::vector<FileToXfer>::iterator iter;
	for( iter = m_FilesToXferList.begin(); iter != m_FilesToXferList.end(); ++iter )
	{
		if( (*iter).getLclSessionId() == lclSessionId )
		{
			m_FilesToXferList.erase(iter);
			break;
		}
	}
	for( iter = m_FilesXferedList.begin(); iter != m_FilesXferedList.end(); ++iter )
	{
		if( (*iter).getLclSessionId() == lclSessionId )
		{
			m_FilesToXferList.erase(iter);
			break;
		}
	}	
}
