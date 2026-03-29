//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileMediaSelectWidget.h"

#include "AppCommon.h"

#include <CoreLib/VxDebug.h>

#include "ui_FileMediaSelectWidget.h"

//============================================================================
FileMediaSelectWidget::FileMediaSelectWidget(QWidget* parent)
: QFrame(parent)
, ui(*(new Ui::FileMediaSelectWidgetUi))
{
	ui.setupUi(this);
	
	ui.m_FileMediaVideoButton->setIcons( eMyIconVideo );
	ui.m_FileMediaAudioButton->setIcons( eMyIconMusic );
	ui.m_FileMediaImageButton->setIcons( eMyIconPhoto );

	ui.m_FileMediaVideoButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileMediaAudioButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileMediaImageButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileMediaFolderButton->setFixedSize( eButtonSizeSmall );

	setScanCancelEnable( false );

	connect( ui.m_FileMediaVideoButton,	SIGNAL(clicked()), this, SLOT(slotVideoButtonClicked()) );
	connect( ui.m_FileMediaAudioButton,	SIGNAL(clicked()), this, SLOT(slotAudioButtonClicked()) );
	connect( ui.m_FileMediaImageButton,	SIGNAL(clicked()), this, SLOT(slotImageButtonClicked()) );
	
	connect( ui.m_VideoLabel,	SIGNAL(clicked()), this, SLOT(slotVideoButtonClicked()) );
	connect( ui.m_AudioLabel,	SIGNAL(clicked()), this, SLOT(slotAudioButtonClicked()) );
	connect( ui.m_ImageLabel,	SIGNAL(clicked()), this, SLOT(slotImageButtonClicked()) );

	connect( ui.m_FileMediaFolderButton, SIGNAL(clicked()), this, SLOT(slotFolderButtonClicked()) );
	connect( ui.m_FolderLabel,			 SIGNAL(clicked()), this, SLOT(slotFolderButtonClicked()) );
}

//============================================================================
void FileMediaSelectWidget::setFileMedia( EMediaFileType eFileMedia )
{
	m_MediaFileType = eFileMedia;
}

//============================================================================
void FileMediaSelectWidget::slotVideoButtonClicked( void )
{
	m_MediaFileType = eMediaFileVideo;
	emit signalFileMediaSelected( m_MediaFileType );
}

//============================================================================
void FileMediaSelectWidget::slotAudioButtonClicked( void )
{
	m_MediaFileType = eMediaFileAudio;
	emit signalFileMediaSelected( m_MediaFileType );
}

//============================================================================
void FileMediaSelectWidget::slotImageButtonClicked( void )
{
	m_MediaFileType = eMediaFileImage;
	emit signalFileMediaSelected( m_MediaFileType );
}

//============================================================================
void FileMediaSelectWidget::slotFolderButtonClicked( void )
{
	emit signalFileFolderSelected();
}

//============================================================================
void FileMediaSelectWidget::setScanFolderVisible( bool visible )
{
	ui.m_FileMediaFolderButton->setVisible( visible );
	ui.m_FolderLabel->setVisible( visible );
}

//============================================================================
void FileMediaSelectWidget::setScanCancelEnable( bool cancelEnable )
{
	if( cancelEnable )
	{
		ui.m_FileMediaFolderButton->setIconOverrideColor( GetAppInstance().getAppTheme().getCancelColor() );
		ui.m_FileMediaFolderButton->setIcons( eMyIconRejectRedX );
		ui.m_FolderLabel->setText( QObject::tr( "Cancel scan" ) );
	}
	else
	{
		ui.m_FileMediaFolderButton->clearIconOverrideColor();
		ui.m_FileMediaFolderButton->setIcons( eMyIconSearchFolder );
		ui.m_FolderLabel->setText( QObject::tr( "Scan folder for media" ) );
	}
}