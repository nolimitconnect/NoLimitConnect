//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TodStatsWidget.h"

#include "ui_TodStatsWidget.h"

//============================================================================
TodStatsWidget::TodStatsWidget( QWidget* parent )
: QWidget( parent )
, ui( *( new Ui::TodStatsUi ) )
{
	ui.setupUi( this );
}

//============================================================================
void TodStatsWidget::setUserName( std::string userName )
{
	ui.m_NameLabel->setText( userName.c_str() );
}

//============================================================================
QString TodStatsWidget::getUserName( void )
{
	return ui.m_NameLabel->text();
}

//============================================================================
void TodStatsWidget::setTruthChallengeCnt( uint32_t cnt )
{
	//QString strCnt = QString::number( cnt );
	//ui.m_TruthChallengeCnt->setText( strCnt );
}

//============================================================================
void TodStatsWidget::setTruthAcceptCnt( uint32_t cnt )
{
	QString strCnt = QString::number( cnt );
	ui.m_TruthAcceptedCnt->setText( strCnt );
}

//============================================================================
void TodStatsWidget::setTruthRejectCnt( uint32_t cnt )
{
	QString strCnt = QString::number( cnt );
	ui.m_TruthRejectedCnt->setText( strCnt );
}

//============================================================================
void TodStatsWidget::setDareChallengeCnt( uint32_t cnt )
{
	//QString strCnt = QString::number( cnt );
	//ui.m_DareChallengeCnt->setText( strCnt );
}

//============================================================================
void TodStatsWidget::setDareAcceptCnt( uint32_t cnt )
{
	QString strCnt = QString::number( cnt );
	ui.m_DareAcceptedCnt->setText( strCnt );
}

//============================================================================
void TodStatsWidget::setDareRejectCnt( uint32_t cnt )
{
	QString strCnt = QString::number( cnt );
	ui.m_DareRejectedCnt->setText( strCnt );
}

