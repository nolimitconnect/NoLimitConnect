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

class GuiGroupieListSession;

class GuiGroupieListItem : public IdentWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	GuiGroupieListItem( QWidget* parent=nullptr );
	virtual ~GuiGroupieListItem();

    MyIcons&					getMyIcons( void );

    void                        setHostSession( GuiGroupieListSession* hostSession );
    GuiGroupieListSession*      getHostSession( void );

    void						onIdentAvatarButtonClicked( void ) override;
    void						onIdentMenuButtonClicked( void ) override;

    void						updateWidgetFromInfo( void );

    QSize                       calculateSizeHint( void );

    void                        updateUser( GuiUser* guiUser );

    void                        setIsThumbUpdated( bool updated )   { m_IsThumbUpdated = updated; }
    bool                        getIsThumbUpdated( void )           { return m_IsThumbUpdated; }

signals:
    void						signalGuiGroupieListItemClicked( QListWidgetItem* poItemWidget );
	void						signalIconButtonClicked( GuiGroupieListItem* listEntryWidget );
    void						signalFriendshipButtonClicked( GuiGroupieListItem* listEntryWidget );
	void						signalMenuButtonClicked( GuiGroupieListItem* listEntryWidget );

 
protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;

	//=== vars ===//
    bool                        m_IsThumbUpdated{ false };
};




