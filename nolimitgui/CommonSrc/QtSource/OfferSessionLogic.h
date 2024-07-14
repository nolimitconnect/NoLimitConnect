#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <OfferBase/OfferBaseInfo.h>

#include <CoreLib/VxSha1Hash.h>

#include <string>

class ActivityBase;
class AppCommon;
class AppletBase;
class GuiOfferMgr;
class GuiOfferSession;
class GuiOfferCallback;
class VxNetIdent;

class OfferSessionLogic
{
public:	
	OfferSessionLogic( AppletBase* appletBase, GuiOfferCallback* callbackInterface, AppCommon& myApp );
	
	bool 						isSessionOffer( void )							{ return m_IsOffer; }
	bool 						isRmtInitiated( void )							{ return m_IsOffer; }
	void 						setIsServerSession( bool isServerSession )		{ m_IsServerSession = isServerSession; }
	bool 						getIsServerSession( void )						{ return m_IsServerSession; }
	void 						setIsInSession( bool isInSession )				{ m_IsInSession = isInSession; }
	bool 						getIsInSession()								{ return m_IsInSession; }
	bool 						getIsMyself( void )								{ return m_IsMyself; }

	void 						setOfferText( std::string strOfferText )		{ m_strOfferText = strOfferText; }
	std::string 				getOfferText( void )							{ return m_strOfferText; }
	void 						setOfferFileName( std::string strOfferFileName ){ m_strOfferFileName = strOfferFileName; }
	std::string 				getOfferFileName()								{ return m_strOfferFileName; }
	void 						setUserData( int userData )						{ m_UserData = userData; }
	int 						getUserData( void )								{ return m_UserData; }
    VxGUID& 					getLclSessionId( void )							{ return m_LclSessionId;    }
	VxGUID& 					getOfferSessionId( void )						{ return m_OfferSessionId;    }

    void                        setPluginType( EPluginType pluginType )			{ m_ePluginType = pluginType;    }
	EPluginType 				getPluginType( void )							{ return m_ePluginType;    }
    void                        setGuiOfferSession( GuiOfferSession* offerSession ) { m_GuiOfferSession = offerSession; };
    GuiOfferSession*			getGuiOfferSession( void )                      { return m_GuiOfferSession; };
    void                        setHisIdent( GuiUser* guiUser )					{ m_HisIdent = guiUser;    }
	GuiUser* 				    getHisIdent( void )								{ return m_HisIdent;    }

	bool						isOurSessionType( GuiOfferSession* offerSession ); 
	bool						isOurSessionInstance( GuiOfferSession* offerSession ); 

	void						onInSession( bool isInSession ); 

	void						callbackToGuiRxedPluginOffer( GuiOfferSession* offerSession );
	void						callbackToGuiRxedOfferReply( GuiOfferSession* offerSession );

	void						toGuiPluginSessionEnded( GuiOfferSession* offerSession ); 
	void						toGuiContactOffline( GuiUser* friendIdent ); 

	bool						sendOfferOrResponse();
	bool						sendOfferReply( EOfferResponse offerResponse );

	void						onStop();
 
	bool						startPluginSessionIfIsSessionOffer();
 
	void						setCallState( int eCallState );

	std::string					getHisOnlineName( void );
	std::string					describePlugin( void );
	std::string					describeResponse( EOfferResponse offerResponse );
	void						postStatusMsg( const char* statusMsg, ... );
	void						postUserMsg( const char* userMsg, ... );

	bool						handleOfferResponse( GuiOfferSession offerSession );
	bool						handleOfferResponseCode( EOfferResponse responseCode ) ;
	void						handleSessionEnded( GuiOfferSession* offerSession );
	void						handleSessionEnded( EOfferResponse responseCode );
	void						handleUserWentOffline( void );
	void						startPhoneRinging( void );
	void						stopPhoneRinging( void );
	void						showOfflineMsg( bool bExitWhenClicked = true );

	OfferBaseInfo&				getOfferInfo( void ) { return m_OfferInfo; }
    bool						isOfferInfoValid( void ) {return false;}

private:
	AppCommon& 				    m_MyApp;
    GuiOfferMgr& 				m_OfferMgr;
    AppletBase*				    m_AppletBase{ nullptr };
	GuiOfferCallback*			m_OfferCallback;
    EPluginType					m_ePluginType{ ePluginTypeInvalid };
    GuiUser* 				    m_HisIdent{ nullptr };
    VxNetIdent*				    m_MyIdent{ nullptr };
	VxGUID						m_LclSessionId;
	VxGUID						m_RmtSessionId;
	VxGUID						m_OfferSessionId;
	VxSha1Hash					m_FileHashId;
	GuiOfferSession*			m_GuiOfferSession{ nullptr };
    bool 						m_IsOffer{ false };
	bool 						m_IsMyself{ false };
	bool 						m_IsServerSession{ false };
    std::string 				m_strOfferText{ "" };
	std::string 				m_strOfferFileName{ "" };
	int							m_UserData{ 0 };
	bool 						m_SessionEndIsHandled{ false };
	bool 						m_IsInSession{ false };
	bool 						m_IsOnStopCalled{ false };
	OfferBaseInfo				m_OfferInfo;
};





