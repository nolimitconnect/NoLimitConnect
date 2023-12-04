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

#include "GuiHostJoinCallback.h"
#include "GuiUserJoinCallback.h"

#include "ui_BottomBarWidget.h"

class GuiUser;
class GuiOfferSession;
class QLabel;
class AppCommon;
class MyIcons;

class BottomBarWidget : public QFrame, public GuiHostJoinCallback, public GuiUserJoinCallback
{
	Q_OBJECT

public:
	BottomBarWidget( QWidget* parent = nullptr );
	virtual ~BottomBarWidget();

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );
    VxMenuButton *              getMenuButton( void );

	//=== host server state callbacks GuiHostJoinCallback ===// 
	void						callbackJoinRequestCount( int requestCnt ) override;
	void						callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
	void						callbackGuiHostJoinLeaveHost( GroupieId& groupieId ) override;

	//=== user member joint to host server state callbacks GuiUserJoinCallback ===// 
	void						callbackGuiUserJoinToHostState( EHostType hostType, bool isJoined ) override; ///< just my join to host state for me and not other members

	//=== bottom bar button visibility ===// 
	void						setArrowLeftVisibility( bool visible );
	void						set30SecBackwardVisibility( bool visible );
	void						setMediaPlayVisibility( bool visible );
	void						setMediaTrashVisibility( bool visible );
	void						setMediaFileShareVisibility( bool visible );
	void						setMediaLibraryVisibility( bool visible );
	void						set30SecForwardVisibility( bool visible );
	void						setArrowRightVisibility( bool visible );
	void						setMediaRepeatVisibility( bool visible );
	void						setMenuBottomVisibility( bool visible );
	void						setExpandWindowVisibility( bool visible );

	//=== bottom bar button icon ===// 
	virtual void				setArrowLeftButtonIcon( EMyIcons myIcon = eMyIconArrowLeft );
	virtual void				set30SecBackwardButtonIcon( EMyIcons myIcon = eMyIcon30SecBackward );
	virtual void				setMediaPlayButtonIcon( EMyIcons myIcon = eMyIconPlayNormal );
	virtual void				setMediaTrashButtonIcon( EMyIcons myIcon = eMyIconTrash );
	virtual void				setMediaFileShareButtonIcon( EMyIcons myIcon = eMyIconShareFilesNormal );
	virtual void				setMediaLibraryButtonIcon( EMyIcons myIcon = eMyIconLibraryNormal );
	virtual void				set30SecForwardButtonIcon( EMyIcons myIcon = eMyIcon30SecForward );
	virtual void				setArrowRightButtonIcon( EMyIcons myIcon = eMyIconArrowRight );
	virtual void				setMediaRepeatButtonIcon( EMyIcons myIcon = eMyIconPowerOff );
	virtual void				setMenuBottomButtonIcon( EMyIcons myIcon = eMyIconMenu );
	virtual void				setExpandWindowButtonIcon( EMyIcons myIcon = eMyIconWindowExpand );

	//=== bottom bar button color ===// 
	virtual void				setPlayProgressBarColor( QColor iconColor );
	virtual void				setArrowLeftButtonColor( QColor iconColor );
	virtual void				set30SecBackwardButtonColor( QColor iconColor );
	virtual void				setMediaPlayButtonColor( QColor iconColor );
	virtual void				setMediaTrashButtonColor( QColor iconColor );
	virtual void				setMediaFileShareButtonColor( QColor iconColor );
	virtual void				setMediaLibraryButtonColor( QColor iconColor );
	virtual void				set30SecForwardButtonColor( QColor iconColor );
	virtual void				setArrowRightButtonColor( QColor iconColor );
	virtual void				setMediaRepeatButtonColor( QColor iconColor );
	virtual void				setMenuBottomButtonColor( QColor iconColor );
	virtual void				setExpandWindowButtonColor( QColor iconColor );

signals:
	//=== bottom bar signals ===// 
	void						signalArrowLeftButtonClicked( void );
	void						signal30SecBackwardButtonClicked( void );
	void						signalMediaPlayButtonClicked( void );
	void						signalMediaTrashButtonClicked( void );
	void						signalMediaFileShareClicked( void );
	void						signalMediaLibraryButtonClicked( void );
	void						signal30SecForwardButtonClicked( void );
	void						signalArrowRightButtonClicked( void );
	void						signalMediaRepeatButtonClicked( void );
	void						signalMenuBottomButtonClicked( void );
	void						signalExpandWindowButtonClicked( void );

protected slots:
	//=== bottom bar slots ===// 
	virtual void				slotArrowLeftButtonClicked( void );
	virtual void				slot30SecBackwardButtonClicked( void );
	virtual void				slotMediaPlayButtonClicked( void );
	virtual void				slotMediaTrashButtonClicked( void );
	virtual void				slot30SecForwardButtonClicked( void );
	virtual void				slotMediaFileShareClicked( void );
	virtual void				slotMediaLibraryButtonClicked( void );
	virtual void				slotArrowRightButtonClicked( void );
	virtual void				slotMediaRepeatButtonClicked( void );
	virtual void				slotMenuBottomButtonClicked( void );
	virtual void				slotExpandWindowButtonClicked( void );

	virtual void				slotMessengerButtonClicked( void );
	virtual void				slotGroupHostButtonClicked( void );
	virtual void				slotChatRoomHostButtonClicked( void );
	virtual void				slotRandomConnectHostButtonClicked( void );

	virtual void				slotSettingsButtonClicked( void );

protected:
	QWidget*				    getParentPageFrame( void ); // get home page frame ( Launch or Messenger Page )
	void						refreshUserJoinedToHostStates( void );

	Ui::BottomBarWidgetClass	ui;
	AppCommon&					m_MyApp;
};
