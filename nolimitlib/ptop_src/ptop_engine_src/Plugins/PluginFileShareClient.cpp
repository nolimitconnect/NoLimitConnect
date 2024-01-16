//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginFileShareClient.h"
#include "PluginMgr.h"

#include <Plugins/FileInfo.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <NetLib/VxSktBase.h>

//============================================================================
PluginFileShareClient::PluginFileShareClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBaseFilesClient( engine, pluginMgr, myIdent, pluginType, "FileShareClient.db3" )
{
	setPluginType( ePluginTypeFileShareClient );
}

//============================================================================
void PluginFileShareClient::onAfterUserLogOnThreaded( void )
{
	m_RootFileFolder = VxGetDownloadsDirectory();
	getFileInfoMgr().setRootFolder( m_RootFileFolder );

	getFileInfoMgr().onAfterUserLogOnThreaded();
}

//============================================================================
void PluginFileShareClient::onLoadedFilesReady( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes )
{
	LogMsg( LOG_VERBOSE, "PluginFileShareClient::onLoadedFilesReady" );
	checkIsWebPageClientReady();
}

//============================================================================
void PluginFileShareClient::onFilesChanged( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes )
{
	checkIsWebPageClientReady();
}

//============================================================================
bool PluginFileShareClient::onFileDownloadComplete( VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& lclSessionId, std::string& fileName, VxGUID& assetId, VxSha1Hash& sha11Hash )
{
	bool result = onlineId.isVxGUIDValid() && sktBase && lclSessionId.isVxGUIDValid() && !fileName.empty() && assetId.isVxGUIDValid() && sha11Hash.isHashValid();
	if( result )
	{
		result = false;
		// move from in progress to completed
		lockInProgressFileList();
		for( auto iter = m_InProgressFileInfoList.begin(); iter != m_InProgressFileInfoList.end(); ++iter )
		{
			FileInfo& fileInfo = *iter;
			if( fileInfo.getAssetId() == assetId && fileInfo.getFileHashId() == sha11Hash )
			{
				lockCompletedFileList();
				fileInfo.setFileName( fileName );

				m_CompletedFileInfoList.push_back( fileInfo );
				m_InProgressFileInfoList.erase( iter );
				result = true;
				unlockCompletedFileList();
				break;
			}
		}

		unlockInProgressFileList();

		if( !result && !m_SearchFileInfoList.empty() )
		{
			FileInfo foundFileInfo;
			lockSearchFileList();
			// may have been started from a search result
			for( auto iter = m_SearchFileInfoList.begin(); iter != m_SearchFileInfoList.end(); ++iter )
			{
				FileInfo& curFileInfo = *iter;
				if( curFileInfo.getAssetId() == assetId && curFileInfo.getFileHashId() == sha11Hash )
				{
					lockCompletedFileList();
					curFileInfo.setFileName( fileName );
					foundFileInfo = curFileInfo;
					m_SearchFileInfoList.erase( iter );
					result = true;
					unlockCompletedFileList();
					break;
				}
			}

			unlockSearchFileList();
			if( result )
			{
				// all done
				m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgDownloadComplete, foundFileInfo.getFullFileName() );
			}
			else
			{
				// failed to find the web index file in downloaded files
				m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgDownloadFailed, "", 0 );
			}

			// do not start another
			result = false;
		}
		
		if( result )
		{
			result = false;
			if( m_SearchFileInfoList.empty() && m_InProgressFileInfoList.empty() )
			{
				// find the index file and send to gui
				FileInfo indexFileInfo;
				lockCompletedFileList();
				for( auto iter = m_CompletedFileInfoList.begin(); iter != m_CompletedFileInfoList.end(); ++iter )
				{
					FileInfo& fileInfo = *iter;
					if( fileInfo.getShortFileName() == getWebIndexFileName() )
					{
						indexFileInfo = fileInfo;
						result = true;
						break;
					}
				}

				unlockCompletedFileList();
				if( result )
				{
					// all done
					m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgDownloadComplete, indexFileInfo.getFullFileName() );
				}
				else
				{
					// failed to find the web index file in downloaded files
					m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgDownloadFailed, "", 0 );
				}
			}
			else
			{
				result = startDownload( lclSessionId, sktBase, onlineId );
			}
		}
		else
		{
			m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgDownloadFailed, "", 0 );
		}
	}
	else
	{
		m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgInvalidParam, "", 0 );
	}

	return result;
}

//============================================================================
void PluginFileShareClient::checkIsWebPageClientReady( void )
{
	setIsWebPageClientReady( getFileInfoMgr().getIsInitialized() );
}

//============================================================================
void PluginFileShareClient::setIsWebPageClientReady( bool isReady )
{
	if( m_WebPageClientReady != isReady )
	{
		m_WebPageClientReady = isReady;
		onWebPageClientReady( isReady );
	}
}

//============================================================================
void PluginFileShareClient::onWebPageClientReady( bool isReady )
{

}

//============================================================================
std::string	PluginFileShareClient::getIncompleteFileXferDirectory( VxGUID& onlineId )
{
	std::string incompleteDir{ "" };
	if( onlineId.isVxGUIDValid() )
	{
		incompleteDir = m_RootFileFolder + onlineId.toHexString().c_str() + "/";
		VxFileUtil::makeDirectory( incompleteDir.c_str() );
		if( VxFileUtil::directoryExists( incompleteDir.c_str() ) )
		{
			int64_t diskFreeSpace = VxFileUtil::getDiskFreeSpace( incompleteDir.c_str() );

			if( diskFreeSpace && diskFreeSpace < VxFileUtil::SIZE_1GB )
			{
				m_Engine.getToGui().toGuiPluginMsg( getPluginType(), onlineId, ePluginMsgLowDiskSpace, "" );
			}
		}
		else
		{
			m_Engine.getToGui().toGuiPluginMsg( getPluginType(), onlineId, ePluginMsgPermissionError, incompleteDir.c_str() );
		}
	}

	return incompleteDir;
}

//============================================================================
bool PluginFileShareClient::fromGuiDownloadWebPage( EWebPageType webPageType, VxGUID& onlineId )
{
	bool result{ false };
	if( (eWebPageTypeAboutMe == webPageType || eWebPageTypeStoryboard == webPageType ) && onlineId.isVxGUIDValid() )
	{
		m_HisOnlineId = onlineId;
		m_DownloadFileFolder = getIncompleteFileXferDirectory( onlineId );
		if( VxFileUtil::directoryExists( m_DownloadFileFolder.c_str() ) )
		{
			// must clear any previous files or download will make duplicates filename_1 filename_2 etc
			VxFileUtil::deleteFilesInFolder( m_DownloadFileFolder, true );
			m_WebPageIndexFile = m_DownloadFileFolder + getWebIndexFileName();
			int64_t diskFreeSpace = VxFileUtil::getDiskFreeSpace( m_DownloadFileFolder.c_str() );

			if( diskFreeSpace && diskFreeSpace < VxFileUtil::SIZE_1GB )
			{
				m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgLowDiskSpace, "" );
			}
			else
			{
				m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgConnecting, "" );
				connectForWebPageDownload( onlineId );
			}
		}
		else
		{
			m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgPermissionError, m_DownloadFileFolder.c_str() );
		}
	}
	else
	{
		LogMsg( LOG_VERBOSE, "PluginFileShareClient::fromGuiDownloadWebPage invalid EWebPageType" );
	}

	return result;
}

//============================================================================
bool PluginFileShareClient::fromGuiCancelWebPage( EWebPageType webPageType, VxGUID& onlineId )
{
	bool result{ false };
	if( eWebPageTypeAboutMe == webPageType )
	{
		cancelDownload();
		m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgCanceled, "" );

	}
	else
	{
		LogMsg( LOG_VERBOSE, "PluginFileShareClient::fromGuiCancelWebPage invalid EWebPageType" );
	}

	return result;
}

//============================================================================
bool PluginFileShareClient::onConnectForFileListDownload( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
	lockSearchFileList();
	m_SearchFileInfoList.clear();
	unlockSearchFileList();
	return PluginBaseFilesClient::onConnectForFileListDownload( sktBase, onlineId );
}

//============================================================================
bool PluginFileShareClient::fileInfoSearchResult( VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, FileInfo& fileInfo )
{
	bool result{ false };
	if( fileInfo.determineFullFileName( m_DownloadFileFolder ) )
	{
		result = fileInfo.isValid( true );
		if( result )
		{
			lockSearchFileList();
			m_SearchFileInfoList.push_back( fileInfo );
			unlockSearchFileList();
			sendFileSearchResultToGui( searchSessionId, onlineId, fileInfo );
		}
	}

	return result;
}

//============================================================================
void PluginFileShareClient::fileInfoSearchCompleted( VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, ECommErr commErr )
{
	if( commErr == eCommErrNone )
	{
		LogMsg( LOG_VERBOSE, "PluginFileShareClient::fileInfoSearchCompleted with no errors" );

		m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgRetrieveInfoComplete, " %d", m_SearchFileInfoList.size() );
	}
	else
	{
		LogMsg( LOG_ERROR, "PluginFileShareClient::fileInfoSearchCompleted with error %s from %s", DescribeCommError( commErr ), sktBase->describeSktConnection().c_str() );
		m_Engine.getToGui().toGuiPluginCommError( getPluginType(), m_HisOnlineId, ePluginMsgRetrieveInfoFailed, commErr );
	}
}

//============================================================================
void PluginFileShareClient::cancelDownload( void )
{
	lockSearchFileList();
	for( auto& fileInfo : m_SearchFileInfoList )
	{
		m_FileInfoMgr.cancelAndDelete( fileInfo.getAssetId() );
	}

	m_SearchFileInfoList.clear();
	unlockSearchFileList();

	lockInProgressFileList();
	for( auto& fileInfo : m_InProgressFileInfoList )
	{
		m_FileInfoMgr.cancelAndDelete( fileInfo.getAssetId() );
	}

	m_InProgressFileInfoList.clear();
	unlockInProgressFileList();

	lockCompletedFileList();
	for( auto& fileInfo : m_InProgressFileInfoList )
	{
		m_FileInfoMgr.cancelAndDelete( fileInfo.getAssetId() );
	}

	m_InProgressFileInfoList.clear();
	unlockCompletedFileList();

	// clear out files on cancel
	VxFileUtil::deleteFilesInFolder( m_DownloadFileFolder, true );
}

//============================================================================
bool PluginFileShareClient::startDownload( VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId )
{
	bool result{ false };
	lockSearchFileList();
	for( auto iter = m_SearchFileInfoList.begin(); iter != m_SearchFileInfoList.end(); ++iter )
	{
		FileInfo& fileInfo = *iter;
		lockInProgressFileList();
		VxGUID xferSessionId = fileInfo.initializeNewXferSessionId();

		m_InProgressFileInfoList.push_back( fileInfo );
		if( m_FileInfoMgr.startDownload( *iter, xferSessionId, sktBase, onlineId ) )
		{
			result = true;
			m_SearchFileInfoList.erase( iter );
		}
		else
		{
			m_InProgressFileInfoList.pop_back();
		}

		unlockInProgressFileList();
		break;
	}

	unlockSearchFileList();
	return result;
}

//============================================================================
bool PluginFileShareClient::fromGuiDownloadFileList( VxGUID& onlineId, VxGUID& sessionId, uint8_t fileTypes )
{
	bool result{ false };
	m_HisOnlineId = onlineId;
	m_SearchSessionId = sessionId;
	m_LclSessionId = sessionId;
	m_DownloadFileFolder = getIncompleteFileXferDirectory( onlineId );
	if( VxFileUtil::directoryExists( m_DownloadFileFolder.c_str() ) )
	{
		int64_t diskFreeSpace = VxFileUtil::getDiskFreeSpace( m_DownloadFileFolder.c_str() );

		if( diskFreeSpace && diskFreeSpace < VxFileUtil::SIZE_1GB )
		{
			m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgLowDiskSpace, "" );
		}
		else
		{
			m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgConnecting, "" );
			if( !fileTypes )
			{
				fileTypes = VXFILE_TYPE_ALLNOTEXE;
			}

			setSearchFileTypes( fileTypes );
			result = connectForFileListDownload( onlineId );
		}
	}
	else
	{
		m_Engine.getToGui().toGuiPluginMsg( getPluginType(), m_HisOnlineId, ePluginMsgPermissionError, m_DownloadFileFolder.c_str() );
	}

	return result;
}

//============================================================================
bool PluginFileShareClient::startStream( std::shared_ptr<VxSktBase>& sktBase, AssetBaseInfo& assetInfo, VxGUID lclSessionId )
{
	FileInfo fileInfo = assetInfo.getFileInfo();
	fileInfo.setIsStream( true );
	fileInfo.setXferSessionId( lclSessionId );

	return m_FileInfoMgr.startDownload( fileInfo, lclSessionId, sktBase, assetInfo.getDestUserId() );
}

//============================================================================
bool PluginFileShareClient::fromGuiDownloadFileListCancel( VxGUID& onlineId, VxGUID& sessionId )
{

	return false;
}

//============================================================================
void PluginFileShareClient::onPktStreamCtrlReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{

}