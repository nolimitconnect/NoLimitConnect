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

#include "ui_InputFaceWidget.h"

class InputFaceWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputFaceWidget( QWidget* parent=0);

	void						setCanSend( bool canSend ) {};

protected slots:
	void						slotCancelFaceButtonClicked( void );
	void						slotFace1LabelClicked( void );
	void						slotFace2LabelClicked( void );
	void						slotFace3LabelClicked( void );
	void						slotFace4LabelClicked( void );
	void						slotFace5LabelClicked( void );
	void						slotFace6LabelClicked( void );
	void						slotFace7LabelClicked( void );
	void						slotFace8LabelClicked( void );
	void						slotFace9LabelClicked( void );
	void						slotFace10LabelClicked( void );
	void						slotFace11LabelClicked( void );
	void						slotFace12LabelClicked( void );
	void						slotFace13LabelClicked( void );
	void						slotFace14LabelClicked( void );
	void						slotFace15LabelClicked( void );
	void						slotFace16LabelClicked( void );
	void						slotFace17LabelClicked( void );
	void						slotFace18LabelClicked( void );
	void						slotFace19LabelClicked( void );
	void						slotFace20LabelClicked( void );
	void						slotFace21LabelClicked( void );
	void						slotFace22LabelClicked( void );
	void						slotFace23LabelClicked( void );
	void						slotFace24LabelClicked( void );
	void						slotFace25LabelClicked( void );
	void						slotFace26LabelClicked( void );
	void						slotFace27LabelClicked( void );
	void						slotFace28LabelClicked( void );
	void						slotFace29LabelClicked( void );
	void						slotFace30LabelClicked( void );
	void						slotFace31LabelClicked( void );
	void						slotFace32LabelClicked( void );


protected:
	virtual QSize				sizeHint() const;
	void						faceLabelClicked( int faceIdx );

	Ui::InputFaceWidgetClass	ui;
	QVector<VxLabel *>		m_FaceList;	
};
