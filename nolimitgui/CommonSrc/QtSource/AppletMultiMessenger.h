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

#include "AppletPeerBase.h"
#include "GuiOfferCallback.h"
#include "InputClientBaseCallback.h"
#include "MultiSessionState.h"
#include "TodGameLogic.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletMultiMessengerUi;
}
QT_END_NAMESPACE

class EngineSettings;
class GuiHosted;
class GuiHostSession;
class GuiOfferSession;
class MultiSessionState;
class P2PEngine;

class AppletMultiMessenger : public AppletPeerBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
	AppletMultiMessenger( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletMultiMessenger();

	AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return AppletPeerBase::getPluginType(); }

	virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )  override;
    virtual void				toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID onlineId, int pos0to100000 ) override;
    virtual void				callbackToGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offerSession ) override;

    virtual void 				onSessionActivityShouldExit( QString shouldExitReason ) override {};

    virtual void				onActivityFinish( void ) override;

	void						setSelectedUser( GuiUser* guiUser );

    void						userJoinedHost( GuiHosted* guiHosted ) override;

protected slots:
    void						slotSetSessionVisible( bool visible );

	void						slotUserInputButtonClicked( void );

	void						slotUserSelected( GuiUser* guiUser );

	void						slotViewChanged( EUserViewType viewType );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

	void                        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

	virtual bool 				checkForSendAccess( bool sendOfferIfPossible );
	virtual void				showReasonAccessNotAllowed( void );

    virtual void 				onSessionStateChange( ESessionState eSessionState ) override;
	// called from session logic
    virtual void				onInSession( bool isInSession ) override;

	void						setupMultiSessionActivity( GuiUser* hisIdent );
	void						setStatusMsg( QString strStatus );

	MultiSessionState *			getMSessionState( EMSessionType sessionType );
	void						onSelectedUserChanged( GuiUser* guiUser );

	bool						checkIfCanSend( void ) override;
	bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

	bool						sendRandConnectSelected( VxGUID& onlineId, bool isSelected );

	//=== vars ===//
	Ui::AppletMultiMessengerUi&	ui;
    TodGameLogic				m_TodGameLogic;
    QVector<MultiSessionState*> m_MSessionsList;
    QFrame*						m_OffersFrame{ nullptr };
	QFrame*						m_ResponseFrame{ nullptr };
	QFrame*						m_HangupSessionFrame{ nullptr };
	TodGameWidget *				m_TodGameWidget{ nullptr };
	VidWidget *					m_VidChatWidget{ nullptr };
	bool						m_IsInitialized{ false };
	bool						m_OfferOrResponseIsSent{ false };
	bool						m_CanSend{ false };    
	GuiUser*					m_SelectedUser{ nullptr };
};

