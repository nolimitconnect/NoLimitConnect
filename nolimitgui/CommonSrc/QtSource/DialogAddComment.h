#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include <CoreLib/AssetDefs.h>
#include <QDialog>
#include "ui_DialogAddComment.h"

class AssetBaseInfo;
class AppCommon;
class MyIcons;

class DialogAddComment : public QDialog
{
	Q_OBJECT

public:
    DialogAddComment( AssetBaseInfo& assetInfo, QWidget* parent = nullptr );
	virtual ~DialogAddComment() = default;
	DialogAddComment( QWidget* parent = nullptr ) = delete;// dont allow without asset info construct

	QString						getCommentText()	{ return m_CommentText; }

protected slots:
	void						slotAccepted( void );

protected:
	Ui::DialogAddCommentUi		ui;
	AppCommon&					m_MyApp;
	QString						m_CommentText;
};
