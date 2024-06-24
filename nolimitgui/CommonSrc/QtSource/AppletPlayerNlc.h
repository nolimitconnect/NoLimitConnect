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
#include "PlayControlWidget.h"
#include "ui_AppletPlayerNlc.h"

#include <CoreLib/VxFileLists.h>

#include <QElapsedTimer>

class QDir;
class QFileInfo;

class AppletPlayerNlc : public AppletPlayerNlcBase
{
	Q_OBJECT
public:
	AppletPlayerNlc( AppCommon& app, QWidget* parent );
    virtual ~AppletPlayerNlc();

	RenderGlWidget*				getRenderConsumer( void ) override		{ return ui.m_RenderWidget; }
	QSlider*					getPlayPosSlider( void ) override		{ return ui.m_PlayControlWidget->getPlayPosSlider(); }
	QPushButton*				getReplayButton( void ) override		{ return ui.m_ReplayButton; }
	VxPushButton*				getPlayPauseButton( void ) override		{ return ui.m_PlayControlWidget->getPlayPauseButton(); }
	PlayControlWidget*			getPlayControlWidget( void ) override	{ return ui.m_PlayControlWidget; }

	void						onMediaPlayerNlcReady( bool isReady ) override;

	bool						isMediaPlayerReady( bool notifyIfNotReady = false );

protected slots:
    void                        slotAppletClosing( void );

	void                        slotMenuItemSelected( int menuId, EMenuItemType menuItemType );

	void                        slotMediaFileComboBoxSelectionChange( int cbIdx );

	void						slotBrowseButtonClick( void );
	void						slotReplayButtonClick( void );
    void                        slotSelectFileButtonClick( void );

	void						slotFileWasSelected( QString fileName );

protected:
	void						initAppletPlayerNlc( void );

	void                        setupBottomMenu( VxMenuButton* menuButton );

	void						browseForMovie( void );
	void						onFileSelected( FileInfo& fileInfo );
	void						playSelectedMovie( std::string movieFile );

	int							addMediaFilesToRecentList( QDir& mediaDir );

	void						refreshRecentFilesComboBox( void );
	void						updateRecentListVisibility( void );

	//=== vars ===//
	Ui::AppletPlayerNlcUi		ui;
	static VxFileList			m_RecentFiles;
	bool						m_MediaPlayerReady{ false };
};


