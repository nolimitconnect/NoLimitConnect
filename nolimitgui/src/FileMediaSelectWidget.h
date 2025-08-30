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

#include "GuiHelpers.h"

#include <QFrame>

QT_BEGIN_NAMESPACE
namespace Ui {
    class FileMediaSelectWidgetUi;
}
QT_END_NAMESPACE

class FileItemInfo;
class AppCommon;

class FileMediaSelectWidget : public QFrame
{
	Q_OBJECT

public:
	FileMediaSelectWidget(QWidget* parent=nullptr);
	virtual ~FileMediaSelectWidget() = default;

	void						setFileMedia( EMediaFileType fileType );
	EMediaFileType				getFileMedia( void ) { return m_MediaFileType; }

signals:
	void						signalFileMediaSelected( EMediaFileType fileType );

protected slots:
	void						slotVideoButtonClicked( void );
	void						slotAudioButtonClicked( void );
	void						slotImageButtonClicked( void );

protected:

	Ui::FileMediaSelectWidgetUi&	ui;
	EMediaFileType				m_MediaFileType{ eMediaFileAny };
};


