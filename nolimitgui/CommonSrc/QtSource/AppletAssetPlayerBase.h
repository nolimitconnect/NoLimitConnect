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

#include "ToGuiActivityInterface.h"
#include "AppletBase.h"

#include <ptop_src/ptop_engine_src/AssetMgr/AssetInfo.h>

#include <QWidget>
#include <QListWidgetItem>

class P2PEngine;
class QProgressBar;
class MyIcons;

class AppletAssetPlayerBase : public AppletBase
{
	Q_OBJECT

public:
	AppletAssetPlayerBase( const char* ObjName, AppCommon& appCommon, QWidget* parent=nullptr );
	virtual ~AppletAssetPlayerBase();

	MyIcons&					getMyIcons( void );

	virtual void				setPlayerAssetId( VxGUID& feedId );

	virtual void				setXferBar( QProgressBar* xferProgressBar );

	virtual void				updateFromAssetInfo( void );

    virtual void				toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

	virtual void				showSendFail( bool show, bool permissionErr = false ) = 0;
	virtual void				showResendButton( bool show ) = 0;
	virtual void				showShredder( bool show ) = 0;
	virtual void				showXferProgress( bool show ) = 0;
	virtual void				setXferProgress( int xferProgress ) = 0;

    virtual void				onActivityStop( void ) {};

signals:
	void						signalShreddingAsset( AppletAssetPlayerBase * assetWidget );

protected slots:
	virtual void				slotShredAsset( void );
	virtual void				slotResendAsset( void );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;
	void						updateProgressBarVisibility( void );

	void						doShowProgress( bool showProgress );
	void						doSetProgress(  int xferProgress );
	void						updateWantCallbacks( bool wantCallbacks );

	P2PEngine&					m_Engine;
	int							m_AssetSendProgress{ 0 };
	bool						m_CallbacksRequested{ false };
	QProgressBar*				m_XferProgressBar{ nullptr };
	bool						m_ProgressBarShouldBeVisible{ false };
	bool						m_ProgressBarIsVisible{ false };
};
