#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <AssetMgr/AssetInfo.h>

class FileInfo;

class OfferBaseInfo : public AssetInfo
{
public:
    OfferBaseInfo() = default;
    OfferBaseInfo( const OfferBaseInfo& rhs );
    OfferBaseInfo( std::string fileName );
    OfferBaseInfo( std::string fileName, uint64_t assetLen, uint16_t assetType );
    OfferBaseInfo( FileInfo& fileInfo );

    OfferBaseInfo& operator=( const OfferBaseInfo& rhs );

    virtual bool				isValid( void ) override;
    virtual bool				isSessionMatch( OfferBaseInfo& rhs );

    virtual bool                addToBlob( PktBlobEntry& blob ) override;
    virtual bool                extractFromBlob( PktBlobEntry& blob ) override;

	virtual void				setOfferType( EOfferType assetType )	{ setAssetType( (EAssetType)assetType ); }
	virtual EOfferType			getOfferType( void )					{ return (EOfferType)getAssetType(); }

    virtual void				setOfferName( std::string& assetName )  { setAssetName( assetName ); }
    virtual std::string&        getOfferName( void )                    { return getAssetName(); }

    virtual void				setOfferLength( int64_t assetLength )   { setAssetLength( assetLength ); }
    virtual int64_t				getOfferLength( void )                  { return getAssetLength(); }

    virtual void				setOfferHashId( VxSha1Hash& hashId )    { setAssetHashId( hashId ); }
    virtual void				setOfferHashId( uint8_t* hashId )       { setAssetHashId( hashId ); }
    virtual VxSha1Hash&         getOfferHashId( void )                  { return getAssetHashId(); }

    virtual void				setOfferTag( std::string assetTag )     { setAssetTag( assetTag.c_str() ); }
    virtual std::string&        getOfferTag( void )                     { return getAssetTag(); }

    virtual void				setOfferSendState( EOfferSendState sendState ) { setAssetSendState( (EAssetSendState)sendState ); }
    virtual EOfferSendState		getOfferSendState( void )               { return (EOfferSendState)getAssetSendState(); }

    virtual std::string         getRemoteOfferName( void )              { return getRemoteAssetName(); }

    virtual bool                isFileOffer( void )                     { return isFileAsset(); }

    virtual void				setIsSharedFileOffer( bool isSharedOffer ) { setIsSharedFileAsset( isSharedOffer ); }
    virtual bool				isSharedFileOffer( void )               { return isSharedFileAsset(); }

    virtual void				setOfferMsg( std::string offerMsg )     { m_OfferMsg = offerMsg; }
    virtual std::string&		getOfferMsg( void )                     { return m_OfferMsg; }

    virtual void				setOfferExpireTime( int64_t expireTime ) { m_OfferExpireTime = expireTime; }
    virtual int64_t             getOfferExpireTime( void )              { return m_OfferExpireTime; }

    virtual void				setOfferId( VxGUID& sessionId )         { m_OfferId = sessionId; }
    virtual void				setOfferId( const char* sessionId )     { m_OfferId.fromVxGUIDHexString( sessionId ); }
    virtual VxGUID&             getOfferId( void )                      { return m_OfferId; }

    virtual void                setOfferResponse( EOfferResponse offerResponse ) { m_OfferResponse = offerResponse; }
    virtual EOfferResponse      getOfferResponse( void )                { return m_OfferResponse; }

    virtual void                setOfferMgr( EOfferMgrType offerMgr )   { m_OfferMgr = offerMgr; }
    virtual EOfferMgrType       getOfferMgr( void )                     { return m_OfferMgr; }

    virtual void                fillOfferSend( EPluginType pluginType, VxNetIdent& netIdent );

    virtual bool				isExpiredOffer( void );

protected:
    EOfferMgrType               m_OfferMgr{ eOfferMgrNotSet };
    VxGUID                      m_OfferId;
    int64_t                     m_OfferExpireTime{ 0 };
    std::string                 m_OfferMsg{ "" };
    EOfferResponse              m_OfferResponse{ eOfferResponseNotSet };
};
