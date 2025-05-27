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

#include "AppletClientBase.h"
#include "GuiHostedListCallback.h"

#include <GuiInterface/IDefs.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletGroupListLocalViewUi;
}
QT_END_NAMESPACE

class VxNetIdent;
class GuiHostSession;
class GuiHostedListItem;

class AppletGroupListLocalView : public AppletClientBase, public GuiHostedListCallback
{
	Q_OBJECT
public:
	AppletGroupListLocalView( AppCommon& app, QWidget*	parent = nullptr );
	virtual ~AppletGroupListLocalView();

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

    void						setStatusLabel( QString strMsg );
    void                        setInfoLabel( QString strMsg );

    void						updateHostedList( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId );
    void                        clearPluginSettingToList( void );
    void                        clearStatus( void );

signals:
    void						signalSearchResult( VxNetIdent* netIdent );
    void						signalSearchComplete( void );
    void                        signalLogMsg( const QString& logMsg );
    void                        signalInfoMsg( const QString& logMsg );

private slots:
    void                        slotInfoMsg( const QString& text );
    void						slotSearchComplete( void );
    void						slotRefreshGroupList( void );
    void						slotHostAnnounceStatus( EHostType hostType, VxGUID sessionId, EHostAnnounceStatus hostStatus, QString strMsg );
    void						slotHostJoinStatus( EHostType hostType, VxGUID sessionId, EHostJoinStatus hostStatus, QString strMsg );
    void						slotHostSearchStatus( EHostType hostType, VxGUID sessionId, EHostSearchStatus hostStatus, QString strMsg );

    virtual void                slotIconButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );
    virtual void                slotMenuButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );
    virtual void                slotJoinButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );

    void                        onCancelButClick( void );

protected:
    virtual void				callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId ) override;

    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

    //=== vars ===//
    Ui::AppletGroupListLocalViewUi& ui;
    bool                        m_SearchStarted{ false };
    VxGUID                      m_SearchSessionId;
    QTimer*                     m_CloseAppletTimer{ nullptr };
};
