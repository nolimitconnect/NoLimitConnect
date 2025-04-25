#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class GuiFriendRequestWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class GuiFriendRequest;
class GuiUser;

class GuiFriendRequestListItem : public QWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	GuiFriendRequestListItem( QWidget* parent=nullptr );
	virtual ~GuiFriendRequestListItem();

    void                        setFriendRequest( GuiFriendRequest* friendRequest );
    GuiFriendRequest*           getFriendRequest( void );

    void						updateWidgetFromInfo( void );
    void						updateUser( GuiUser* guiUser );
    void                        updateFriendRequest( GuiFriendRequest* friendRequest );

    virtual QSize				sizeHint() const override;

signals:
    void						signalAcceptButtonClicked( GuiFriendRequestListItem* listEntryWidget );
	void						signalDetailsButtonClicked( GuiFriendRequestListItem* listEntryWidget );
    void						signalFriendshipButtonClicked( GuiFriendRequestListItem* listEntryWidget );
	void						signalRejectButtonClicked( GuiFriendRequestListItem* listEntryWidget );

	void						signalFriendRequestListItemClicked( GuiFriendRequestListItem* listEntryWidget );

protected slots:
    void						slotAcceptButtonClicked( void );
	void						slotDetailsButtonClicked( void );
    void						slotFriendshipButtonClicked( void );
	void						slotRejectButtonClicked( void );

protected:
    virtual void				mousePressEvent( QMouseEvent * event ) override;
    virtual void				resizeEvent( QResizeEvent* resizeEvent ) override;

	//=== vars ===//
    Ui::GuiFriendRequestWidgetUi&	ui;
	AppCommon&					m_MyApp;
};




