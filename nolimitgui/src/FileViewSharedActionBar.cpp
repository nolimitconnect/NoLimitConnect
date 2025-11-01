//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileViewSharedActionBar.h"

#include "AppCommon.h"
#include "FileItemInfo.h"

#include "ui_FileViewSharedActionBar.h"

//============================================================================
FileViewSharedActionBar::FileViewSharedActionBar(QWidget* parent)
: QFrame(parent)
, ui(*(new Ui::FileViewSharedActionBarUi))
{
	ui.setupUi(this);
	
	ui.m_StreamButton->setIcons( eMyIconPlayStream );
	ui.m_DownloadButton->setIcons( eMyIconFileDownload );
	ui.m_FileAboutButton->setIcons( eMyIconInformation );

	ui.m_StreamButton->setFixedSize( eButtonSizeSmall );
	ui.m_DownloadButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileAboutButton->setFixedSize( eButtonSizeSmall );

	connect( ui.m_StreamButton,			SIGNAL(clicked()), this, SLOT(slotStreamButtonClicked()) );
	connect( ui.m_StreamLabel,			SIGNAL(clicked()), this, SLOT(slotStreamButtonClicked()) );
	connect( ui.m_DownloadButton,		SIGNAL(clicked()), this, SLOT(slotDownloadButtonClicked()) );
	connect( ui.m_DownloadLabel,		SIGNAL(clicked()), this, SLOT(slotDownloadButtonClicked()) );
	connect( ui.m_FileAboutButton,		SIGNAL(clicked()), this, SLOT(slotAboutFileButtonClicked()) );
	connect( ui.m_FileAboutLabel,		SIGNAL(clicked()), this, SLOT(slotAboutFileButtonClicked()) );
}

//============================================================================
void FileViewSharedActionBar::slotStreamButtonClicked( void )
{
	emit signalStreamButtonClicked();
}

//============================================================================
void FileViewSharedActionBar::slotDownloadButtonClicked( void )
{
	emit signalDownloadButtonClicked();
}

//============================================================================
void FileViewSharedActionBar::slotAboutFileButtonClicked( void )
{
	emit signalAboutFileButtonClicked();
}
