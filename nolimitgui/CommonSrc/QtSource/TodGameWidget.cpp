//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TodGameWidget.h"

//============================================================================
TodGameWidget::TodGameWidget(QWidget* parent)
: QWidget(parent)
{
	ui.setupUi(this);

	connect( ui.m_DareButton, SIGNAL(clicked()), this, SIGNAL(dareButtonClicked()));
	connect( ui.m_TruthButton, SIGNAL(clicked()), this, SIGNAL(truthButtonClicked()));
}

//============================================================================
void TodGameWidget::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
	ui.TruthOrDareWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
}

//============================================================================
void TodGameWidget::enableGameButtons( bool bEnable )
{
	ui.m_TruthButton->setVisible( bEnable );
	ui.m_DareButton->setVisible( bEnable );
}

//============================================================================
VidWidget * TodGameWidget::getVidWidget( void )
{
	return ui.TruthOrDareWidget;
}
