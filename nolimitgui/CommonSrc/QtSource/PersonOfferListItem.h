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

#include "ui_PersonOfferListItem.h"
#include "IdentLogicInterface.h"

#include <QListWidgetItem>

class GuiOfferSession;

class PersonOfferListItem : public IdentLogicInterface, public QListWidgetItem
{
	Q_OBJECT;
public:
	PersonOfferListItem( QWidget* parent=nullptr );
	virtual ~PersonOfferListItem();

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setOfferSession( GuiOfferSession* hostSession );
    GuiOfferSession*            getOfferSession( void );

    void						updateWidgetFromInfo( void );
    QSize                       calculateSizeHint( void );

    VxPushButton*               getIdentAvatarButton( void ) override       { return ui.m_AvatarButton; }
    VxPushButton*               getIdentFriendshipButton( void ) override   { return ui.m_FriendshipButton; }
    VxPushButton*               getIdentMenuButton( void )  override        { return ui.m_MenuButton; }
    VxPushButton*               getAcceptButton( void )                     { return ui.m_AcceptButton; }
    VxPushButton*               getRejectButton( void )                     { return ui.m_RejectButton; }

    virtual QLabel*             getIdentLine1( void ) override              { return ui.m_TitlePart1; }
    virtual QLabel*             getIdentLine2( void ) override              { return ui.m_Line2Label; }

    void						onIdentAvatarButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

signals:
    void						signalPersonOfferListItemClicked( PersonOfferListItem * poItemWidget );
	void						signalAvatarButtonClicked( PersonOfferListItem* listEntryWidget );
	void						signalMenuButtonClicked( PersonOfferListItem* listEntryWidget );

protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;

	//=== vars ===//
    Ui::PersonOfferListItemUi	ui;
    AppCommon&					m_MyApp;
};




