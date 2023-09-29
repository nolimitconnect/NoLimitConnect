#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IdentWidget.h"

#include <QListWidgetItem>


class GuiUserSessionBase;
class GuiThumb;

class GuiUserListItem : public IdentWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	GuiUserListItem( QWidget* parent=nullptr );
	virtual ~GuiUserListItem();

    virtual bool                operator<( const QListWidgetItem& other ) const override; // compare operator for sorting
    virtual bool                compareLessThanItem( GuiUserListItem* rhs );

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setUserSession( GuiUserSessionBase* hostSession );
    GuiUserSessionBase*         getUserSession( void );

    VxPushButton*               getAvatarButton( void )                 { return getIdentAvatarButton(); }
    VxPushButton*               getFriendshipButton( void )             { return getIdentFriendshipButton(); }
    VxPushButton*               getPushToTalkButton( void )             { return getIdentPushToTalkButton(); }
    VxPushButton*               getOfferViewButton( void )              { return getIdentOfferViewButton(); }
    VxPushButton*               getOfferAcceptButton( void )            { return getIdentOfferAcceptButton(); }
    VxPushButton*               getOfferRejectButton( void )            { return getIdentOfferRejectButton(); }
    VxPushButton*               getMenuButton( void )                   { return getIdentMenuButton(); }

    void						onIdentAvatarButtonClicked( void ) override;
    //void                        onIdentFriendshipButtonClicked( void ) override; // commented out to allow default behavior
    void						onIdentPushToTalkButtonClicked( void ) override;
    void						onIdentOfferViewButtonClicked( void ) override;
    void						onIdentOfferAcceptButtonClicked( void ) override;
    void						onIdentOfferRejectButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    void                        updateThumb( GuiThumb* guiThumb );

    QSize                       calculateSizeHint( void );

    void                        setIsThumbUpdated( bool updated )       { m_IsThumbUpdated = updated; }
    bool                        getIsThumbUpdated( void )               { return m_IsThumbUpdated; }

    void                        callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus ) override;

signals:
    void						signalGuiUserListItemClicked( GuiUserListItem * poItemWidget );
	void						signalAvatarButtonClicked( GuiUserListItem* listEntryWidget );
    void						signalFriendshipButtonClicked( GuiUserListItem* listEntryWidget );
    void						signalOfferViewButtonClicked( GuiUserListItem* listEntryWidget );
    void						signalOfferAcceptButtonClicked( GuiUserListItem* listEntryWidget );
    void						signalOfferRejectButtonClicked( GuiUserListItem* listEntryWidget );
    void						signalPushToTalkButtonClicked( GuiUserListItem* listEntryWidget );
	void						signalMenuButtonClicked( GuiUserListItem* listEntryWidget );

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




