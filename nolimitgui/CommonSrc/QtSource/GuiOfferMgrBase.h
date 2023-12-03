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

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiUserUpdateCallback.h"
#include "GuiOfferSession.h"

#include <CoreLib/VxMutex.h>
#include <CoreLib/VxTimer.h>

class GuiOfferCallback;
class GuiOfferSession;
class GuiOfferSession;
class GuiUser;
class QListWidgetItem;
class QTimer;
class VxGUID;

class GuiOfferMgrBase : public QWidget, public GuiUserUpdateCallback
{
	Q_OBJECT
public:
	GuiOfferMgrBase( AppCommon& myApp );

    virtual std::vector<GuiOfferSession*>& getCallList( void )			{ return m_OfferList; }
    virtual std::vector<GuiOfferSession*>& getOfferList( void )			{ return m_OfferList; }

	virtual void                onAppCommonCreated( void ) = 0;
	virtual void                onMessengerReady( bool ready )				{ }
	virtual bool                isMessengerReady( void );
	virtual void                onSystemReady( bool ready ) { }

	virtual int					getActiveOfferCount( void )					{ return m_ActiveOfferCount; }

	virtual GuiOfferSession*	getTopGuiOfferSession( void ); // returns null if no session offers

// from engine
	virtual void                toGuiRxedPluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo );
	virtual void                toGuiRxedOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo );

	virtual void				toGuiPluginSessionEnded( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId );

	virtual bool				fromGuiMakePluginOffer( QWidget* parent, EPluginType pluginType, GuiUser* guiUser, FileInfo& fileInfo );
	virtual bool				fromGuiMakePluginOffer( QWidget* parent, EPluginType pluginType, GuiUser* guiUser, OfferBaseInfo& offerInfo );
	virtual bool				fromGuiToPluginOfferReply( EPluginType pluginType, GuiUser* guiUser, OfferBaseInfo& offerInfo, VxGUID& offerSessionId, EOfferResponse offerResponse );

	void						onIsInSession( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, bool isInSession );
	void						onSessionExit( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser );
	void						startedSessionInReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser );

	void						acceptOfferButtonClicked( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser );
	void						rejectOfferButtonClicked( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser );

	// called if starting new session to know if responding to existing offer
	GuiOfferSession*			findActiveAndAvailableOffer( GuiUser* guiUser, EPluginType pluginType );
	void						sentOffer( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser );
	void						sentOfferReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse );
	void						removePluginSessionOffer( EPluginType pluginType, GuiUser* guiUser );
	void						removePluginSessionOffer( VxGUID& offerSessionId );

	virtual void				viewOffer( GuiOfferSession* offerSession, QWidget* contentFrame );
	virtual bool				acceptOffer( GuiOfferSession* offerSession, QWidget* contentFrame );
	virtual bool				rejectOffer( GuiOfferSession* offerSession, QWidget* contentFrame );

	virtual bool				validateOffer( GuiOfferSession* offerSession, QWidget* contentFrame, bool showErrorMsg = true );

protected slots:
	void						slotUpdateOffersTimer( void );
	void						slotOncePerSecondRingTimer( void );

protected:
	virtual void				callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

	void						recievedOffer( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser );
	void						recievedOfferReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse );
	void						recievedSessionEnd( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse );

	void						changeOfferState( GuiOfferSession* sessionState, EOfferState newOfferState );
	void						forceToNotInSession( GuiOfferSession* sessionState );
	void						startRingTimerIfNotInSession( void );
	void						stopRingTimer( void );
	void						updateSndsAndMessages( GuiOfferSession* offerSession );

	GuiOfferSession*			createOfferSession( GuiUser* guiUser, OfferBaseInfo& offerInfo );
    GuiOfferSession*			findOfferSession( EPluginType pluginType, VxGUID sessionId, GuiUser* guiUser );

	void						checkAndUpdateIfEmptyOfferList( void );
	void						updateActiveOfferCount( void );

	//=== vars ===//
	static const int 			RING_COUNT 				= 4; 
	static const int 			RING_ELAPSE_SEC 		= 4; 

	AppCommon& 					m_MyApp;
	QTimer*						m_OfferUpdateTimer{ nullptr };
	QTimer*						m_RingTimer{ nullptr };

	int							m_ActiveOfferCount{ 0 };
	bool						m_UserIsInSession{ false };
	int							m_RingTimerCycleCnt{ 0 };
	int							m_RingTimerSecondCnt{ 0 };

	std::vector<OfferBaseInfo>	m_OffersSentList;
	std::vector<OfferBaseInfo>	m_ResponseSentList;

	std::vector<GuiOfferSession*> m_OfferList;

	std::vector<GuiOfferCallback*>	m_OfferCallbackList;
};
