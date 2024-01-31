//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "FileItemInfo.h"
#include "FileActionBarWidget.h"
#include "AppCommon.h"

//============================================================================
FileActionBarWidget::FileActionBarWidget(QWidget* parent)
: QFrame(parent)
, m_MyApp( GetAppInstance() )
{
	ui.setupUi(this);
	
	ui.m_PlayFileButton->setIcons( eMyIconPlayNormal );
	ui.m_PlayExternallyButton->setIcons( eMyIconPlayExtern );
	ui.m_FileInLibraryButton->setIcons( eMyIconLibraryNormal );
	ui.m_FileShareButton->setIcons( eMyIconShareFilesNormal );
	ui.m_ShredFileButton->setIcons( eMyIconShredderNormal );

	ui.m_PlayFileButton->setFixedSize( eButtonSizeSmall );
	ui.m_PlayExternallyButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileInLibraryButton->setFixedSize( eButtonSizeSmall );
	ui.m_FileShareButton->setFixedSize( eButtonSizeSmall );
	ui.m_ShredFileButton->setFixedSize( eButtonSizeSmall );

	connect( ui.m_PlayFileButton,		SIGNAL(clicked()), this, SLOT(slotPlayButtonClicked()) );
	connect( ui.m_PlayExternallyButton,	SIGNAL(clicked()), this, SLOT(slotPlayExternButtonClicked()) );
	connect( ui.m_FileInLibraryButton,	SIGNAL(clicked()), this, SLOT(slotLibraryButtonClicked()) );
	connect( ui.m_FileShareButton,		SIGNAL(clicked()), this, SLOT(slotFileShareButtonClicked()) );
	connect( ui.m_ShredFileButton,		SIGNAL(clicked()), this, SLOT(slotShredButtonClicked()) );
}

//============================================================================
void FileActionBarWidget::setSelectAFileMode( bool selectAFile )
{
	ui.m_PlayFileButton->setIcons( selectAFile ? eMyIconCheckMark : eMyIconPlayNormal );
}

//============================================================================
void FileActionBarWidget::slotPlayButtonClicked( void )
{
	emit signalPlayButtonClicked();
}

//============================================================================
void FileActionBarWidget::slotPlayExternButtonClicked( void )
{
	emit signalPlayExternButtonClicked();
}

//============================================================================
void FileActionBarWidget::slotLibraryButtonClicked( void )
{
	emit signalLibraryButtonClicked();
}

//============================================================================
void FileActionBarWidget::slotFileShareButtonClicked( void )
{
	emit signalFileShareButtonClicked();
}

//============================================================================
void FileActionBarWidget::slotShredButtonClicked( void )
{
	emit signalShredButtonClicked();
}

//============================================================================
void FileActionBarWidget::setIsInLibrary( bool isInLibrary )
{
	ui.m_FileInLibraryButton->setNotifyType( isInLibrary ? eNotifyOnline : eNotifyNone );
}

//============================================================================
void FileActionBarWidget::setIsSharedFile( bool isShared )
{
	ui.m_FileShareButton->setNotifyType( isShared ? eNotifyOnline : eNotifyNone );
}
