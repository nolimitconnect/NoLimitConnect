#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_AppletMultiMessenger.h"

#include "AppletPeerBase.h"
#include "GuiOfferCallback.h"
#include "MultiSessionState.h"
#include "TodGameLogic.h"

class EngineSettings;
class GuiHosted;
class GuiHostSession;
class GuiOfferSession;
class MultiSessionState;
class P2PEngine;

class AppletMultiMessenger : public AppletPeerBase
{
	Q_OBJECT
public:
	AppletMultiMessenger( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletMultiMessenger();

	virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )  override;
    virtual void				toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000 ) override;
	virtual void				callbackToGuiPluginSessionEnded( GuiOfferSession* offerSession ) override;

    virtual void 				onSessionActivityShouldExit( QString shouldExitReason ) override {};

    virtual void				onActivityFinish( void ) override;

	void						setSelectedUser( GuiUser* guiUser );

    void						userJoinedHost( GuiHosted* guiHosted ) override;

protected slots:
    void						slotSetSessionVisible( bool visible );

	void						slotUserInputButtonClicked( void );

	void						slotUserSelected( GuiUser* guiUser );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;

    virtual void				callbackToGuiRxedPluginOffer( GuiOfferSession* offer ) override;
    virtual void				callbackToGuiRxedOfferReply( GuiOfferSession* offer ) override;
	void                        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

	virtual bool 				checkForSendAccess( bool sendOfferIfPossible );
	virtual void				showReasonAccessNotAllowed( void );

    virtual void 				onSessionStateChange( ESessionState eSessionState ) override;
	// called from session logic
    virtual void				onInSession( bool isInSession ) override;

	void						setupMultiSessionActivity( GuiUser* hisIdent );
	void						setStatusMsg( QString strStatus );

	void						toGuiSetGameValueVar(	EPluginType     pluginType, 
														VxGUID&		    onlineId, 
														int32_t			s32VarId, 
                                                        int32_t			s32VarValue ) override;

	void						toGuiSetGameActionVar(	EPluginType     pluginType, 
														VxGUID&		    onlineId, 
														int32_t			s32VarId, 
                                                        int32_t			s32VarValue ) override;

	//virtual bool				handleOfferResponse( EOfferResponse offerResponse, QWidget* parent );
    //void						onInSessionResponse( bool bResponseOk );

	MultiSessionState *			getMSessionState( EMSessionType sessionType );
	void						onSelectedUserChanged( GuiUser* guiUser );

	//=== vars ===//
	Ui::AppletMultiMessengerUi	ui;
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

