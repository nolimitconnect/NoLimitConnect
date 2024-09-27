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

#include <QListWidgetItem>
#include <QFrame>

#include <CoreLib/VxFileTypeMasks.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class FileFilterSelectWidgetUi;
}
QT_END_NAMESPACE

class FileItemInfo;
class AppCommon;

class FileFilterSelectWidget : public QFrame
{
	Q_OBJECT

public:
	FileFilterSelectWidget(QWidget* parent=nullptr);
	virtual ~FileFilterSelectWidget() = default;

	void						setFileFilter( EFileFilterType fileFilter );
	EFileFilterType				getFileFilter( void ) { return m_FileFilter; }

	void						setMediaPlayerFileTypesOnly( void ); // show only video and audio buttons

signals:
	void						signalFileFilterChanged( EFileFilterType fileTypeFilter );

protected slots:
	void						slotVideoButtonClicked( void );
	void						slotAudioButtonClicked( void );
	void						slotImageButtonClicked( void );
	void						slotAllButtonClicked( void );

	void						slotVideoLabelClicked( void );
	void						slotAudioLabelClicked( void );
	void						slotImageLabelClicked( void );
	void						slotAllLabelClicked( void );

protected:
	void						updateFilterButtons( void );

	Ui::FileFilterSelectWidgetUi&	ui;

	EFileFilterType				m_FileFilter{ eFileFilterAll };
};

