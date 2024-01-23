#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_FileActionBarWidget.h"
#include <QListWidgetItem>
#include <QFrame>

class FileItemInfo;
class AppCommon;

class FileActionBarWidget : public QFrame
{
	Q_OBJECT

public:
	FileActionBarWidget(QWidget* parent=nullptr);
	virtual ~FileActionBarWidget() = default;

	void						setIsInLibrary( bool isInLibrary );
	void						setIsSharedFile( bool isShared );

signals:
	void						signalPlayButtonClicked( void );
	void						signalPlayExternButtonClicked( void );
	void						signalLibraryButtonClicked( void );
	void						signalFileShareButtonClicked( void );
	void						signalShredButtonClicked( void );

protected slots:
	void						slotPlayButtonClicked( void );
	void						slotPlayExternButtonClicked( void );
	void						slotLibraryButtonClicked( void );
	void						slotFileShareButtonClicked( void );
	void						slotShredButtonClicked( void );

protected:
	Ui::FileActionBarWidgetClass	ui;
    AppCommon&					m_MyApp;
};

