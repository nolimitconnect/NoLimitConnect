//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileFilterSelectWidget.h"

#include <CoreLib/VxDebug.h>

#include "ui_FileFilterSelectWidget.h"

//============================================================================
FileFilterSelectWidget::FileFilterSelectWidget(QWidget* parent)
: QFrame(parent)
, ui(*(new Ui::FileFilterSelectWidgetUi))
{
	ui.setupUi(this);
	
	ui.m_FileTypeVideoButton->setIcons( eMyIconVideo );
	ui.m_FileTypeAudioButton->setIcons( eMyIconMusic );
	ui.m_FileTypeImageButton->setIcons( eMyIconPhoto );
	ui.m_FileTypeAllButton->setIcons( eMyIconShareFilesNormal );

	ui.m_FileTypeVideoButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileTypeAudioButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileTypeImageButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileTypeAllButton->setFixedSize( eButtonSizeSmall );

	connect( ui.m_FileTypeVideoButton,	SIGNAL(clicked()), this, SLOT(slotVideoButtonClicked()) );
	connect( ui.m_FileTypeAudioButton,	SIGNAL(clicked()), this, SLOT(slotAudioButtonClicked()) );
	connect( ui.m_FileTypeImageButton,	SIGNAL(clicked()), this, SLOT(slotImageButtonClicked()) );
	connect( ui.m_FileTypeAllButton,	SIGNAL(clicked()), this, SLOT(slotAllButtonClicked()) );
	
	connect( ui.m_VideoLabel,	SIGNAL(clicked()), this, SLOT(slotVideoLabelClicked()) );
	connect( ui.m_AudioLabel,	SIGNAL(clicked()), this, SLOT(slotAudioLabelClicked()) );
	connect( ui.m_ImageLabel,	SIGNAL(clicked()), this, SLOT(slotImageLabelClicked()) );
	connect( ui.m_AllLabel,		SIGNAL(clicked()), this, SLOT(slotAllLabelClicked()) );

	ui.m_FileTypeAllButton->setNotifyOnline( true );
}

//============================================================================
void FileFilterSelectWidget::setFileFilter( EFileFilterType eFileFilter )
{
	m_FileFilter = eFileFilter;
	updateFilterButtons();
}

//============================================================================
void FileFilterSelectWidget::setMediaPlayerFileTypesOnly( void )
{
	ui.m_FileTypeImageButton->setVisible( false );
	ui.m_ImageLabel->setVisible( false );
	ui.m_FileTypeAllButton->setVisible( false );
	ui.m_AllLabel->setVisible( false );
}

//============================================================================
void FileFilterSelectWidget::slotVideoButtonClicked( void )
{
	if( m_FileFilter != eFileFilterVideo )
	{
		m_FileFilter = eFileFilterVideo;
		updateFilterButtons();
		emit signalFileFilterChanged( m_FileFilter );
	}
}

//============================================================================
void FileFilterSelectWidget::slotAudioButtonClicked( void )
{
	if( m_FileFilter != eFileFilterAudio )
	{
		m_FileFilter = eFileFilterAudio;
		updateFilterButtons();
		emit signalFileFilterChanged( m_FileFilter );
	}
}

//============================================================================
void FileFilterSelectWidget::slotImageButtonClicked( void )
{
	if( m_FileFilter != eFileFilterPhoto )
	{
		m_FileFilter = eFileFilterPhoto;
		updateFilterButtons();
		emit signalFileFilterChanged( m_FileFilter );
	}
}

//============================================================================
void FileFilterSelectWidget::slotAllButtonClicked( void )
{
	if( m_FileFilter != eFileFilterAll )
	{
		m_FileFilter = eFileFilterAll;
		updateFilterButtons();
		emit signalFileFilterChanged( m_FileFilter );
	}
}

//============================================================================
void FileFilterSelectWidget::updateFilterButtons( void )
{
	ui.m_FileTypeVideoButton->setNotifyOnline( false );
    ui.m_FileTypeAudioButton->setNotifyOnline( false );
    ui.m_FileTypeImageButton->setNotifyOnline( false );
    ui.m_FileTypeAllButton->setNotifyOnline( false );

	switch( m_FileFilter )
	{
	case eFileFilterAll:
		ui.m_FileTypeAllButton->setNotifyOnline( true );
		break;
	case eFileFilterPhoto:
		ui.m_FileTypeImageButton->setNotifyOnline( true );
		break;
	case eFileFilterAudio:
		ui.m_FileTypeAudioButton->setNotifyOnline( true );
		break;
	case eFileFilterVideo:
		ui.m_FileTypeVideoButton->setNotifyOnline( true );
		break;
	default:
		LogMsg( LOG_ERROR, "%s Unknown Filter Type %d", __func__, m_FileFilter );
	}    
}

//============================================================================
void FileFilterSelectWidget::slotVideoLabelClicked( void )
{
	if( m_FileFilter != eFileFilterVideo )
	{
		ui.m_FileTypeVideoButton->emulateUserClicked();
	}
}

//============================================================================
void FileFilterSelectWidget::slotAudioLabelClicked( void )
{
	if( m_FileFilter != eFileFilterAudio )
	{
		ui.m_FileTypeAudioButton->emulateUserClicked();
	}
}

//============================================================================
void FileFilterSelectWidget::slotImageLabelClicked( void )
{
	if( m_FileFilter != eFileFilterPhoto )
	{
		ui.m_FileTypeImageButton->emulateUserClicked();
	}
}

//============================================================================
void FileFilterSelectWidget::slotAllLabelClicked( void )
{
	if( m_FileFilter != eFileFilterAll )
	{
		ui.m_FileTypeAllButton->emulateUserClicked();
	}
}
