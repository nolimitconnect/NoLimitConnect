#pragma once
//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IdentWidget.h"

#include <QListWidgetItem>
#include <QObject>

class FriendListEntryWidget : public IdentWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	FriendListEntryWidget( QListWidget * parent = 0, int type = Type );
	virtual ~FriendListEntryWidget() = default;

	void						setUser( GuiUser* guiUser );
	GuiUser*					getUser( void );

signals:
	void						listButtonClicked( FriendListEntryWidget* listEntryWidget );
	void						signalMenuButtonClicked( FriendListEntryWidget* listEntryWidget );

public slots:
	virtual void				onIdentAvatarButtonClicked( void ) override;
	virtual void				onIdentOfferViewButtonClicked( void ) override;
	virtual void				onIdentOfferAcceptButtonClicked( void ) override;
	virtual void				onIdentOfferRejectButtonClicked( void ) override;
	virtual void				onIdentMenuButtonClicked( void ) override;

public:
	//=== vars ===//
};




