#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include "MyIconsDefs.h"

#include <CoreLib/VxGUID.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AvatarBarWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;

class AvatarBar : public QWidget
{
	Q_OBJECT

public:
	AvatarBar( QWidget* parent = nullptr );
	virtual ~AvatarBar() = default;

	AppCommon&					getMyApp( void ) { return m_MyApp; }

	void						setOnlineId( VxGUID& onlineId );
	void						setTime( time_t creationTime, bool isQueued );
	void						setShredFile( QString fileName );
	void						setShredButtonIcon( EMyIcons iconTrash );

	virtual void				showAvatar( bool show );
	virtual void				showSendFail( bool show, bool permissionErr );
	virtual void				showShredder( bool show );
	virtual void				showResendButton( bool show );
	virtual void				showXferProgress( bool show );
	virtual void				setXferProgress( int sendProgress );

	int							getDesiredHeight( void );

signals:
	void						signalShredAsset( void );
	void						signalResendAsset( void );

protected:
	//=== vars ===//
	Ui::AvatarBarWidgetUi&		ui;
	AppCommon&					m_MyApp;
	VxGUID						m_OnlineId;
	bool						m_ThumbnailQueried{ false };
};
