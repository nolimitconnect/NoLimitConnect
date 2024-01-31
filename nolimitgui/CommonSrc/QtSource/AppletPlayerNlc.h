#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerNlcBase.h"

#include "GuiPlayerCallback.h"
#include "ui_AppletPlayerNlc.h"

#include <QElapsedTimer>

class AppletPlayerNlc : public AppletPlayerNlcBase
{
	Q_OBJECT
public:
	AppletPlayerNlc( AppCommon& app, QWidget* parent );
    virtual ~AppletPlayerNlc();

	RenderGlWidget*				getRenderConsumer( void ) override		{ return ui.m_RenderWidget; }
	QSlider*					getPlayPosSlider( void ) override		{ return ui.m_PlayPosSlider; }
	QPushButton*				getReplayButton( void ) override		{ return ui.m_ReplayButton; }

	void						onMediaPlayerNlcReady( bool isReady ) override;

protected slots:
    void                        slotAppletClosing( void );

	void                        slotMenuItemSelected( int menuId, EMenuItemType menuItemType );

	void                        slotMediaFileComboBoxSelectionChange( int cbIdx );

	void						slotBrowseButtonClick( void );
	void						slotReplayButtonClick( void );

protected:
	void						initAppletPlayerNlc( void );

	void                        setupBottomMenu( VxMenuButton* menuButton );

	void						browseForMovie( void );
	void						onFileSelected( FileInfo& fileInfo );
	void						playSelectedMovie( std::string movieFile );

	//=== vars ===//
	Ui::AppletPlayerNlcUi		ui;
};


