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

#include "IdentLogicInterface.h"

#include <QListWidgetItem>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class HostJoinRequestListItemUi;
}
QT_END_NAMESPACE

class GuiHostJoinSession;
class MyIcons;
class VxPushButton;

class HostJoinRequestListItem : public IdentLogicInterface, public QListWidgetItem
{
	Q_OBJECT;
public:
	HostJoinRequestListItem( QWidget* parent=nullptr );
	virtual ~HostJoinRequestListItem();

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setHostSession( GuiHostJoinSession* hostSession );
    GuiHostJoinSession*         getHostSession( void );

    VxPushButton*               getIdentAvatarButton( void ) override;
    VxPushButton*               getIdentFriendshipButton( void ) override;
    VxPushButton*               getIdentMenuButton( void )  override;
    VxPushButton*               getAcceptButton( void );
    VxPushButton*               getRejectButton( void );

    virtual QLabel*             getIdentLine1( void ) override;
    virtual QLabel*             getIdentLine2( void ) override;

    void						onIdentAvatarButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    void                        setJoinedState( EJoinState joinState );
    EJoinState                  getJoinedState( void ) const         { return m_JoinedState; }

    void						updateWidgetFromInfo( void );

signals:
    void						signalHostJoinRequestListItemClicked( QListWidgetItem* poItemWidget );
	void						signalAvatarButtonClicked( HostJoinRequestListItem* listEntryWidget );
    void						signalFriendshipButtonClicked( HostJoinRequestListItem* listEntryWidget );
	void						signalMenuButtonClicked( HostJoinRequestListItem* listEntryWidget );
    void						signalAcceptButtonClicked( HostJoinRequestListItem* listEntryWidget );
    void						signalRejectButtonClicked( HostJoinRequestListItem* listEntryWidget );

public slots:
    void						slotAcceptButtonPressed( void );
    void						slotRejectButtonPressed( void );
 
protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;
    virtual void                showAcceptButton( bool makeVisible );
    virtual void                showRejectButton( bool makeVisible );

	//=== vars ===//
    Ui::HostJoinRequestListItemUi&	        ui;
    AppCommon&					m_MyApp;
    EJoinState                  m_JoinedState{ eJoinStateNone };
};




