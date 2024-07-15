#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class HistoryEntryWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;
class P2PEngine;

class HistoryEntryWidget : public QWidget, public QListWidgetItem
{
	Q_OBJECT

public:
	HistoryEntryWidget(QWidget* parent=nullptr);

protected:
	Ui::HistoryEntryWidgetClass&	ui;

	AppCommon&					m_MyApp;
	P2PEngine&					m_Engine;
};
