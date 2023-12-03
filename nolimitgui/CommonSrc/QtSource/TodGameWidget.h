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

#include <QBitmap>
#include "ui_TodGameWidget.h"

class VxGUID;

class TodGameWidget : public QWidget
{
	Q_OBJECT

public:
	Ui::TodGameWidget ui;

	TodGameWidget( QWidget* parent = nullptr );

	VidWidget *		getVidWidget( void );

	void			callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );
	void			enableGameButtons( bool bEnable );

signals:
	void			truthButtonClicked();
	void			dareButtonClicked();

};
