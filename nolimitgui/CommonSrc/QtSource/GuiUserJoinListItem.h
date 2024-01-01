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

#include "IdentWidget.h"

#include <QListWidgetItem>
#include <QWidget>

class GuiConnectIdListMgr;
class GuiOfferMgr;
class GuiUserJoinSession;
class GuiUser;
class GuiUserJoin;
class GuiUserJoinMgr;
class GuiUserMgr;
class GuiThumb;

class GuiUserJoinListItem : public IdentWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	GuiUserJoinListItem( QWidget* parent=nullptr );
	virtual ~GuiUserJoinListItem();

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setUserJoinSession( GuiUserJoinSession* hostSession );
    GuiUserJoinSession*         getUserJoinSession( void );

    void						updateWidgetFromInfo( void );
    void                        updateThumb( GuiThumb* guiThumb );

    QSize                       calculateSizeHint( void );

    void                        updateUser( GuiUser* guiUser );

    void                        setIsThumbUpdated( bool updated )   { m_IsThumbUpdated = updated; }
    bool                        getIsThumbUpdated( void )           { return m_IsThumbUpdated; }

signals:
	void						signalAvatarButtonClicked( GuiUserJoinListItem* listEntryWidget );
	void						signalMenuButtonClicked( GuiUserJoinListItem* listEntryWidget );

protected:
    void						onIdentAvatarButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;

	//=== vars ===//
    AppCommon&					m_MyApp;
    GuiConnectIdListMgr&		m_ConnectIdListMgr;
    GuiOfferMgr&			    m_OfferMgr;
    GuiUserMgr&					m_UserMgr;
    GuiUserJoinMgr&				m_UserJoinMgr;
    bool                        m_IsThumbUpdated{ false };
};




