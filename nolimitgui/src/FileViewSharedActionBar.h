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

#include <QFrame>

QT_BEGIN_NAMESPACE
namespace Ui {
    class FileViewSharedActionBarUi;
}
QT_END_NAMESPACE

class FileViewSharedActionBar : public QFrame
{
	Q_OBJECT

public:
	FileViewSharedActionBar(QWidget* parent=nullptr);
	virtual ~FileViewSharedActionBar() = default;

signals:
	void						signalStreamButtonClicked( void );
	void						signalDownloadButtonClicked( void );
	void						signalAboutFileButtonClicked( void );

protected slots:
	void						slotStreamButtonClicked( void );
	void						slotDownloadButtonClicked( void );
	void						slotAboutFileButtonClicked( void );

protected:
	Ui::FileViewSharedActionBarUi&	ui;
};

