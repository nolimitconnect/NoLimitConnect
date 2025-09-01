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

#include <QFrame>

#include "AppletMgrCallback.h"
#include "GuiHostJoinCallback.h"
#include "GuiMemberActiveCallback.h"

#include "MyIconsDefs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class BottomBarWidgetClass;
}
QT_END_NAMESPACE

class GuiUser;
class GuiOfferSession;
class QLabel;
class AppCommon;
class MyIcons;
class VxMenuButton;
class VxPushButton;

class BottomBarWidget : public QFrame, public GuiHostJoinCallback, public GuiMemberActiveCallback, public AppletMgrCallback
{
	Q_OBJECT

public:
	BottomBarWidget( QWidget* parent = nullptr );
	virtual ~BottomBarWidget();

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );
    VxMenuButton*               getMenuButton( void );

	//=== host server state callbacks GuiHostJoinCallback ===// 
	void						callbackJoinRequestCount( int requestCnt ) override;
	void						callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
	void						callbackGuiHostJoinLeaveHost( GroupieId& groupieId ) override;

	//=== user member joint to host server state callbacks GuiMemberActiveCallback ===// 
	void						callbackGuiMemberIsJoinedToHost( VxGUID& onlineId, EHostType host, bool isJoined ) override; 

	//=== applet opened or closed ===// 
	void						callbackAppletIsOpen( EApplet applet, bool isOpen ) override;

	//=== bottom bar button visibility ===// 
	void						setMenuBottomVisibility( bool visible );
	void						setExpandWindowVisibility( bool visible );

	//=== bottom bar button icon ===// 
	void          				setMenuBottomButtonIcon( EMyIcons myIcon = eMyIconMenu );
	void          				setExpandWindowButtonIcon( EMyIcons myIcon = eMyIconWindowExpand );

	//=== bottom bar button color ===// 
	void          				setMenuBottomButtonColor( QColor iconColor );
	void          				setExpandWindowButtonColor( QColor iconColor );

signals:
	//=== bottom bar signals ===// 
	void						signalMenuBottomButtonClicked( void );
	void						signalExpandWindowButtonClicked( void );

public slots:
	//=== bottom bar slots ===// 
	void						slotMenuBottomButtonClicked( void );
	void						slotExpandWindowButtonClicked( void );

	void						slotMessengerButtonClicked( void );
	void						slotGroupHostButtonClicked( void );
	void						slotChatRoomHostButtonClicked( void );
	void						slotRandomConnectHostButtonClicked( void );

	void          				slotGroupJoinButtonClicked( void );
	void          				slotChatRoomJoinButtonClicked( void );
	void          				slotRandomConnectJoinButtonClicked( void );

	void          				slotSettingsButtonClicked( void );
	void						slotSystemReady( bool isReady );

protected:
	void						showEvent( QShowEvent* ev ) override;

	QWidget*				    getParentPageFrame( void ); // get home page frame ( Launch or Messenger Page )
	void						refreshUserJoinedToHostStates( void );
	void						launchJoinHostView( EHostType hostType );
	void						refreshAdminHostStates( void );
	void						updateAdminHostState( EHostType hostType, VxPushButton* adminButton );
	void						launchAdminHostView( EHostType hostType );

	Ui::BottomBarWidgetClass&	ui;
	AppCommon&					m_MyApp;
};
