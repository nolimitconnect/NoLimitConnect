//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetBaseMgr.h"
#include "AssetBaseInfo.h"
#include "AssetBaseInfoDb.h"
#include "AssetBaseCallbackInterface.h"

#include <P2PEngine/P2PEngine.h>

#include <AssetMgr/AssetInfoDb.h>
#include <BlobXferMgr/BlobInfoDb.h>
#include <ThumbMgr/ThumbInfoDb.h>
#include <Plugins/FileInfo.h>
#include <Plugins/PluginFileShareServer.h>
#include <SendQueue/SendQueueMgr.h>

#include <GuiInterface/IToGui.h>

#include <PktLib/PktAnnounce.h>
#include <PktLib/PktsFileList.h>

#include <CoreLib/Sha1GeneratorMgr.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxFileShredder.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxTime.h>

#include <algorithm>
#include <time.h>

namespace
{
	const char* ASSET_INFO_DB_NAME = "AssetBaseInfoDb.db3";

	//============================================================================
    static void * AssetBaseMgrStartupThreadFunc( void * pvContext )
	{
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );
		AssetBaseMgr * poMgr = (AssetBaseMgr *)poThread->getThreadUserParam();
        if( poMgr )
        {
            poMgr->assetInfoMgrStartup( poThread );
        }

		poThread->threadAboutToExit();
        return nullptr;
	}


	//============================================================================
	static void* AssetBaseMgrHistoryListThreadFunc( void* pvContext )
	{
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );
		AssetBaseMgr* poMgr = (AssetBaseMgr*)poThread->getThreadUserParam();
		if( poMgr )
		{
			poMgr->sendHistoryAssetsToGuiByThread( poThread );
		}

		poThread->threadAboutToExit();
		return nullptr;
	}
}

std::vector<VxGUID>	AssetBaseMgr::m_EmoticonIdList{
{ 3913462368200503545U, 2760340527898317750U },	  // 1 !364F694A1A7330F9264EB315D07E53B6! // thumbs up
{ 13999558228189016709U, 7413105485242018473U },  // 2 !C2486C09239A728566E0A4299A59AAA9! // thumbs down
{ 10829822772292086897U, 16991265692937849009U }, // 3 !964B426EBAE7EC71EBCD1A1FC7D788B1! // smile
{ 7945445069844048192U, 12229757484046445461U },  // 4 !6E43E431BA6EE940A9B8D3A2BDC36B95! // sun glasses
{ 2324257314457588032U, 17711723986212541605U },  // 5 !20416BB68AD91140F5CCAF1FEDF6D4A5! // angel
{ 10756322002679464500U, 16760991902078506408U }, // 6 !954621DF3B6D8234E89B0154D69695A8! // devil
{ 16664578291555380852U, 7906860280759028378U },  // 7 !E74479A9D7D82E746DBACF8873498A9A! // heart eyes
{ 15874765662471761056U, 5995629303460670394U },  // 8 !DC4E7F430C32ACA05334C115D5B22BBA! // kiss
{ 14217291826520502406U, 3076774834097433248U },  // 9 !C54DF796FBD1B0862AB2E669003D2AA0! // flowers
{ 12701126956946010700U, 1186903973967391369U },  // 10 !B04377AC83E7B24C1078BAE036D36A89! // heart
{ 8016853232088298773U, 14460858311392523710U },  // 11 !6F41958A1B10B515C8AF4A046DCE75BE! // swear
{ 16591719547199697346U, 4982584807328050075U },  // 12 !E641A1057FBE31C24525B27BC00BEB9B! // mad
{ 10037911910613534570U, 16761057474731904642U }, // 13 !8B4DD3B316C8476AE89B3CF8294D0E82! // snort
{ 4489243676149697357U, 2172841312657848993U },   // 14 !3E4CFF56A3124B4D1E277BA96243BAA1! // nuclear
{ 15799846624809938546U, 1468224797688694922U },  // 15 !DB4454CB49736E7214602EACACA7888A! // cry flood
{ 13352407061259212155U, 13577725499120930184U }, // 16 !B94D477A66C5D57BBC6DC56750DDCD88! // poop
{ 1462663878651414172U, 9453108208298294703U },   // 17 !144C6D0C50D0069C83302FD4AAF2FDAF! // blue
{ 6649443090428064893U, 14264198717685600396U },  // 18 !5C479142AF23F47DC5F49D28A53B908C! // happy cry
{ 2469078113775934304U, 13112475985264128399U },  // 19 !2243ED76764E5B60B5F8DF6F8B82318F! // ebrows furrowed
{ 7730781286067177641U, 3789408903274709648U },   // 20 !6B49409EC84588A93496AF5204644290! // suprise
{ 16159875440403194858U, 7145204477185456825U },  // 21 !E043691827CAA3EA6328DD9D762B7AB9! // huh
{ 14721770146071603619U, 3089010263141508768U },  // 22 !CC4E3BF2D58C69A32ADE5E77D215E6A0! // barf
{ 7227605314620074266U, 2103507223893740458U },   // 23 !644D9CBD7AA7A91A1D3128ACDEA78BAA! // ok with fingers
{ 14357759964851208390U, 13198575573570798776U }, // 24 !C741029E0B4800C6B72AC28E1BD5C4B8! // bandage
{ 10611472365506944914U, 2062838987330950796U },  // 25 !934385E4F9EA5F921CA0AD212CB0168C! // thermometer
{ 10899107697234873459U, 4802095315587358606U },  // 26 !974168B466690C7342A4783F3726DF8E! // sick
{ 1891901626467743579U, 13846726454066888592U },  // 27 !1A4162816B97F75BC0297458CCA93F90! // sneeze
{ 15655074862930481624U, 16561591255687549347U }, // 28 !D941FFA4C91E51D8E5D6977FFE78D1A3! // corner of mouth up
{ 15654901279462188462U, 3057683189972498358U },  // 29 !D94161C53C26E5AE2A6F12A935FE77B6! // look up
{ 5207910658819667909U, 8685100309678688915U },   // 30 !48463726C8EAB7C57887ACB74D088A93! // blush
{ 14574170544256737769U, 7416743895397619600U },  // 31 !CA41DAE48A6261E966ED9146FB38D790! // suprise
{ 596537419207726873U, 17631004771266123946U },   // 32 !0847539F471AD719F4ADE96D2888D8AA! // shush
{ 12848138990292053852U, 9367519687061681027U },  // 33 !B24DC256DD342F5C82001D862DCA3383! // monical
{ 3837853253425696308U, 1553238764248287121U },   // 34 !3542CB333E840E34158E366D3A62C791! // hand on chin (thinking)
{ 3910185950057618989U, 7631546539539877510U },   // 35 !3643C5678EE85E2D69E8B324E022CE86! // pinocheo
{ 15657202536165776185U, 17647356409875005320U }, // 36 !D9498EC050819B39F4E80127B34FFF88! // hand cover mouth
{ 238173133304132614U, 4864860177195122085U },    // 37 !034E2933EDA274064383748E57A8BDA5! // hands out
{ 12341615954129516697U, 2738267793027437699U },  // 38 !AB463A52542EF8992600480CBF4B1483! // nerd
{ 12632410285504908102U, 4294383552542830217U },  // 39 !AF4F5639AC82CB463B98B716A2986E89! // clown
{ 15438663251370106147U, 5292085521134278792U },  // 40 !D641266DF8CADD23497143BE28325888! // drool
{ 4993020490155633777U, 11738686233263127465U },  // 41 !454AC5AE7FF92C71A2E830EF576ABBA9! // emoj41.svg (tongue out raspberry)
{ 16522474626922221383U, 15482595468947255479U }, // 42 !E54B9F22241A8F47D6DD3A8C77AC70B7! // emoj42.svg( linux penguin )
{ 14504884163639376415U, 13718914074721043350U }, // 43 !C94BB34BF1E9FA1FBE635FAA3FDAC396! // emoj43.svg( raspberry pi penquin )
{ 7584589277108821538U, 10124185894024794536U },  // 44 !6941DFC33D4EDE228C80556E2D5E51A8! // emoj44.svg( ubuntu )
{ 5711226150392763480U, 8497943599727683497U },   // 45 !4F4259ECA615145875EEC2AE0F12A7A9! // emoj45.svg( linux mint )
{ 9097487941835145970U, 4367213554728824753U },   // 46 !7E40C515217BC2F23C9B7596EBE1EFB1! // emoj46.svg( freebsd )
{ 9100097679121650618U, 4285115141208480959U },   // 47 !7E4A0A9FF16713BA3B77C98475611CBF! // emoj47.svg( android )
{ 10972345742477311342U, 7384896862811218842U },  // 48 !98459A519D213D6E667C6C91E2C2639A! // emoj48.svg( windows ladybug )
{ 17456372216474597834U, 10723652770054697090U }, // 49 !F2417E0B05F779CA94D21160C5BE7082! // emoj49.svg( apple )
{ 15802608836277822000U, 16924341092067752867U }, // 50 !DB4E2502C2C9E230EADF568DE23EBBA3! // emoj50.svg( old computer )
}; 

//============================================================================
AssetBaseMgr::AssetBaseMgr( P2PEngine& engine, const char* dbName, const char* dbStateName, EAssetMgrType assetMgrType )
: m_Engine( engine )
, m_AssetMgrType( assetMgrType )
, m_AssetBaseInfoDb( createAssetInfoDb( dbName, assetMgrType ) )
, m_FileShredder( GetVxFileShredder() )
{
}

//============================================================================
AssetBaseMgr::~AssetBaseMgr()
{
	delete &m_AssetBaseInfoDb;
}

//============================================================================
bool AssetBaseMgr::isEmoticonThumbnail( VxGUID& thumbId )
{
	return thumbId.isVxGUIDValid() && std::find( m_EmoticonIdList.begin(), m_EmoticonIdList.end(), thumbId ) != m_EmoticonIdList.end();
}

//============================================================================
AssetBaseInfoDb& AssetBaseMgr::createAssetInfoDb( const char* dbName, EAssetMgrType assetMgrType )
{
    switch( assetMgrType )
    {
    case eAssetMgrTypeAssets:
        return *(new AssetInfoDb( *this, dbName ));
    case eAssetMgrTypeBlob:
        return *(new BlobInfoDb( *this, dbName ));
    case eAssetMgrTypeThumb:
        return *(new ThumbInfoDb( *this, dbName ));

    default:
        return *(new AssetInfoDb( *this, dbName ));
    }
}

//============================================================================
void AssetBaseMgr::onPluginsInitialized( void )
{
	if( !m_Initialized )
	{
		m_Initialized = true;
		m_AssetMgrStartupThread.startThread( (VX_THREAD_FUNCTION_T)AssetBaseMgrStartupThreadFunc, this, "AssetBaseMgrStartup" );			
	}
}

//============================================================================
void AssetBaseMgr::assetInfoMgrStartup( VxThread* startupThread )
{
	if( startupThread->isAborted() )
		return;
	// user specific directory should be set
	std::string dbName = VxGetSettingsDirectory();
	dbName += ASSET_INFO_DB_NAME; 
	lockResources();
	m_AssetBaseInfoDb.dbShutdown();
	m_AssetBaseInfoDb.dbStartup( 1, dbName );
	unlockResources();
	if( startupThread->isAborted() )
		return;
	updateAssetListFromDb( startupThread );
	m_AssetBaseListInitialized = true;
}

//============================================================================
void AssetBaseMgr::assetInfoMgrShutdown( void )
{
	m_AssetMgrStartupThread.abortThreadRun( true );
	lockResources();
	clearAssetInfoList();
	clearAssetFileListPackets();
	m_AssetBaseInfoDb.dbShutdown();
	unlockResources();
	m_AssetBaseListInitialized = false;
	m_Initialized = false;
}

//============================================================================
bool AssetBaseMgr::doesAssetExist( AssetBaseInfo& assetInfo ) // check if file still exists in directory or database
{
    if( assetInfo.isDeleted() )
    {
        return false;
    }

    if( assetInfo.isFileAsset() || assetInfo.isThumbAsset() )
    {
        if( !assetInfo.getAssetLength() || !( assetInfo.getAssetLength() == (int64_t)VxFileUtil::getFileLen( assetInfo.getAssetNameAndPath().c_str() ) ) )
        {
            LogMsg( LOG_WARN, "File %s no longer exists for asset %s length %lld", assetInfo.getAssetNameAndPath().c_str(), assetInfo.getAssetUniqueId().toOnlineIdString().c_str(), assetInfo.getAssetLength() );
            assetInfo.setIsDeleted( true );
            updateDatabase( &assetInfo );
            return false;
        }

        return true;
    }

    // TODO verify exists in database
    return true;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::findAsset( std::string& fileNameAndPath )
{
	std::vector<AssetBaseInfo*>::iterator iter;
	for( iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
        if( (*iter)->getAssetNameAndPath() == fileNameAndPath )
		{
			return (*iter);
		}
	}

	return 0;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::findAsset( VxSha1Hash& fileHashId )
{
	if( false == fileHashId.isHashValid() )
	{
		LogMsg( LOG_ERROR, "AssetBaseMgr::findAsset: invalid file hash id" );
		return 0;
	}

	std::vector<AssetBaseInfo*>::iterator iter;
	for( iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
		if( (*iter)->getAssetHashId() == fileHashId )
		{
			return (*iter);
		}
	}

	return 0;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::findAsset( VxGUID& assetId )
{
	if( false == assetId.isVxGUIDValid() )
	{
		//LogMsg( LOG_ERROR, "AssetBaseMgr::findAsset: invalid VxGUID asset id\n" );
        return nullptr;
	}

	for( AssetBaseInfo* assetInfo : m_AssetBaseInfoList )
	{
		if( assetInfo->getAssetUniqueId() == assetId )
		{
			return assetInfo;
		}
	}

	return nullptr;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::addAssetFile( EAssetType assetType, const char* fileName, const char* fileNameAndPath, uint64_t fileLen )
{
	VxGUID assetId;
	assetId.initializeWithNewVxGUID();
    AssetBaseInfo* assetInfo = createAssetInfo( assetType, fileName, fileNameAndPath, fileLen, assetId );
    if( assetInfo )
    {
		if( !assetInfo->isValid() )
		{
			return nullptr;
		}

        if( insertNewInfo( assetInfo ) )
        {
            return assetInfo;
        }
    }

    return nullptr;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::addAssetFile( EAssetType assetType, const char* fileName, const char* fileNameAndPath, uint64_t fileLen, VxGUID& assetId )
{
    AssetBaseInfo* assetInfo = createAssetInfo( assetType, fileName, fileNameAndPath, fileLen, assetId );
	if( assetInfo )
	{
		if( !assetInfo->isValid() )
		{
			return nullptr;
		}

		if( insertNewInfo( assetInfo ) )
		{
			return assetInfo;
		}
	}

	return nullptr;
}

//============================================================================
bool AssetBaseMgr::addAssetFile(	EAssetType      assetType,
                                    const char*		fileName, 
                                    const char*		fileNameAndPath,
									VxGUID&			assetId,  
									uint8_t *		hashId, 
									EAssetLocation	locationFlags, 
									const char*		assetTag, 
                                    int64_t			timestamp )
{
    AssetBaseInfo* assetInfo = createAssetInfo( assetType, fileName, fileNameAndPath, assetId, hashId, locationFlags, assetTag, timestamp );
	if( assetInfo )
	{
		if( !assetInfo->isValid() )
		{
            return false;
		}

		return insertNewInfo( assetInfo );
	}
	
	return false;
}

//============================================================================
bool AssetBaseMgr::addAssetFile(	EAssetType      assetType,
                                    const char*		fileName, 
                                    const char*		fileNameAndPath,
									VxGUID&			assetId,  
									VxGUID&		    creatorId, 
									VxGUID&		    historyId, 
									uint8_t *		hashId, 
									EAssetLocation	locationFlags, 
									const char*		assetTag, 
                                    int64_t			timestamp )
{
    AssetBaseInfo* assetInfo = createAssetInfo( assetType, fileName, fileNameAndPath, assetId, hashId, locationFlags, assetTag, timestamp );
	if( assetInfo )
	{
		assetInfo->setCreatorId( creatorId );
		assetInfo->setHistoryId( historyId );

		if( !assetInfo->isValid() )
		{
            return false;
		}

		return insertNewInfo( assetInfo );
	}
	
	return false;
}

//============================================================================
bool AssetBaseMgr::addAsset( AssetBaseInfo& assetInfo, AssetBaseInfo*& retCreatedAsset )
{
	if( !assetInfo.isValid() )
	{
		return false;
	}

	AssetBaseInfo* newAssetBaseInfo = createAssetInfo( assetInfo );
	LogMsg( LOG_VERBOSE, "AssetBaseMgr::addAsset" );

	if( newAssetBaseInfo != nullptr && insertNewInfo( newAssetBaseInfo ) )
	{
		retCreatedAsset = newAssetBaseInfo;
		return true;
	}
	else
	{
		return false;
	}
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::createAssetInfo( EAssetType assetType, const char* fileName, const char* fileNameAndPath, uint64_t fileLen )
{
    AssetBaseInfo* assetInfo = new AssetBaseInfo( assetType, fileName, fileNameAndPath, fileLen );
    if( assetInfo )
    {
        assetInfo->getAssetUniqueId().initializeWithNewVxGUID();
    }

	assetInfo->assureHasCreatorId();
    return assetInfo;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::createAssetInfo( EAssetType assetType, const char* fileName, const char* fileNameAndPath, uint64_t fileLen, VxGUID& assetId )
{
    AssetBaseInfo* assetInfo = new AssetBaseInfo( assetType, fileName, fileNameAndPath, fileLen, assetId );
	assetInfo->assureHasCreatorId();
	return assetInfo;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::createAssetInfo( FileInfo& fileInfo )
{
	AssetBaseInfo* assetInfo = new AssetBaseInfo( fileInfo );
	assetInfo->assureHasCreatorId();
	return assetInfo;
}

//============================================================================
AssetBaseInfo* AssetBaseMgr::createAssetInfo( 	EAssetType      assetType, 
                                                const char*		fileName, 
                                                const char*     fileNameAndPath,
										        VxGUID&			assetId,  
										        uint8_t *	    hashId, 
										        EAssetLocation	locationFlags, 
										        const char*		assetTag, 
                                                int64_t			timestamp )
{
    uint64_t  fileLen = VxFileUtil::getFileLen( fileNameAndPath );
	if( ( false == isAllowedFileOrDir( fileName ) )
		|| ( 0 == fileLen ) )
	{
		LogMsg( LOG_ERROR, "ERROR %d AssetBaseMgr::createAssetInfo could not get file info %s", VxGetLastError(), fileName );
		return NULL;
	}

    AssetBaseInfo* assetInfo = createAssetInfo( assetType, fileName, fileNameAndPath, fileLen );
	assetInfo->setAssetUniqueId( assetId );
	if( false == assetInfo->getAssetUniqueId().isVxGUIDValid() )
	{
		assetInfo->getAssetUniqueId().initializeWithNewVxGUID();
	}

	assetInfo->getAssetHashId().setHashData( hashId );
	assetInfo->setLocationFlags( locationFlags );
	assetInfo->setAssetTag( assetTag );
	assetInfo->setCreationTime( timestamp ? timestamp : GetTimeStampMs() );
	assetInfo->assureHasCreatorId();

	return assetInfo;
}

//============================================================================
bool AssetBaseMgr::insertNewInfo( AssetBaseInfo* assetInfo )
{
	if( !assetInfo->isValid() )
	{
		return false;
	}

	bool result = false;
	AssetBaseInfo* assetInfoExisting = findAsset( assetInfo->getAssetUniqueId() );
	if( assetInfoExisting )
	{
		LogMsg( LOG_ERROR, "ERROR AssetBaseMgr::insertNewInfo: duplicate assset %s", assetInfo->getAssetName().c_str() );
		if( assetInfoExisting != assetInfo )
		{
            // update existing asset
			*assetInfoExisting = *assetInfo;
		}
	}

	if( 0 == assetInfo->getCreationTime() )
	{
		assetInfo->setCreationTime( GetTimeStampMs() );
	}

	//if( assetInfo->needsHashGenerated() )
	//{
	//	lockResources();
	//	m_WaitingForHastList.push_back( assetInfo );
	//	unlockResources();
    //	generateHashForFile( assetInfo->getFileNameAndPath() );
	//	result = true;
	//}
	//else
	{
        updateDatabase( assetInfo );
		if( !assetInfoExisting )
		{
			lockResources();
			m_AssetBaseInfoList.emplace_back( assetInfo );
			unlockResources();
			announceAssetAdded( assetInfo );
		}
        else
        {
            announceAssetUpdated( assetInfo );
        }
	
		result = true;
	}

	return result;
}

//============================================================================
bool AssetBaseMgr::updateAsset( AssetBaseInfo& assetInfo )
{
	if( !assetInfo.isValid() )
	{
		return false;
	}

    AssetBaseInfo* existingAsset = findAsset( assetInfo.getAssetUniqueId() );
    if( existingAsset )
    {
        *existingAsset = assetInfo;
        updateDatabase( existingAsset );
        announceAssetUpdated( existingAsset );
        return true;
    }

    return false;
}

//============================================================================
bool AssetBaseMgr::updateAsset( FileInfo& fileInfo )
{
	AssetBaseInfo* existingAsset = findAsset( fileInfo.getAssetId() );
    if( existingAsset )
    {
		bool updateDb{ false };
		if( !existingAsset->isInLibrary() && fileInfo.getIsInLibrary() )
		{
			existingAsset->setIsInLibrary( fileInfo.getIsInLibrary() );
			updateDb = true;
		}

		if( !existingAsset->isSharedFileAsset() && fileInfo.getIsSharedFile() )
		{
			existingAsset->setIsSharedFileAsset( fileInfo.getIsSharedFile() );
			updateDb = true;
		}

		if( updateDb )
		{
			updateDatabase( existingAsset );
		}
    }
	else
	{
		AssetBaseInfo* newAssetInfo = createAssetInfo( fileInfo );
		newAssetInfo->setCreationTime( GetGmtTimeMs() );
		newAssetInfo->setModifiedTime( GetGmtTimeMs() );
		insertNewInfo( newAssetInfo );
	}

	return true;
}

//============================================================================
void AssetBaseMgr::announceAssetAdded( AssetBaseInfo* assetInfo, bool resourceLocked )
{
	if( !assetInfo->isValid() )
	{
		return;
	}

	// LogMsg( LOG_VERBOSE, "AssetBaseMgr::announceAssetAdded start" );
	if( assetInfo->isFileAsset() )
	{
		updateFileListPackets(resourceLocked);
		updateAssetFileTypes(resourceLocked);
	}
	
	lockClientList();
	std::vector<AssetBaseCallbackInterface *>::iterator iter;
	for( iter = m_AssetClients.begin();	iter != m_AssetClients.end(); ++iter )
	{
		AssetBaseCallbackInterface * client = *iter;
		client->callbackAssetAdded( assetInfo );
	}

	m_Engine.getToGui().toGuiAssetAdded( assetInfo );
	unlockClientList();
	// LogMsg( LOG_VERBOSE, "AssetBaseMgr::announceAssetAdded done" );
}

//============================================================================
void AssetBaseMgr::announceAssetUpdated( AssetBaseInfo* assetInfo )
{
	if( !assetInfo->isValid() )
	{
		return;
	}

    // LogMsg( LOG_VERBOSE, "AssetBaseMgr::announceAssetUpdated start" );
    lockClientList();
    for( auto& client : m_AssetClients )
    {
        client->callbackAssetAdded( assetInfo );
    }

	m_Engine.getToGui().toGuiAssetUpdated( assetInfo );
    unlockClientList();
    // LogMsg( LOG_VERBOSE, "AssetBaseMgr::announceAssetUpdated done" );
}

//============================================================================
void AssetBaseMgr::announceAssetRemoved( AssetBaseInfo* assetInfo, bool resourceLocked )
{
	//if( assetInfo->getIsAssetBase() )
	{
		updateFileListPackets(resourceLocked);
		updateAssetFileTypes(resourceLocked);
	}

	lockClientList();
    for( auto& client : m_AssetClients )
    {
		client->callbackAssetRemoved( assetInfo );
	}

	m_Engine.getToGui().toGuiAssetRemoved( assetInfo );
	unlockClientList();
}

//============================================================================
void AssetBaseMgr::announceAssetXferState( VxGUID& sendToId, VxGUID& assetUniqueId, EAssetSendState assetSendState, int param )
{
	LogMsg( LOG_VERBOSE, "AssetBaseMgr::announceAssetXferState state %d start", assetSendState );
	lockClientList();
    for( auto& client : m_AssetClients )
    {
		client->callbackAssetSendState( sendToId, assetUniqueId, assetSendState, param );
	}

	m_Engine.getToGui().toGuiAssetXferState( assetUniqueId, assetSendState, param );
	unlockClientList();
	LogMsg( LOG_VERBOSE, "AssetBaseMgr::announceAssetXferState state %d done", assetSendState );
}

//============================================================================
bool AssetBaseMgr::removeAsset( std::string fileNameAndPath, bool deleteFile )
{
	bool assetRemoved = false;
	std::vector<AssetBaseInfo*>::iterator iter;
	for( iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
        if( fileNameAndPath == (*iter)->getAssetNameAndPath() )
		{
			AssetBaseInfo* assetInfo = *iter;
			m_AssetBaseInfoList.erase( iter );
            m_AssetBaseInfoDb.removeAsset( fileNameAndPath.c_str() );
			announceAssetRemoved( assetInfo );
            if( deleteFile && ( assetInfo->isThumbAsset() || assetInfo->isFileAsset() ) )
			{
                VxFileUtil::deleteFile( assetInfo->getFileNameAndPath().c_str() );
			}

			delete assetInfo;
			assetRemoved = true;
			break;
		}
	}

	return assetRemoved;
}

//============================================================================
bool AssetBaseMgr::removeAsset( VxGUID& assetUniqueId, bool deleteFile )
{
	m_Engine.getSendQueueMgr().removeAsset( assetUniqueId );
	bool assetRemoved = false;
	std::vector<AssetBaseInfo*>::iterator iter;
	for ( iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
		if( assetUniqueId == ( *iter )->getAssetUniqueId() )
		{
			AssetBaseInfo* assetInfo = *iter;
            std::string fileName = assetInfo->getAssetNameAndPath();
			m_AssetBaseInfoList.erase( iter );
			m_AssetBaseInfoDb.removeAsset( assetInfo );
			announceAssetRemoved( assetInfo );
            if( deleteFile && ( assetInfo->isThumbAsset() || assetInfo->isFileAsset() ) )
			{
				GetVxFileShredder().shredFile( fileName );
			}

			delete assetInfo;
			assetRemoved = true;
			break;
		}
	}

	return assetRemoved;
}

//============================================================================
void AssetBaseMgr::clearAssetInfoList( void )
{
	for( auto iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
		delete (*iter);
	}

	m_AssetBaseInfoList.clear();
}

//============================================================================
void AssetBaseMgr::updateAssetListFromDb( VxThread* startupThread )
{
	std::vector<AssetBaseInfo*> toDeleteList;
	lockResources();
	clearAssetInfoList();
	m_AssetBaseInfoDb.getAllAssets( m_AssetBaseInfoList );

	// there should not be any without valid hash but if is then generate it
	for( auto iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end();  )
	{
		if( startupThread->isAborted() )
		{
			unlockResources();
			return;
		}

		AssetBaseInfo* assetInfo = (*iter);
		if( !assetInfo->validateAssetExist() )
		{
			// add to list to remove from database and delete
			toDeleteList.emplace_back( assetInfo );
			++iter;
			continue;
		}

		if( !assetInfo->isValid() )
		{
			toDeleteList.emplace_back( assetInfo );
			++iter;
			continue;
		}

		if( assetInfo->isSharedFileAsset() )
		{
			m_Engine.getPluginFileShareServer().fileShareEnable( assetInfo, true );
		}

		EAssetSendState sendState = assetInfo->getAssetSendState();
		if( eAssetSendStateTxProgress == sendState ) 
		{
			assetInfo->setAssetSendState( eAssetSendStateTxFail );
			m_AssetBaseInfoDb.updateAssetSendState( assetInfo->getAssetUniqueId(), eAssetSendStateTxFail );
		}
		else if(  eAssetSendStateRxProgress == sendState  )
		{
			assetInfo->setAssetSendState( eAssetSendStateRxFail );
			m_AssetBaseInfoDb.updateAssetSendState( assetInfo->getAssetUniqueId(), eAssetSendStateRxFail );
		}

		if( assetInfo->needsHashGenerated() )
		{
			m_WaitingForHastList.emplace_back( assetInfo );
			iter = m_AssetBaseInfoList.erase( iter );
            requestFileHash( assetInfo );
		}
		else
		{
			++iter;
		}
	}

	for( auto assetInfo : toDeleteList )
	{
		m_AssetBaseInfoDb.removeAsset( assetInfo );
		if( assetInfo->isThumbAsset() || assetInfo->isFileAsset() )
		{
            GetVxFileShredder().shredFile( assetInfo->getAssetNameAndPath() );
		}
	}

	unlockResources();
	updateFileListPackets();
	updateAssetFileTypes();
}

//============================================================================
void AssetBaseMgr::updateAssetFileTypes( bool resourceLocked )
{
	uint16_t u16FileTypes = 0;
	if( !resourceLocked )
	{
		lockResources();
	}

	for( auto assetInfo : m_AssetBaseInfoList )
	{
		if( assetInfo->isFileAsset() )
		{
			u16FileTypes |= assetInfo->getAssetType();
		}
	}

	if( !resourceLocked )
	{
		unlockResources();
	}

	// ignore extended types
	u16FileTypes = u16FileTypes & 0xff;
	if( m_u16AssetBaseFileTypes != u16FileTypes )
	{
		m_u16AssetBaseFileTypes = u16FileTypes;

		lockClientList();
		for( auto& client : m_AssetClients )
		{
			client->callbackAssetFileTypesChanged( u16FileTypes );
		}

		unlockClientList();
	}
}

//============================================================================
void AssetBaseMgr::updateFileListPackets( bool resourceLocked )
{
	bool hadAssetBaseFiles = m_FileListPackets.size() ? true : false;
	PktFileListReply * pktFileList = 0;
	clearAssetFileListPackets();
	lockFileListPackets();
	if( !resourceLocked )
	{
		lockResources();
	}

	std::vector<AssetBaseInfo*>::iterator iter;
	for( iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
		AssetBaseInfo* assetInfo = (*iter); 
		if( ( false == assetInfo->isFileAsset() ) || ( false == assetInfo->getAssetHashId().isHashValid() ) )
			continue;

		if( 0 == pktFileList )
		{
			pktFileList = new PktFileListReply();
			pktFileList->setListIndex( (uint32_t)m_FileListPackets.size() );
		}

		if( pktFileList->canAddFile( (uint32_t)(assetInfo->getRemoteAssetName().size() + 1) ) )
		{
			pktFileList->addFile(	assetInfo->getAssetHashId(),
									assetInfo->getAssetLength(),
									assetInfo->getAssetType(),
									assetInfo->getRemoteAssetName().c_str() );
		}
		else
		{
			m_FileListPackets.push_back( pktFileList );
			pktFileList = new PktFileListReply();
			pktFileList->setListIndex( (uint32_t)m_FileListPackets.size() );
			pktFileList->addFile(	assetInfo->getAssetHashId(),
									assetInfo->getAssetLength(),
									assetInfo->getAssetType(),
									assetInfo->getAssetName().c_str() );
		}
	}

	if( 0 != pktFileList )
	{
		if( pktFileList->getFileCount() )
		{
			pktFileList->setIsListCompleted( true ); // last pkt in list
			m_FileListPackets.emplace_back( pktFileList );
		}
		else
		{
			delete pktFileList;
		}
	}

	if( !resourceLocked )
	{
		unlockResources();
	}

	unlockFileListPackets();
	if( hadAssetBaseFiles || m_FileListPackets.size() )
	{
		lockClientList();
		std::vector<AssetBaseCallbackInterface *>::iterator iter;
		for( iter = m_AssetClients.begin();	iter != m_AssetClients.end(); ++iter )
		{
			AssetBaseCallbackInterface * client = *iter;
			client->callbackAssetPktFileListUpdated();
		}

		unlockClientList();
	}
}

//============================================================================
void AssetBaseMgr::clearAssetFileListPackets( void )
{
	lockFileListPackets();
	std::vector<PktFileListReply*>::iterator iter;
	for( iter = m_FileListPackets.begin(); iter != m_FileListPackets.end(); ++iter )
	{
		delete (*iter);
	}

	m_FileListPackets.clear();
	unlockFileListPackets();
}

//============================================================================
bool AssetBaseMgr::fromGuiSetFileIsShared( FileInfo& fileInfo, bool shareFile )
{
	lockResources();
	AssetBaseInfo* assetInfo = findAsset( fileInfo.getAssetId() );
	if( assetInfo )
	{
		if( shareFile != assetInfo->isSharedFileAsset() )
		{
			assetInfo->setIsSharedFileAsset( shareFile );
			updateDatabase( assetInfo );
			unlockResources();
			updateAssetFileTypes();
			updateFileListPackets();
			if( !assetInfo->getAssetHashId().isHashValid() )
			{
				requestFileHash( assetInfo );
			}

			m_Engine.getPluginFileShareServer().fileShareEnable( assetInfo, shareFile );

			return true;
		}

		// alread set to same
		unlockResources();
		return true;
	}

	unlockResources();

	if( shareFile )
	{
		// file is not currently AssetBase and should be
		VxGUID guid;
		AssetBaseInfo* assetInfo = createAssetInfo( fileInfo );
		if( assetInfo )
		{
			assetInfo->setIsSharedFileAsset( true );
			insertNewInfo( assetInfo );
			FileInfo fileInfo = assetInfo->getFileInfo();
			m_Engine.getPluginFileShareServer().fromGuiSetFileIsShared( fileInfo, true );
			if( !assetInfo->getAssetHashId().isHashValid() )
			{
				requestFileHash( assetInfo );
			}

			m_Engine.getPluginFileShareServer().fileShareEnable( assetInfo, shareFile );
		}
	}
	else
	{
		return false;
	}

	return true;
}

//============================================================================
bool AssetBaseMgr::fromGuiSetFileIsShared( std::string& fileNameAndPath, bool shareFile )
{
	bool isFound{ false };
	bool isChanged{ false };
	lockResources();
	AssetBaseInfo* assetInfo = findAsset( fileNameAndPath );
	if( assetInfo )
	{
		isFound = true;
		if( shareFile != assetInfo->isSharedFileAsset() )
		{
			isChanged = true;
			assetInfo->setIsSharedFileAsset( shareFile );
			updateDatabase( assetInfo );
		}
	}

	unlockResources();

	return isFound;
}

//============================================================================
bool AssetBaseMgr::fromGuiGetFileIsShared( std::string& fileNameAndPath )
{
	bool isShared{ false };
	lockResources();
	AssetBaseInfo* assetInfo = findAsset( fileNameAndPath );
	if( assetInfo )
	{
		isShared = assetInfo->isSharedFileAsset();
	}

	unlockResources();
	return isShared;
}

//============================================================================
bool AssetBaseMgr::fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool isInLibrary )
{
	lockResources();
	AssetBaseInfo* assetInfo = findAsset( fileInfo.getAssetId() );
	if( assetInfo )
	{
		if( isInLibrary != assetInfo->isInLibrary() )
		{
			assetInfo->setIsInLibrary( isInLibrary );
			updateDatabase( assetInfo );
		}

		unlockResources();
		return true;
	}

	unlockResources();

	if( isInLibrary )
	{
		// file is not currently AssetBase and should be
		VxGUID guid;
		AssetBaseInfo* assetInfo = createAssetInfo( fileInfo );
		if( assetInfo )
		{
			assetInfo->setIsInLibrary( true );
			insertNewInfo( assetInfo );
			if( !assetInfo->getAssetHashId().isHashValid() )
			{
				requestFileHash( assetInfo );
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

//============================================================================
bool AssetBaseMgr::fromGuiSetFileIsInLibrary(  std::string& fileNameAndPath, bool isInLibrary )
{
	bool foundAsset{ false };
	lockResources();
	AssetBaseInfo* assetInfo = findAsset( fileNameAndPath );
	if( assetInfo )
	{
		if( isInLibrary != assetInfo->isInLibrary() )
		{
			assetInfo->setIsInLibrary( isInLibrary );
			updateDatabase( assetInfo );
		}

		unlockResources();
		return true;
	}

	unlockResources();

	if( !foundAsset && VxFileUtil::fileExists( fileNameAndPath.c_str() ) )
	{
		// create the asset for this file and put in library
		// file is not currently AssetBase and should be
		FileInfo fileInfo;
		if( VxFileUtil::getFileInfo( fileNameAndPath.c_str(), fileInfo ) )
		{
			VxGUID guid;
			AssetBaseInfo* assetInfo = createAssetInfo( fileInfo );
			if( assetInfo )
			{
				assetInfo->setIsInLibrary( true );
				insertNewInfo( assetInfo );
				if( !assetInfo->getAssetHashId().isHashValid() )
				{
					requestFileHash( assetInfo );
				}
			}
			else
			{
				LogMsg( LOG_ERROR, "AssetBaseMgr::%s failed createAssetInfo for %s", __func__, fileNameAndPath.c_str() );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AssetBaseMgr::%s failed getFileInfo for %s", __func__, fileNameAndPath.c_str() );
		}
	}
	else if( !foundAsset )
	{
		LogMsg( LOG_ERROR, "AssetBaseMgr::%s failed file does not exist %s", __func__, fileNameAndPath.c_str() );
	}

	return false;
}

//============================================================================
bool AssetBaseMgr::fromGuiGetFileIsInLibrary( std::string& fileNameAndPath )
{
	bool isInLibrary{ false };
	lockResources();
	AssetBaseInfo* assetInfo = findAsset( fileNameAndPath );
	if( assetInfo )
	{
		isInLibrary = assetInfo->isInLibrary();
	}

	unlockResources();
	return isInLibrary;
}

//============================================================================
void AssetBaseMgr::fromGuiSendFileList( VxGUID& appInstId, uint8_t fileTypeFilter, bool inLibrary, bool isShared )
{
	lockResources();
	for( auto assetInfo : m_AssetBaseInfoList )
	{
		if( 0 != (fileTypeFilter & assetInfo->getFileType()) )
		{
			if( inLibrary && assetInfo->isInLibrary() || isShared && assetInfo->isSharedFileAsset() )
			{
				FileInfo fileInfo = assetInfo->getFileInfo();
				IToGui::getIToGui().toGuiFileList( appInstId, fileInfo );
			}
		}
	}

	unlockResources();
	IToGui::getIToGui().toGuiFileListCompleted( appInstId );
}

//============================================================================
bool AssetBaseMgr::getFileHashId( std::string& fileFullName, VxSha1Hash& retFileHashId )
{
	bool foundHash = false;
	lockResources();
	for( auto assetInfo : m_AssetBaseInfoList )
	{
        if( fileFullName == assetInfo->getAssetNameAndPath() )
		{
			retFileHashId = assetInfo->getAssetHashId();
			foundHash = retFileHashId.isHashValid();
			break;
		}
	}

	unlockResources();
	return foundHash;
}

//============================================================================
bool AssetBaseMgr::getFileFullName( VxSha1Hash& fileHashId, std::string& retFileFullName )
{
	bool isAssetBase = false;
	lockResources();
	std::vector<AssetBaseInfo*>::iterator iter;
	for( iter = m_AssetBaseInfoList.begin(); iter != m_AssetBaseInfoList.end(); ++iter )
	{
		if( fileHashId == (*iter)->getAssetHashId() )
		{
			isAssetBase = true;
            retFileFullName = (*iter)->getAssetNameAndPath();
			break;
		}
	}

	unlockResources();
	return isAssetBase;
}

//============================================================================
void AssetBaseMgr::updateDatabase( AssetBaseInfo* assetInfo )
{
	if( !assetInfo->isValid() )
	{
		return;
	}

	m_AssetBaseInfoDb.addAsset( assetInfo );
}

//============================================================================
void AssetBaseMgr::updateAssetDatabaseSendState( VxGUID& assetUniqueId, enum EAssetSendState sendState )
{
	m_AssetBaseInfoDb.updateAssetSendState( assetUniqueId, sendState );
}

//============================================================================
void AssetBaseMgr::fromGuiQuerySessionHistory( GroupieId& groupieId )
{
	// if we send all now while in the gui thread we may overflow the stack because qt cannot process while in this call
	// instead spin up a thread to send the assets
	lockResources();
	m_HistorySendList.emplace_back( groupieId );
	unlockResources();

	if( !m_HistoryListThread.isThreadRunning() )
	{
		m_HistoryListThread.startThread( (VX_THREAD_FUNCTION_T)AssetBaseMgrHistoryListThreadFunc, this, "AssetBaseMgrHistoryList" );
	}	
}

//============================================================================
void AssetBaseMgr::sendHistoryAssetsToGuiByThread( VxThread* poThread )
{
	while( !m_HistorySendList.empty() && !VxIsAppShuttingDown() )
	{
		lockResources();
		if( m_HistorySendList.empty() )
		{
			unlockResources();
			break;
		}

		GroupieId groupieId = m_HistorySendList.front();
		m_HistorySendList.erase( m_HistorySendList.begin() );
		if( groupieId.getHostedId().isValid() )
		{
			for( auto assetInfo : m_AssetBaseInfoList )
			{
				if( assetInfo->isHistoryMatch( groupieId ) )
				{
					onQueryHistoryAsset( assetInfo );				
				}
			}
		}

		unlockResources();
	}
}

//============================================================================
void AssetBaseMgr::updateAssetXferState( VxGUID& sendToId, VxGUID& assetUniqueId, EAssetSendState assetSendState, int param )
{
	switch( assetSendState )
	{
	case eAssetSendStateRxFail:
	case eAssetSendStateTxFail:
	case eAssetSendStateTxPermissionErr:
	case eAssetSendStateRxPermissionErr:
		LogMsg( LOG_VERBOSE, "AssetBaseMgr::updateAssetXferState FAILED %s state %s", assetUniqueId.toOnlineIdString().c_str(), DescribeAssetSendState( assetSendState ) );
		break;

	default:
		LogMsg( LOG_VERBOSE, "AssetBaseMgr::updateAssetXferState %s state %s", assetUniqueId.toOnlineIdString().c_str(), DescribeAssetSendState( assetSendState ) );
	}

	bool assetSendStateChanged{ false };
	bool assetSendStateFound{ false };
	lockResources();
	for( auto assetInfo : m_AssetBaseInfoList )
	{
		if( assetInfo->getAssetUniqueId() == assetUniqueId )
		{
			assetSendStateFound = true;
			EAssetSendState oldSendState = assetInfo->getAssetSendState();
			if( oldSendState != assetSendState || 
				( eAssetSendStateTxProgress == assetSendState || eAssetSendStateRxProgress == assetSendState ) )
			{
				assetInfo->setAssetSendState( assetSendState );
				assetSendStateChanged = true;

				updateAssetDatabaseSendState( assetUniqueId, assetSendState );
				switch( assetSendState )
				{
				case eAssetSendStateTxProgress:
					if( eAssetSendStateNone == oldSendState )
					{
						IToGui::getIToGui().toGuiAssetAction( eAssetActionTxBegin, assetUniqueId, param );
					}

					IToGui::getIToGui().toGuiAssetAction( eAssetActionTxProgress, assetUniqueId, param );
					break;

				case eAssetSendStateRxProgress:
					if( eAssetSendStateNone == oldSendState )
					{
						IToGui::getIToGui().toGuiAssetAction( eAssetActionRxBegin, assetUniqueId, param );
					}

					IToGui::getIToGui().toGuiAssetAction( eAssetActionRxProgress, assetUniqueId, param );
					break;

				case eAssetSendStateRxSuccess:
					if( ( eAssetSendStateNone == oldSendState )
						|| ( eAssetSendStateRxProgress == oldSendState ) )
					{
						IToGui::getIToGui().toGuiAssetAction( eAssetActionRxSuccess, assetUniqueId, param );
						IToGui::getIToGui().toGuiAssetAction( eAssetActionRxNotifyNewMsg, assetInfo->getCreatorId(), 100 );
					}
					else 
					{
						IToGui::getIToGui().toGuiAssetAction( eAssetActionRxSuccess, assetUniqueId, param );
					}

					break;

				case eAssetSendStateTxSuccess:
					IToGui::getIToGui().toGuiAssetAction( eAssetActionTxSuccess, assetUniqueId, param );
					break;

				case eAssetSendStateRxFail:
					IToGui::getIToGui().toGuiAssetAction( eAssetActionRxError, assetUniqueId, param );
					break;

				case eAssetSendStateTxFail:
					IToGui::getIToGui().toGuiAssetAction( eAssetActionTxError, assetUniqueId, param );
					break;

				case eAssetSendStateTxPermissionErr:
					IToGui::getIToGui().toGuiAssetAction( eAssetActionTxError, assetUniqueId, param );
					break;

				case eAssetSendStateRxPermissionErr:
					IToGui::getIToGui().toGuiAssetAction( eAssetActionRxError, assetUniqueId, param );
					break;


				case eAssetSendStateNone:
				default:
					break;
				}
			}

			break;
		}
	}

	if( !assetSendStateFound )
	{
		LogMsg( LOG_ERROR, "AssetBaseMgr::updateAssetXferState asset not found" );
	}

	unlockResources();
	if( assetSendStateChanged )
	{
		announceAssetXferState( sendToId, assetUniqueId, assetSendState, param );
	}

	LogMsg( LOG_VERBOSE, "AssetBaseMgr::updateAssetXferState state %d done", assetSendState );
}

//============================================================================
bool AssetBaseMgr::isAllowedFileOrDir( std::string strFileName )
{
    if( VxIsExecutableFile( strFileName ) 
        || VxIsShortcutFile( strFileName ) )
    {
        return false;
    }

    return true;
}

//============================================================================
void AssetBaseMgr::addAssetMgrClient( AssetBaseCallbackInterface * client, bool enable )
{
    AutoResourceLock( this );
    if( enable )
    {
        m_AssetClients.emplace_back( client );
    }
    else
    {
        std::vector<AssetBaseCallbackInterface *>::iterator iter;
        for( iter = m_AssetClients.begin(); iter != m_AssetClients.end(); ++iter )
        {
            if( *iter == client )
            {
                m_AssetClients.erase( iter );
                return;
            }
        }
    }
}

//============================================================================
void AssetBaseMgr::onQueryHistoryAsset( AssetBaseInfo* assetInfo )
{
	m_Engine.getToGui().toGuiAssetSessionHistory( assetInfo );
}

//============================================================================
bool AssetBaseMgr::fromGuiQueryFileHash( FileInfo& fileInfo )
{
	bool result{ false };
	lockResources();
	for( auto* assetInfo : m_AssetBaseInfoList )
	{
        if( fileInfo.getFileLength() == assetInfo->getAssetLength() && fileInfo.getFileNameAndPath() == assetInfo->getAssetNameAndPath() )
		{
			if( assetInfo->getAssetHashId().isHashValid() )
			{
				fileInfo.setFileHashId( assetInfo->getAssetHashId() );
				result = true;
			}

			break;
		}
	}

	unlockResources();
	return result;
}

//============================================================================
void AssetBaseMgr::getStreamableAssets( std::vector<AssetBaseInfo>& streamableAssets )
{
	streamableAssets.clear();

	lockResources();
	for( auto* assetInfo : m_AssetBaseInfoList )
	{
        if( assetInfo->getIsAssetStreamable() && assetInfo->isSharedFileAsset() )
		{
			streamableAssets.emplace_back( *assetInfo );
		}
	}

	unlockResources();
}

//============================================================================
void AssetBaseMgr::getSharedFiles( std::vector<AssetBaseInfo>& sharedFiles )
{
	sharedFiles.clear();
	int assetIdx{ 0 };

	lockResources();
	for( auto* assetInfo : m_AssetBaseInfoList )
	{
		if( assetInfo->isPhotoAsset() || assetInfo->isVideoAsset() || assetInfo->isAudioAsset() )
		{
			if( assetInfo->isSharedFileAsset() )
			{
				sharedFiles.emplace_back( *assetInfo );
				assetIdx++;
			}
		}
	}

	LogMsg( LOG_VERBOSE, "%s %d of %d assets are shared", __func__, assetIdx, m_AssetBaseInfoList.size() );

	unlockResources();
}

//============================================================================
void AssetBaseMgr::deleteFile( std::string fileNameAndPath, bool shredFile )
{
	// remove from library
	m_Engine.getPluginLibraryServer().fromGuiSetFileIsInLibrary( fileNameAndPath, false );
	// remove from shared files
	m_Engine.getPluginFileShareServer().fromGuiSetFileIsShared( fileNameAndPath, false );
	// remove from transfers
	m_Engine.getPluginFileShareServer().fileAboutToBeDeleted( fileNameAndPath );
	// remove from assets
	removeAsset( fileNameAndPath, !shredFile );
	if( shredFile )
	{
		m_FileShredder.shredFile( fileNameAndPath );
	}
}

//============================================================================
void AssetBaseMgr::fromGuiFileHashGenerated( std::string& fileNameAndPath, int64_t fileLen, VxSha1Hash& fileHash )
{
	lockResources();
	for( auto* assetInfo : m_AssetBaseInfoList )
	{
        if( fileLen == assetInfo->getAssetLength() && fileNameAndPath == assetInfo->getAssetNameAndPath() )
		{
			assetInfo->setAssetHashId( fileHash );
			updateDatabase( assetInfo );
			break;
		}
	}

	unlockResources();
}

//============================================================================
void AssetBaseMgr::requestFileHash( AssetBaseInfo* assetInfo )
{
	GetSha1GeneratorMgr().generateSha1( assetInfo->getAssetUniqueId(), assetInfo->getAssetName(), assetInfo->getFileNameAndPath(), this);
}

//============================================================================
void AssetBaseMgr::callbackSha1GenerateResult( ESha1GenResult sha1GenResult, VxGUID& assetId, Sha1Info& sha1Info )
{
	if( eSha1GenResultNoError == sha1GenResult )
	{
		lockResources();

		// move from waiting to completed
		bool wasMoved{ false };
		for( auto iter = m_WaitingForHastList.begin(); iter != m_WaitingForHastList.end(); ++iter )
		{
			AssetBaseInfo* inListAssetBaseInfo = *iter;
			if( assetId == inListAssetBaseInfo->getAssetUniqueId() && sha1Info.getFileNameAndPath() == inListAssetBaseInfo->getAssetNameAndPath() )
			{
				AssetBaseInfo* toMoveAssetInfo = inListAssetBaseInfo;
				m_WaitingForHastList.erase( iter );
				toMoveAssetInfo->setAssetHashId( sha1Info.getSha1Hash() );
				m_AssetBaseInfoList.emplace_back( toMoveAssetInfo );
				updateDatabase( toMoveAssetInfo );
				announceAssetAdded( toMoveAssetInfo, true );
				wasMoved = true;
				break;
			}
		}

		bool wasFound{ false };
		if( !wasMoved )
		{
			for( auto* assetInfo : m_AssetBaseInfoList )
			{
				if( assetId == assetInfo->getAssetUniqueId() && sha1Info.getFileNameAndPath() == assetInfo->getAssetNameAndPath() )
				{
					assetInfo->setAssetHashId( sha1Info.getSha1Hash() );
					updateDatabase( assetInfo );
					wasFound = true;
					break;
				}
			}
		}

		unlockResources();
		if( wasMoved || wasFound )
		{
			m_Engine.getPluginFileShareServer().fromGuiFileHashGenerated( sha1Info.getFileNameAndPath(), sha1Info.getFileLen(), sha1Info.getSha1Hash() );
		}
	}
	else
	{
		LogMsg( LOG_VERBOSE, "AssetBaseMgr::%s failed %s", __func__, DescribeSha1GenResult( sha1GenResult ) );
	}
}

/*
//============================================================================
RCODE AssetBaseMgr::SendMatchList(	uint32_t			u32SktNum,		// Tronacom socket number
									CString &	csMatchString,	// Search Match name
									uint16_t			u16FileTypes,	// types of files to match
									uint16_t			u16LimitType,	// file size limit type
									uint64_t			u64SizeLimit,	// file size limit
									bool		bCloseAfterSend,// if true close the socket after send
									bool		bEncrypt )		// if true encrypt
{	
	int i;
	RCODE rc = 0;

	PktShareFileList gPktList;
	char as8FileName[ VX_MAX_PATH ];
	char as8TmpFileName[ VX_MAX_PATH ];
	long s32PktIdx = 0;
	unsigned long u32FileLen = 0;
	bool bFileOpen = false;
	FILE * pgFile = 0;
	CStringArray acsMatchTokens;
	bool bSendAll = false;
	ShareFileInfo * poInfo;

	if( 1 == csMatchString.GetLength() && 
		'*' == csMatchString[ 0 ] )
	{
		bSendAll = true; //send complete list
	}
	else
	{

		rc = ParseSearchString(	csMatchString,		//String of search tokens
							acsMatchTokens );	//return seperate token in each String
	}
	if( rc )
	{
		LogMsg( LOG_VERBOSE, "AssetBaseMgr::SendMatchList Error 0x%x occured parsing string\n", rc );
		ASSERT( false );
	}
	else
	{
		int	iCnt = m_List.GetSize();
		for( i = 0; i < iCnt; i++ )
		{
			poInfo = &m_List[ i ];
			if( 0 == ( u16FileTypes & poInfo->m_u32Flags ) )
			{
				//is not the file type we are searching for
				continue;
			}
			if( bSendAll ||
				Match( acsMatchTokens, poInfo->m_csDisplayName ) )
			{
				if( 0 == poInfo->m_s64FileLen )
				{
					LogMsg( LOG_VERBOSE, "AssetBaseMgr::Zero Length File %s\n", (const char*) poInfo->m_csDisplayName );
					continue;
				}
				//add file to PktFileList
				unsigned char * pu8Temp = poInfo->m_pu8ChoppedDisplayName;
				unsigned long u32ItemLen = * pu8Temp + sizeof( __int64 );
				if( gPktList.m_u16PktLen + u32ItemLen > sizeof( PktShareFileList ) )
				{
					//wont fit in this Pkt so store and start another
					gPktList.m_u16PktLen = (( gPktList.m_u16PktLen + 15 ) & ~15 );
					if( false == bFileOpen )
					{	
						tmpnam( as8TmpFileName );
						strcpy( as8FileName, g_oGlobals.m_csExeDir );
						strcat( as8FileName, "TcFileShareRes" );
						strcat( as8FileName, as8TmpFileName );
						strcat( as8FileName, "tmp" );
						pgFile = fopen( as8FileName, "wb+" );
						if( 0 == pgFile )
						{
							rc = FILE_OPEN_ERR;
							break;
						}
						bFileOpen = true;
					}

					if( (unsigned long)gPktList.m_u16PktLen != fwrite( &gPktList, 
																		1, 
																		(unsigned long)gPktList.m_u16PktLen, 
																		pgFile ) )
					{
						rc = FILE_WRITE_ERR;
					}
					u32FileLen += gPktList.m_u16PktLen;
					gPktList.m_u16PktLen = gPktList.emptyLen();
					s32PktIdx = 0;
					gPktList.m_u16ListCnt = 0;
				}
				//copy file length first
				*( ( __int64 *)&gPktList.m_au8List[ s32PktIdx ] ) = poInfo->m_s64FileLen;
				//copy chopped string that has display name
				memcpy( &gPktList.m_au8List[ s32PktIdx + sizeof( __int64 ) ], poInfo->m_pu8ChoppedDisplayName, u32ItemLen - sizeof( __int64 ) );

				//show trace of file we added to list
				char as8Buf[ 256 * 2 ];
				VxUnchopStr( poInfo->m_pu8ChoppedDisplayName, as8Buf );
				LogMsg( LOG_VERBOSE, "Added to PktFileList file size %d file %s\n", 
						*( ( unsigned long *)&gPktList.m_au8List[ s32PktIdx ] ),
						as8Buf );


				//increment vars for next item
				s32PktIdx += u32ItemLen;
				gPktList.m_u16ListCnt++;
				gPktList.m_u16PktLen += (unsigned short)u32ItemLen;
			}	
		}
	}
	if( pgFile )
	{
		if( u32FileLen )
		{
			//we allready have at least one pkt in file
			//so add this one if has data
			if( s32PktIdx )
			{
				//we have a pkt with data
				gPktList.m_u16PktLen = (( gPktList.m_u16PktLen + 15 ) & ~15 );
			
				//flush Pkt to file
				if( (unsigned long)gPktList.m_u16PktLen != fwrite( &gPktList, 
																	1, 
																	(unsigned long)gPktList.m_u16PktLen, 
																	pgFile ) )
				{
					rc = FILE_WRITE_ERR;
				}
				u32FileLen += gPktList.m_u16PktLen;
				if( u32FileLen & 0x0f )
				{
					//file length is not on 16 byte boundery so fill in a little
					if( 16 - (u32FileLen & 0x0f) != fwrite( &gPktList, 
																		16 - (u32FileLen & 0x0f),
																		(unsigned long)gPktList.m_u16PktLen, 
																		pgFile ) )
					{
						rc = FILE_WRITE_ERR;
					}
				}
				fclose( pgFile );
			}
			//use threaded file send to send list
			//this uses a separate thread to send file containing packets
			rc = g_poApi->SktSendFile(	u32SktNum,// Socket to send on
							as8FileName,	// packet to be sent via tcp connection
							0,	//offset into file to start at
							4096, //size of pkt chunks
							-2,	// PktType to put in header 	
								// positive value..add header of u16PktType to each chunk sent
								//  -1 = use no headers..send file in chunks of u32PktSize in length
								//	-2 = file is file of packets add value in size of pkt chunks to pkt types
							0, //instance value if has header
							true,		// if true delete file after it is sent
							true,		// if true disconnect after send
							0, //user data for callback
							NULL );		// callback from thread with File Send status
		}
	}
	else
	{
		//we havent even written one pkt to file so
		//just send this one
		gPktList.m_u16PktLen = (( gPktList.m_u16PktLen + 15 ) & ~15 );
 
		g_poApi->SktSend( u32SktNum, &gPktList, bCloseAfterSend );
	}
	return rc;
}

//============================================================================
RCODE AssetBaseMgr::AddDir( CString & csDirPath )
{
	HRESULT hr;
	char as8FullPath[ VX_MAX_PATH ];

	struct _stat gStat;
	int i;


    CArray<CString, CString&> acsWildNameList;//Extensions ( file extentions )
	acsWildNameList.Add( CString( "*.*" ) );
	CArray<AssetBaseInfo, AssetBaseInfo&> agAssetBaseInfoList;//return FileInfo in array

	VxFindFilesByName(	csDirPath,				//start path to search in
						acsWildNameList,//Extensions ( file extentions )
						agAssetBaseInfoList,//return FileInfo in array
						true,						//recurse subdirectories if TRUE
						false		//if TRUE dont return files matching filter else return files that do
 						);
	int iCnt = agAssetBaseInfoList.GetSize();
	//expand list to include the new files
	int iCurListCnt = m_List.GetSize();
	m_List.SetSize( iCnt + iCurListCnt );
	int iResolvedListIdx = iCurListCnt;

	for( i = 0; i < iCnt; i++ )
	{
		CString cs = agAssetBaseInfoList[ i ];
		__int64 s64FileLen = agAssetBaseInfoList[ i ].m_s64Len;
		makeForwardSlashPath( cs );

		//get file extension
		CString csExt = getFileExtension( cs );
		char * pExt = (char *)((const char*)csExt);
		bool bIsLink = false;

		if( 0 == strcmp( pExt, "lnk" ) )
		{ 
			bIsLink = true; 
			as8FullPath[ 0 ] = 0;
			hr = VxResolveShortcut(	0,				//handle to window of caller
									cs,	//.lnk file
									as8FullPath );		//return path to target file
			if( FAILED( hr ) )
			{
				TRACE("Unable to resolve path in %s\n", (const char*)cs );
				continue;
			}
			else
			{
				cs = as8FullPath;	
				int iResult = ::_stat( (const char*)cs, &gStat );
				if( iResult != 0 )
				{
					TRACE( "Problem getting file information\n" );
					continue;
				}
				else
				{
					s64FileLen = gStat.st_size;
					csExt = getFileExtension( cs );
					pExt = (char *)((const char*)csExt);
				}
			}
		}
		//weve got a live one so

		unsigned long u32FileFlags = 0;

		if( VxIsPhotoFileExtention( pExt ) )
		{
			m_bHasImage = true;
			u32FileFlags |= FILE_TYPE_PHOTO;
			m_u32FileFlags |= FILE_TYPE_PHOTO;
		}
		else if( VxIsAudioFileExtention( pExt ) )
		{
			m_bHasAudio = true;
			u32FileFlags |= FILE_TYPE_AUDIO;
			m_u32FileFlags |= FILE_TYPE_AUDIO; 
		}
		else if( VxIsVideoFileExtention( pExt ) )
		{
			m_bHasVideo = true;
			u32FileFlags |= FILE_TYPE_VIDEO;
			m_u32FileFlags |= FILE_TYPE_VIDEO; 
		}
		else if( VxIsExecutableFileExtention( pExt ) )
		{
			m_bHasExecutable = true;
			u32FileFlags |= FILE_TYPE_EXECUTABLE;
			m_u32FileFlags |= FILE_TYPE_EXECUTABLE;
		}
		else if( VxIsArchiveOrDocFileExtention( pExt ) )
		{
			m_bHasArchiveOrDoc = true;
			u32FileFlags |= FILE_TYPE_ARCHIVE_OR_DOC;
			m_u32FileFlags |= FILE_TYPE_ARCHIVE_OR_DOC;
		}
		else if( VxIsArchiveOrDocFileExtention( pExt ) )
		{
			m_bHasCdImage = true;
			u32FileFlags |= FILE_TYPE_CD_IMAGE;
			m_u32FileFlags |= FILE_TYPE_CD_IMAGE;
		}
		else
		{
			m_bHasOther = true;
			u32FileFlags |= FILE_TYPE_OTHER;
			m_u32FileFlags |= FILE_TYPE_OTHER;
		}
		//fill in the data
		m_s64TotalByteCnt += s64FileLen;
		ShareFileInfo * poInfo = &m_List[ iResolvedListIdx ];
		poInfo->m_csPathAndName = cs;
		poInfo->m_s64FileLen = s64FileLen;
		poInfo->m_u32Flags = u32FileFlags;
		if( bIsLink )
		{
			//display name is the file name
			poInfo->m_csDisplayName = VxGetFileNameFromFullPath( cs );
		}
		else
		{
			//display name is full path - root path
			char * pTemp = (char *)(const char*)cs;
			int iRootLen =  csDirPath.GetLength();
			ASSERT( iRootLen < cs.GetLength() );
			if( iRootLen )
			{
				if( '\\' == pTemp[ iRootLen - 1 ] )
				{
					//root path has the slash
					poInfo->m_csDisplayName = &pTemp[ iRootLen ];
				}
				else
				{
					//root path doesnt has the slash so add 1 to length
					poInfo->m_csDisplayName = &pTemp[ iRootLen + 1 ];
				}

			}
		}

		unsigned char * pu8ChoppedStr = new unsigned char[ poInfo->m_csDisplayName.GetLength() + 10 ];
		VxChopStr( (char *)(const char*) poInfo->m_csDisplayName, pu8ChoppedStr );
		if( poInfo->m_pu8ChoppedDisplayName )
		{
			delete poInfo->m_pu8ChoppedDisplayName;
		}
		poInfo->m_pu8ChoppedDisplayName = pu8ChoppedStr;
		LogChoppedString( pu8ChoppedStr );
		poInfo = &m_List[0];
		const char* pTemp = (const char*)poInfo->m_csDisplayName;
		
		iResolvedListIdx++;

	}
	m_List.SetSize( iResolvedListIdx );//remove any unused entries
	return 0;
}
//============================================================================
//! make array of packets with matching files
RCODE AssetBaseMgr::MakeMatchList(	CPtrArray * paoRetListPkts, // return list of packets of file lists
									CString &	csMatchString,	// Search Match name
									uint16_t			u16FileTypes,	// types of files to match
									uint16_t			u16LimitType,	// file size limit type
									uint64_t			u64SizeLimit )	// file size limit
{	
	int i;
	RCODE rc = 0;

	PktShareFileList gPktList;
	long s32PktIdx = 0;
	unsigned long u32FileLen = 0;
	bool bFileOpen = false;
	FILE * pgFile = 0;
	CStringArray acsMatchTokens;
	bool bSendAll = false;
	ShareFileInfo * poInfo;

	if( 1 == csMatchString.GetLength() && 
		'*' == csMatchString[ 0 ] )
	{
		bSendAll = true; //send complete list
	}
	else
	{

		rc = ParseSearchString(	csMatchString,		//String of search tokens
							acsMatchTokens );	//return seperate token in each String
	}
	if( rc )
	{
		LogMsg( LOG_VERBOSE, "AssetBaseMgr::SendMatchList Error 0x%x occured parsing string\n", rc );
		ASSERT( false );
	}
	else
	{
		int	iCnt = m_List.GetSize();
		for( i = 0; i < iCnt; i++ )
		{
			poInfo = &m_List[ i ];
			if( 0 == ( u16FileTypes & poInfo->m_u32Flags ) )
			{
				//is not the file type we are searching for
				continue;
			}
			if( bSendAll ||
				Match( acsMatchTokens, poInfo->m_csDisplayName ) )
			{
				if( 0 == poInfo->m_s64FileLen )
				{
					LogMsg( LOG_VERBOSE, "AssetBaseMgr::Zero Length File %s\n", (const char*) poInfo->m_csDisplayName );
					continue;
				}
				//add file to PktFileList
				unsigned char * pu8Temp = poInfo->m_pu8ChoppedDisplayName;
				unsigned long u32ItemLen = * pu8Temp + sizeof( __int64 );
				if( gPktList.m_u16PktLen + u32ItemLen > sizeof( PktShareFileList ) )
				{
					//wont fit in this Pkt so store and start another
					gPktList.m_u16PktLen = (( gPktList.m_u16PktLen + 15 ) & ~15 );
					u32FileLen += gPktList.m_u16PktLen;
					
					paoRetListPkts->Add( gPktList.makeCopy() );

					gPktList.m_u16PktLen = gPktList.emptyLen();
					s32PktIdx = 0;
					gPktList.m_u16ListCnt = 0;
				}
				//copy file length first
				*( ( __int64 *)&gPktList.m_au8List[ s32PktIdx ] ) = poInfo->m_s64FileLen;
				//copy chopped string that has display name
				memcpy( &gPktList.m_au8List[ s32PktIdx + sizeof( __int64 ) ], poInfo->m_pu8ChoppedDisplayName, u32ItemLen - sizeof( __int64 ) );

				//show trace of file we added to list
				char as8Buf[ 256 * 2 ];
				VxUnchopStr( poInfo->m_pu8ChoppedDisplayName, as8Buf );
				LogMsg( LOG_VERBOSE, "Added to PktFileList file size %d file %s\n", 
						*( ( unsigned long *)&gPktList.m_au8List[ s32PktIdx ] ),
						as8Buf );


				//increment vars for next item
				s32PktIdx += u32ItemLen;
				gPktList.m_u16ListCnt++;
				gPktList.m_u16PktLen += (unsigned short)u32ItemLen;
			}	
		}
	}
	gPktList.m_u16PktLen = (( gPktList.m_u16PktLen + 15 ) & ~15 );
	paoRetListPkts->Add( gPktList.makeCopy() );
 	return 0;
}
*/
