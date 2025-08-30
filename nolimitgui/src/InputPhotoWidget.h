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

#include "InputBaseWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class InputPhotoWidget;
}
QT_END_NAMESPACE

class InputPhotoWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputPhotoWidget( QWidget* parent=0);

	void						setCanSend( bool canSend ) {};

	void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );

protected slots:
    void						slotSnapShotButtonClicked( void );
	void						slotRotateCamButtonClicked( void );
	void						slotSelectCamButtonClicked( void );
	void						slotCancelPhotoButtonClicked( void );

    void                        slotExitPhotoWidget( void );

protected:
	void						hideEvent( QHideEvent* hideEvent );
	void						showEvent( QShowEvent* showEvent );

	Ui::InputPhotoWidget&		ui;
	QImage						m_PicImage;
};
