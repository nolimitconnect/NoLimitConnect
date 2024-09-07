#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <CoreLib/AssetDefs.h>
#include <CoreLib/VxGUID.h>
#include <CoreLib/VxSha1Hash.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxSemaphore.h>
#include <CoreLib/VxMutex.h>

enum EAssetMgrType
{
    eAssetMgrTypeNone,
    eAssetMgrTypeAssetBase,
    eAssetMgrTypeAssets,
    eAssetMgrTypeBlob,
    eAssetMgrTypeThumb,

    eMaxAssetMgrType
};

class AssetBaseCallbackInterface;
class AssetBaseInfo;
class AssetBaseInfoDb;
class AssetBaseHistoryMgr;
class AssetBaseXferMgr;
class FileInfo;
class GroupieId;
class IToGui;
class P2PEngine;
class PktFileListReply;

class AssetBaseMgr
{
public:
	AssetBaseMgr( P2PEngine& engine, const char* dbName, const char* dbStateName, EAssetMgrType assetMgrType );
	virtual ~AssetBaseMgr();

    class AutoResourceLock
    {
    public:
        AutoResourceLock( AssetBaseMgr * assetMgrBase ) : m_Mutex(assetMgrBase->getResourceMutex())	{ m_Mutex.lock(); }
        ~AutoResourceLock()																			{ m_Mutex.unlock(); }
        VxMutex&				m_Mutex;
    };

    virtual AssetBaseInfoDb&    getAssetInfoDb( void )                                  { return m_AssetBaseInfoDb; }
    virtual std::vector<AssetBaseInfo*>& getAssetBaseInfoList( void )					{ return m_AssetBaseInfoList; }

	static std::vector<VxGUID>& getEmoticonIdList( void )							    { return m_EmoticonIdList; }

	static bool                 isEmoticonThumbnail( VxGUID& thumbId );

    // startup when user specific directory has been set after user logs on
    virtual void				onPluginsInitialized( void );

    virtual bool				fromGuiSetFileIsShared( FileInfo& fileInfo, bool shareFile );
    virtual bool				fromGuiQueryFileHash( FileInfo& fileInfo );
    virtual void				fromGuiFileHashGenerated( std::string& fileName, int64_t fileLen, VxSha1Hash& fileHash );

    virtual void				announceAssetAdded( AssetBaseInfo* assetInfo );
    virtual void				announceAssetUpdated( AssetBaseInfo* assetInfo );
    virtual void				announceAssetRemoved( AssetBaseInfo* assetInfo );
    virtual void				announceAssetXferState( VxGUID& sendToId, VxGUID& assetUniqueId, EAssetSendState assetSendState, int param );

    virtual void                onQueryHistoryAsset( AssetBaseInfo* assetInfo ); // should be overriden

    VxMutex&					getResourceMutex( void )					{ return m_ResourceMutex; }
    void						lockResources( void )						{ m_ResourceMutex.lock(); }
    void						unlockResources( void )						{ m_ResourceMutex.unlock(); }

    void						addAssetMgrClient( AssetBaseCallbackInterface * client, bool enable );
    bool						isAllowedFileOrDir( std::string fileNameAndPath );

	virtual bool				isAssetListInitialized( void )				{ return m_AssetBaseListInitialized; }

	void						assetInfoMgrStartup( VxThread* startupThread );
	void						assetInfoMgrShutdown( void );

    bool						getFileHashId( std::string& fileNameAndPath, VxSha1Hash& retFileHashId );
	bool						getFileFullName( VxSha1Hash& fileHashId, std::string& retFileFullName );

    virtual bool				doesAssetExist( AssetBaseInfo& assetInfo ); // check if file still exists in directory or database

    AssetBaseInfo*				findAsset( std::string& fileNameAndPath );
	AssetBaseInfo*				findAsset( VxSha1Hash& fileHashId );
	AssetBaseInfo*				findAsset( VxGUID& assetId );

	uint16_t					getAssetBaseFileTypes( void )				{ return m_u16AssetBaseFileTypes; }
	void						updateAssetFileTypes( void );

	void						lockFileListPackets( void )					{ m_FileListPacketsMutex.lock(); }
	void						unlockFileListPackets( void )				{ m_FileListPacketsMutex.unlock(); }
	std::vector<PktFileListReply*>&	getFileListPackets( void )				{ return m_FileListPackets; }
	void						updateFileListPackets( void );

    AssetBaseInfo* 			    addAssetFile( enum EAssetType assetType, const char* fileName, const char* fileNameAndPath, uint64_t fileLen );
    AssetBaseInfo*				addAssetFile( enum EAssetType assetType, const char* fileName, const char* fileNameAndPath, uint64_t fileLen, VxGUID& assetId );

    bool						addAssetFile(   enum EAssetType assetType,
                                                const char*	    fileName, 
                                                const char*     fileNameAndPath,
												VxGUID&			assetId,  
												uint8_t *		hashId = 0, 
												EAssetLocation	locationFlags = eAssetLocUnknown, 
												const char*	    assetTag = "", 
												int64_t		    timestamp = 0 );

    bool						addAssetFile(	enum EAssetType assetType,
                                                const char*	    fileName, 
                                                const char*     fileNameAndPath,
												VxGUID&			assetId,  
												VxGUID&		    creatorId, 
												VxGUID&		    historyId, 
												uint8_t *		hashId = 0, 
												EAssetLocation	locationFlags = eAssetLocUnknown, 
												const char*	    assetTag = "", 
                                                int64_t			timestamp = 0 );

	virtual bool				addAsset( AssetBaseInfo& assetInfo, AssetBaseInfo*& retCreatedAsset );

    bool						updateAsset( AssetBaseInfo& assetInfo );
    bool						updateAsset( FileInfo& fileInfo );

    bool						removeAsset( std::string fileNameAndPath, bool deleteFile = false );
	bool						removeAsset( VxGUID& assetUniqueId, bool deleteFile = false );
	void						fromGuiQuerySessionHistory( GroupieId& groupieId );
    void                        sendHistoryAssetsToGuiByThread( VxThread* poThread );

	void						generateHashForFile( std::string fileName );
	void						updateAssetXferState( VxGUID& sendToId, VxGUID& assetUniqueId, EAssetSendState assetSendState, int param = 0 );

    void                        getStreamableAssets( std::vector<AssetBaseInfo>& streamableAssets );
    void                        getSharedFiles( std::vector<AssetBaseInfo>& sharedFiles );

protected:
    virtual AssetBaseInfo*      createAssetInfo( AssetBaseInfo& assetInfo ) = 0;
    virtual AssetBaseInfo*      createAssetInfo( FileInfo& fileInfo ) = 0;
    virtual AssetBaseInfo*      createAssetInfo( enum EAssetType asset, const char* fileName, const char* fileNameAndPath, uint64_t fileLen ) = 0;
    virtual AssetBaseInfo*      createAssetInfo( enum EAssetType asset, const char* fileName, const char* fileNameAndPath, uint64_t fileLen, VxGUID& assetId ) = 0;

    void						lockClientList( void )						{ m_ClientListMutex.lock(); }
    void						unlockClientList( void )					{ m_ClientListMutex.unlock(); }

    virtual AssetBaseInfoDb&    createAssetInfoDb(  const char* dbName, EAssetMgrType assetMgrType );


	void						updateAssetListFromDb( VxThread* thread );
	void						generateHashIds( VxThread* thread );
	void						clearAssetFileListPackets( void );
	void						clearAssetInfoList( void );
    AssetBaseInfo*				createAssetInfo(	enum EAssetType assetType,
                                                    const char*	    fileName, 
                                                    const char*	    fileNameAndPath,
													VxGUID&			assetId,  
													uint8_t *		hashId, 
                                                    enum EAssetLocation	locationFlags = eAssetLocUnknown,
													const char*	    assetTag = "", 
													int64_t			timestamp = 0 );
	bool						insertNewInfo( AssetBaseInfo* assetInfo );
	void						updateDatabase( AssetBaseInfo* assetInfo );
    void						updateAssetDatabaseSendState( VxGUID& assetUniqueId, enum EAssetSendState sendState );

    //=== vars ===//
    P2PEngine&					m_Engine;
    EAssetMgrType               m_AssetMgrType{ eAssetMgrTypeNone };
    VxMutex						m_ResourceMutex;
    VxMutex						m_ClientListMutex;

    std::vector<AssetBaseCallbackInterface *> m_AssetClients;

	bool						m_Initialized{ false };

	std::vector<AssetBaseInfo*>	m_WaitingForHastList;
	std::vector<std::string>	m_GenHashList;
	VxMutex						m_GenHashMutex;
	VxThread					m_GenHashThread;
	VxSemaphore					m_GenHashSemaphore;

    uint16_t					m_u16AssetBaseFileTypes{ 0 };
	VxMutex						m_FileListPacketsMutex;
	std::vector<PktFileListReply*> m_FileListPackets;

protected:
    bool						m_AssetBaseListInitialized{ false };
    AssetBaseInfoDb&			m_AssetBaseInfoDb;
    std::vector<AssetBaseInfo*>	m_AssetBaseInfoList;
	static std::vector<VxGUID>	m_EmoticonIdList;

    VxThread					m_HistoryListThread;
    std::vector<GroupieId>      m_HistorySendList;
};

