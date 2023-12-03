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

#include "ui_GuiHostedListItem.h"
#include "IdentLogicInterface.h"

#include <QWidget>

class GuiHostSession;

class HostedPluginWidget : public IdentLogicInterface
{
	Q_OBJECT;
public:
	HostedPluginWidget( QWidget* parent=nullptr );
	virtual ~HostedPluginWidget();

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setHostSession( GuiHostSession* hostSession );
    GuiHostSession*             getHostSession( void );

    VxPushButton*               getIdentAvatarButton( void ) override         { return ui.m_IconButton; }
    VxPushButton*               getIdentFriendshipButton( void ) override { return ui.m_FriendshipButton; }
    VxPushButton*               getIdentMenuButton( void ) override { return ui.m_MenuButton; }

    virtual QLabel*             getIdentLine1( void ) override { return ui.m_TitlePart1; }
    virtual QLabel*             getIdentLine2( void ) override { return ui.m_DescPart1; }

    void						onIdentAvatarButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    void                        setJoinedState( EJoinState joinState );

    void						updateWidgetFromInfo( void );

public slots:
    void						slotJoinButtonPressed( void );
    void						slotConnectButtonPressed( void );
 
protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;
    virtual void                showConnectButton( bool isAccepted );

	//=== vars ===//
    Ui::GuiHostedListItemUi	    ui;
    AppCommon&					m_MyApp;
    GuiHostSession*             m_HostSession{ nullptr };
};




