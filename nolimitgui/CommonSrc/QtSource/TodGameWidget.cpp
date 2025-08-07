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

#include "ui_TodGameWidget.h"

#include <QFrame>

QFrame*						TodGameWidget::getButtonFrame( void ) { return ui.m_ButtonFrame; }
VxPushButton*				TodGameWidget::getTruthButton( void ) { return ui.m_TruthButton; }
VxPushButton*				TodGameWidget::getDareButton( void ) { return ui.m_DareButton; }
VxPushButton*				TodGameWidget::getChallengeEmoticon( void ) { return ui.m_EmoticonButton; }
QLabel*						TodGameWidget::getChallengeTextLabel( void ) { return ui.m_InstructLabel; }

//============================================================================
TodGameWidget::TodGameWidget(QWidget* parent)
: QWidget(parent)
, ui(*(new Ui::TodGameWidgetUi))
{
	ui.setupUi(this);
	ui.m_VidWidget->setMediaModule( eMediaModuleTruthOrDare );
	ui.m_EmoticonButton->setFixedSize( eButtonSizeSmall );

	connect( ui.m_DareButton, SIGNAL(clicked()), this, SIGNAL(dareButtonClicked()));
	connect( ui.m_TruthButton, SIGNAL(clicked()), this, SIGNAL(truthButtonClicked()));
}

//============================================================================
void TodGameWidget::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
	ui.m_VidWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
}

//============================================================================
void TodGameWidget::enableGameButtons( bool bEnable )
{
	ui.m_TruthButton->setVisible( bEnable );
	ui.m_DareButton->setVisible( bEnable );
}

//============================================================================
VidWidget* TodGameWidget::getVidWidget( void )
{
	return ui.m_VidWidget;
}

//============================================================================
TodStatsWidget* TodGameWidget::getMyStatsWidget( void )
{
	return ui.m_MyStats;
}

//============================================================================
TodStatsWidget* TodGameWidget::getHisStatsWidget( void )
{
	return ui.m_HisStats;
}