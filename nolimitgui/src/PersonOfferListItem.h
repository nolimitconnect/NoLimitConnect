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
    class PersonOfferListItemUi;
}
QT_END_NAMESPACE

class GuiOfferSession;
class MyIcons;
class QLabel;
class VxPushButton;

class PersonOfferListItem : public IdentLogicInterface, public QListWidgetItem
{
	Q_OBJECT;
public:
	PersonOfferListItem( QWidget* parent=nullptr );
    virtual ~PersonOfferListItem() = default;

    AppCommon&					getMyApp( void ) { return m_MyApp; }
    MyIcons&					getMyIcons( void );

    void                        setOfferSession( GuiOfferSession* hostSession );
   GuiOfferSession*             getOfferSession( void );

    void						updateWidgetFromInfo( void );
    QSize                       calculateSizeHint( void );

    VxPushButton*               getIdentAvatarButton( void ) override;
    VxPushButton*               getIdentFriendshipButton( void ) override;
    VxPushButton*               getIdentMenuButton( void )  override;
    VxPushButton*               getAcceptButton( void );
    VxPushButton*               getRejectButton( void );

    QLabel*                     getIdentLine1( void ) override;
    QLabel*                     getIdentLine2( void ) override;

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
    Ui::PersonOfferListItemUi&	ui;
    AppCommon&					m_MyApp;
};




