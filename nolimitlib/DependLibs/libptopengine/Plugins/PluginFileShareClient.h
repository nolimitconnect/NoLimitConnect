#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseFilesClient.h"

class PluginFileShareClient : public PluginBaseFilesClient
{
public:
	PluginFileShareClient( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType );
	virtual ~PluginFileShareClient() = default;

	bool						startStream( std::shared_ptr<VxSktBase>& sktBase, AssetBaseInfo& assetInfo, VxGUID lclSessionId );

	bool						getIsInitialized( void ) { return m_WebPageClientReady; }

	virtual bool				fromGuiDownloadWebPage( EWebPageType webPageType, VxGUID& onlineId ) override;
	virtual bool				fromGuiCancelWebPage( EWebPageType webPageType, VxGUID& onlineId ) override;

	virtual bool				fromGuiDownloadFileList( VxGUID& onlineId, VxGUID& sessionId, uint8_t fileTypes = 0 ) override;
	virtual bool				fromGuiDownloadFileListCancel( VxGUID& onlineId, VxGUID& sessionId ) override;

	virtual std::string			getIncompleteFileXferDirectory( VxGUID& onlineId ) override;

	void						lockSearchFileList( void ) { m_SearchFilesListMutex.lock(); }
	void						unlockSearchFileList( void ) { m_SearchFilesListMutex.unlock(); }

	void						lockInProgressFileList( void ) { m_InProgressFilesListMutex.lock(); }
	void						unlockInProgressFileList( void ) { m_InProgressFilesListMutex.unlock(); }

	void						lockCompletedFileList( void ) { m_CompletedFilesListMutex.lock(); }
	void						unlockCompletedFileList( void ) { m_CompletedFilesListMutex.unlock(); }

	void						onPktStreamCtrlReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent ) override;

protected:
	virtual bool				onConnectForFileListDownload( std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId ) override;
	virtual bool                fileInfoSearchResult( VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, FileInfo& fileInfo ) override;
	virtual void                fileInfoSearchCompleted( VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId, ECommErr commErr ) override;

	virtual void				onAfterUserLogOnThreaded( void ) override;
	virtual void				onLoadedFilesReady( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes ) override;
	virtual void				onFilesChanged( int64_t lastFileUpdateTime, int64_t totalBytes, uint16_t fileTypes ) override;

	virtual bool				onFileDownloadComplete( VxGUID& onlineId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& lclSessionId, std::string& fileName, VxGUID& assetId, VxSha1Hash& sha11Hash ) override;

	void						setIsWebPageClientReady( bool isReady );
	bool						getIsWebPageClientReady( void ) { return m_WebPageClientReady; }

	void						checkIsWebPageClientReady( void );
	void						onWebPageClientReady( bool isReady );
	void						cancelDownload( void );
	bool						startDownload( VxGUID& searchSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID onlineId );

	std::string					getWebIndexFileName( void ) { return "story_board.htm"; }

	std::string					m_RootFileFolder{ "" };
	std::string					m_DownloadFileFolder{ "" };
	std::string					m_WebPageIndexFile{ "" };

	bool						m_WebPageClientReady{ false };

	VxMutex						m_SearchFilesListMutex;
	std::vector<FileInfo>		m_SearchFileInfoList;

	VxMutex						m_InProgressFilesListMutex;
	std::vector<FileInfo>		m_InProgressFileInfoList; // map of assetId, FileInfo

	VxMutex						m_CompletedFilesListMutex;
	std::vector<FileInfo>		m_CompletedFileInfoList;
};



