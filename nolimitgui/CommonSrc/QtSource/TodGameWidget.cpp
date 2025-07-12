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

VxPushButton*				TodGameWidget::getTruthButton( void ) { return ui.m_TruthButton; }
VxPushButton*				TodGameWidget::getDareButton( void ) { return ui.m_DareButton; }
VxLabel*					TodGameWidget::getChallengeImageLabel( void ) { return ui.m_ChallengeImageLabel; }
QLabel*						TodGameWidget::getChallengeTextLabel( void ) { return ui.m_ChallengeTextLabel; }
QLabel*						TodGameWidget::getMyDaresLabel( void ) { return ui.MyDaresLabel; }
QLabel*						TodGameWidget::getMyTruthsLabel( void ) { return ui.MyTruthsLabel; }
QLabel*						TodGameWidget::getFriendDaresLabel( void ) { return ui.FriendDaresLabel; }
QLabel*						TodGameWidget::getFriendTruthsLabel( void ) { return ui.FriendTruthsLabel; }
QLabel*						TodGameWidget::getTodStatusLabel( void ) { return ui.m_TodStatusLabel; }

//============================================================================
TodGameWidget::TodGameWidget(QWidget* parent)
: QWidget(parent)
, ui(*(new Ui::TodGameWidgetUi))
{
	ui.setupUi(this);
	ui.TruthOrDareWidget->setMediaModule( eMediaModuleTruthOrDare );

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
