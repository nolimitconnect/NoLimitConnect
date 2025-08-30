#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletClientBase.h"

#include <GuiInterface/IDefs.h>
#include "GuiHostedListCallback.h"

#include <PktLib/SearchParams.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletGroupListClientUi;
}
QT_END_NAMESPACE

class VxNetIdent;
class GuiHostSession;
class GuiHostedListItem;

class AppletGroupListClient : public AppletClientBase, public GuiHostedListCallback
{
	Q_OBJECT
public:
	AppletGroupListClient(	AppCommon&		    app, 
							QWidget*			parent = nullptr );
	virtual ~AppletGroupListClient();

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

    void						setStatusLabel( QString strMsg );
    void                        setInfoLabel( QString strMsg );

    QString                     getSearchText( void );

    virtual void				callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId ) override;

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
    void						slotStartSearchState( bool startSearch );
    void						slotHostAnnounceStatus( EHostType hostType, VxGUID sessionId, EHostAnnounceStatus hostStatus, QString strMsg );
    void						slotHostJoinStatus( EHostType hostType, VxGUID sessionId, EHostJoinStatus hostStatus, QString strMsg );
    void						slotHostSearchStatus( EHostType hostType, VxGUID sessionId, EHostSearchStatus hostStatus, QString strMsg );

    virtual void                slotIconButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );
    virtual void                slotMenuButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );
    virtual void                slotJoinButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );
    virtual void                slotConnectButtonClicked( GuiHostSession* hostSession, GuiHostedListItem* hostItem );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

    //=== vars ===//
    Ui::AppletGroupListClientUi& ui;
    SearchParams                m_SearchParams;
    bool                        m_SearchStarted{ false };
};
