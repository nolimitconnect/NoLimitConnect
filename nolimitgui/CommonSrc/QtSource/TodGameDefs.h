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

#include "TodGameWidget.h"

#include <PktLib/PktsTodGame.h>
#include <PktLib/VxCommon.h>

enum EGameStatus
{
	eWaitingForChoiceRx,
	eRxedDareChoice,
	eRxedDareAccepted,
	eRxedDareRejected,
	eRxedTruthChoice,
	eRxedTruthAccepted,
	eRxedTruthRejected,

	eWaitingForChoiceTx,
	eTxedChoiceDare,
	eTxedChoiceTruth,

	eTxedEvaluation,


	eWaitingGameStart,
	eGameEnded,

	eMaxGameStatus // must be last
};

enum EGameButton
{
	eGameButtonDare,
	eGameButtonTruth,

	eMaxGameButton
};

class GuiUser;

class TodPlayerStats
{
public:
	TodPlayerStats() = default;
	void                        setStatsWidget( GuiUser* guiUser, TodStatsWidget* statsWidget );

	uint32_t					getVar( ETodGameVarId varId );
	bool						setVar( ETodGameVarId varId, uint32_t varValue ); // return true if user values changed

	void						updateStatsWidget( void );

	bool						statsHaveChanged( void ) { return m_StatsHaveChanged; }
	void						clear( void );

	//=== vars ===//
	GuiUser*					m_GuiUser{ nullptr };
	TodStatsWidget*				m_StatsWidget{ nullptr };
	bool						m_StatsHaveChanged{ false };

	uint32_t					m_TruthChallengeCnt{ 0 };
	uint32_t					m_TruthAcceptedCnt{ 0 };
	uint32_t					m_TruthRejectedCnt{ 0 };

	uint32_t					m_DareChallengeCnt{ 0 };
	uint32_t					m_DareAcceptedCnt{ 0 };
	uint32_t					m_DareRejectedCnt{ 0 };
};

class TodGameStats
{
public:
	//=== vars ===//
	TodPlayerStats				m_MyStats;
	TodPlayerStats				m_HisStats;
};
