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

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <CoreLib/VxDefs.h>
#include <CoreLib/AssetDefs.h>
#include <CoreLib/VxGUID.h>
#include <PktLib/VxCommon.h>

#include <GuiInterface/IDefs.h>

#include <QString>

class VxGUID;
class FileInfo;
class GuiUser;
class GuiOfferSession;
class AssetBaseInfo;
class BlobInfo;
class ThumbInfo;

class ToGuiActivityInterface
{
public:
    virtual void				toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, QString& paramValue ){};

    virtual void				toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) {};
    virtual void				toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId ) {};

    virtual void				toGuiContactAdded( GuiUser* netIdent ){}; 
    virtual void				toGuiContactRemoved( VxGUID& onlineId ) {}; 

	virtual void				toGuiContactOnline( GuiUser* netIdent ){}; 
	virtual void				toGuiContactOffline( GuiUser* guiUser ){}; 

    //virtual void				toGuiContactNameChange( GuiUser* guiUser ){}; 
    //virtual void				toGuiContactDescChange( GuiUser* guiUser ){}; 
    //virtual void				toGuiContactFriendshipChange( GuiUser* guiUser ){}; 

    //virtual void				toGuiPluginPermissionChange( GuiUser* guiUser ){}; 
    //virtual void				toGuiContactSearchFlagsChange( GuiUser* guiUser ){}; 

    virtual void				toGuiContactLastSessionTimeChange( GuiUser* guiUser ){}; 
    virtual void				toGuiUpdateMyIdent(GuiUser* guiUser ){}; 

	virtual void				toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ){}; 

	virtual void				toGuiScanResultSuccess( EScanType eScanType, GuiUser* guiUser ){};
	virtual void				toGuiSearchResultError( EScanType eScanType, GuiUser* guiUser, int errCode ){};
	virtual void				toGuiSearchResultProfilePic( GuiUser* guiUser, uint8_t* pu8JpgData, uint32_t u32JpgDataLen ){};
    virtual void				toGuiSearchResultFileSearch( GuiUser* guiUser, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) {};

	virtual void				toGuiClientScanSearchComplete( EScanType eScanType ){};

    virtual void				toGuiAssetAdded( AssetBaseInfo& assetInfo ) {};
    virtual void				toGuiAssetUpdated( AssetBaseInfo& assetInfo ) {};
    virtual void				toGuiAssetRemoved( AssetBaseInfo& assetInfo ) {};

    virtual void				toGuiAssetXferState( VxGUID& assetUniqueId, EAssetSendState assetSendState, int param ) {};

    virtual void				toGuiAssetSessionHistory( AssetBaseInfo& assetInfo ) {};
	virtual void				toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ){};
	virtual void				toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID onlineId, int pos0to100000 ){};

	virtual void				toGuiSetGameValueVar( EPluginType pluginType, VxGUID& onlineId, int32_t varId, int32_t varValue ){};
	virtual void				toGuiSetGameActionVar( EPluginType pluginType, VxGUID& onlineId, int32_t actionId, int32_t varValue ){};

    virtual void				toGuiClientBlobAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ){};
    virtual void				toGuiBlobAdded( BlobInfo& blobInfo ){};
    virtual void				toGuiBlobSessionHistory( BlobInfo& blobInfo ){};

    virtual void				toGuiThumbAdded( ThumbInfo* assetInfo ){};
    virtual void				toGuiThumbUpdated( ThumbInfo* assetInfo ){};
    virtual void				toGuiThumbRemoved( VxGUID& thumbId ){};

    virtual void				toGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue ) {};
};

