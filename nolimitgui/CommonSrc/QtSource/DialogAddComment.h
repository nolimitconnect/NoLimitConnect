#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
