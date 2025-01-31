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

#include "IdentWidget.h"

#include <QListWidgetItem>

class GuiOfferSession;
class GuiThumb;

class GuiOfferListItem : public IdentWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	GuiOfferListItem( QWidget* parent=nullptr );
    virtual ~GuiOfferListItem() override;

    AppCommon&					getMyApp( void ) { return m_MyApp; }

    void                        setOfferSession( GuiOfferSession* offerSession );
    GuiOfferSession*            getOfferSession( void );

    VxPushButton*               getAvatarButton( void )                 { return getIdentAvatarButton(); }
    VxPushButton*               getFriendshipButton( void )             { return getIdentFriendshipButton(); }
    VxPushButton*               getPushToTalkButton( void )             { return getIdentPushToTalkButton(); }
    VxPushButton*               getOfferViewButton( void )              { return getIdentOfferViewButton(); }
    VxPushButton*               getOfferAcceptButton( void )            { return getIdentOfferAcceptButton(); }
    VxPushButton*               getOfferRejectButton( void )            { return getIdentOfferRejectButton(); }
    VxPushButton*               getMenuButton( void )                   { return getIdentMenuButton(); }

    void						onIdentAvatarButtonClicked( void ) override;
    //void                        onIdentFriendshipButtonClicked( void ) override; // commented out to allow default behavior
    void						onIdentOfferViewButtonClicked( void ) override;
    void						onIdentOfferAcceptButtonClicked( void ) override;
    void						onIdentOfferRejectButtonClicked( void ) override;
    void						onIdentPushToTalkButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    void                        updateThumb( GuiThumb* guiThumb );

    QSize                       calculateSizeHint( void );

    void                        setIsThumbUpdated( bool updated )       { m_IsThumbUpdated = updated; }
    bool                        getIsThumbUpdated( void )               { return m_IsThumbUpdated; }

signals:
    void						signalGuiOfferListItemClicked( GuiOfferListItem * poItemWidget );
	void						signalAvatarButtonClicked( GuiOfferListItem* listEntryWidget );
    void						signalFriendshipButtonClicked( GuiOfferListItem* listEntryWidget );
    void						signalOfferViewButtonClicked( GuiOfferListItem* listEntryWidget );
    void						signalOfferAcceptButtonClicked( GuiOfferListItem* listEntryWidget );
    void						signalOfferRejectButtonClicked( GuiOfferListItem* listEntryWidget );
    void						signalPushToTalkButtonClicked( GuiOfferListItem* listEntryWidget );
	void						signalMenuButtonClicked( GuiOfferListItem* listEntryWidget );

public slots:
    void						updateWidgetFromInfo( void );

protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;

	//=== vars ===//
    AppCommon&					m_MyApp;
    bool                        m_IsThumbUpdated{ false };
    bool                        m_UserUpdateConnected{ false };
};




