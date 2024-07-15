#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QDialog>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
    class TimedMessageDlg;
}
QT_END_NAMESPACE

class ActivityTimedMessage : public QDialog
{
	Q_OBJECT

public:
	ActivityTimedMessage( QString strTitle, QString strMessage, int iTimeoutSeconds = 4, QWidget* parent = nullptr );
	virtual ~ActivityTimedMessage() override = default;

private:
	Ui::TimedMessageDlg&		ui;
	QTimer						m_CloseDlgTimer;

};
