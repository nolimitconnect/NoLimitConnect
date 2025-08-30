#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QPlainTextEdit>

class VxPlainTextEdit : public QPlainTextEdit
{
	Q_OBJECT
public:
	VxPlainTextEdit( QWidget* parent = nullptr );
	VxPlainTextEdit( const QString &text, QWidget* parent = nullptr );
	virtual ~VxPlainTextEdit() = default;

    virtual void 	            contextMenuEvent( QContextMenuEvent *event ) override;
};

