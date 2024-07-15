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

QT_BEGIN_NAMESPACE
namespace Ui {
    class GuiHostedListItemUi;
}
QT_END_NAMESPACE

class GuiHostedListSession;
class MyIcons;

class GuiHostedListItem : public IdentLogicInterface, public QListWidgetItem
{
	Q_OBJECT;
public:
	GuiHostedListItem( EHostType hostType, QWidget* parent=nullptr );
	virtual ~GuiHostedListItem();

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setHostSession( GuiHostedListSession* hostSession );
    GuiHostedListSession*       getHostSession( void );

    VxPushButton*               getIdentAvatarButton( void ) override;
    VxPushButton*               getIdentFriendshipButton( void ) override;
    VxPushButton*               getIdentMenuButton( void ) override;

    virtual QLabel*             getIdentLine1( void ) override;
    virtual QLabel*             getIdentLine2( void ) override;

    void						onIdentAvatarButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    void                        setJoinedState( EJoinState joinState );

    void						updateWidgetFromInfo( void );

    QSize                       calculateSizeHint( void );

    void                        updateUser( GuiUser* guiUser );

    void                        setIsThumbUpdated( bool updated )   { m_IsThumbUpdated = updated; }
    bool                        getIsThumbUpdated( void )           { return m_IsThumbUpdated; }

    void                        setIsHostView( bool isHost )        { m_IsHostView = isHost; }
    bool                        getIsHostView( void )               { return m_IsHostView; }

signals:
    void						signalGuiHostedListItemClicked( QListWidgetItem* poItemWidget );
	void						signalIconButtonClicked( GuiHostedListItem* listEntryWidget );
    void						signalFriendshipButtonClicked( GuiHostedListItem* listEntryWidget );
	void						signalMenuButtonClicked( GuiHostedListItem* listEntryWidget );
    void						signalJoinButtonClicked( GuiHostedListItem* listEntryWidget );
    void						signalConnectButtonClicked( GuiHostedListItem* listEntryWidget );
    void						signalKickButtonClicked( GuiHostedListItem* listEntryWidget );
    void						signalIgnoreButtonClicked( GuiHostedListItem* listEntryWidget );

public slots:
    void						slotJoinButtonClicked( void );
    void						slotConnectButtonClicked( void );
    void						slotKickButtonClicked( void );
    void						slotIgnoreButtonClicked( void );
 
protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;

    void                        showConnectButton( bool isAccepted );
    void                        showKickButton( bool isVisible );
    void                        showIgnoreButton( bool isVisible );

	//=== vars ===//
    Ui::GuiHostedListItemUi&	ui;
    AppCommon&					m_MyApp;
    EHostType                   m_HostType{ eHostTypeUnknown };
    bool                        m_IsThumbUpdated{ false };
    bool                        m_IsHostView{ false };
};




