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

#include "AppletBase.h"
#include "GuiUserUpdateCallback.h"

#include <GuiInterface/IDefs.h>

#include <QString>
#include <QDialog>

#include "ui_AppletOfferList.h"

class VxNetIdent;
class GuiHostSession;

class AppletOfferList : public AppletBase, public GuiUserUpdateCallback
{
	Q_OBJECT
public:
	AppletOfferList( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletOfferList() override;

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

    void						setStatusLabel( QString strMsg );

    EOfferViewType              getListType( void )             { return m_ListViewType; }
    void                        clearList( void );
    void                        clearStatus( void );

signals:
    void						signalSearchResult( VxNetIdent* netIdent );
    void						signalSearchComplete( void );
    void                        signalLogMsg( const QString& logMsg );
    void                        signalInfoMsg( const QString& logMsg );

private slots:
    void                        slotActiveOffersButtonClicked( void );
    void                        slotOfferHistoryButtonClicked( void );

    void                        slotOfferListItemClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        slotAvatarButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );

    void                        slotOfferViewButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        slotOfferAcceptButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        slotOfferRejectButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        slotPushToTalkButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );

protected:
    virtual void				callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    virtual void				callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override;

    virtual void				callbackUserAdded( GuiUser* guiUser ) override;
    virtual void				callbackUserUpdated( GuiUser* guiUser ) override;
    virtual void				callbackUserRemoved( VxGUID& onlineId ) override;

    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

    void                        updateUser( EUserViewType listType, VxGUID& onlineId );
    void                        updateUser( GuiUser* guiUser );
    void                        removeUser( VxGUID& onlineId );

    void                        updateOfferList( EOfferViewType offerViewType );

    //=== vars ===//
    Ui::AppletOfferListUi       ui;
    EOfferViewType              m_ListViewType{ eOfferViewTypeNone };
};
