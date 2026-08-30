#pragma once
#include <QWidget>
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


QT_BEGIN_NAMESPACE
namespace Ui {
	class TodStatsUi;
}
QT_END_NAMESPACE

class QLabel;

class TodStatsWidget : public QWidget
{
	Q_OBJECT

public:
	TodStatsWidget( QWidget* parent );

	void						setUserName( std::string userName );
	QString						getUserName( void );

	void						setTruthChallengeCnt( uint32_t cnt );
	void						setTruthAcceptCnt( uint32_t cnt );
	void						setTruthRejectCnt( uint32_t cnt );

	void						setDareChallengeCnt( uint32_t cnt );
	void						setDareAcceptCnt( uint32_t cnt );
	void						setDareRejectCnt( uint32_t cnt );


protected:
	Ui::TodStatsUi&				ui;
};
