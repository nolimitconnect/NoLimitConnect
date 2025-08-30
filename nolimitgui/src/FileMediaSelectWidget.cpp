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

	connect( ui.m_FileMediaVideoButton,	SIGNAL(clicked()), this, SLOT(slotVideoButtonClicked()) );
	connect( ui.m_FileMediaAudioButton,	SIGNAL(clicked()), this, SLOT(slotAudioButtonClicked()) );
	connect( ui.m_FileMediaImageButton,	SIGNAL(clicked()), this, SLOT(slotImageButtonClicked()) );
	
	connect( ui.m_VideoLabel,	SIGNAL(clicked()), this, SLOT(slotVideoButtonClicked()) );
	connect( ui.m_AudioLabel,	SIGNAL(clicked()), this, SLOT(slotAudioButtonClicked()) );
	connect( ui.m_ImageLabel,	SIGNAL(clicked()), this, SLOT(slotImageButtonClicked()) );
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
