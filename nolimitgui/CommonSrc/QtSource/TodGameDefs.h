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
	eWaitingForChallengeRx,
	eRxedDareChallenge,
	eRxedDareAccepted,
	eRxedDareRejected,
	eRxedTruthChallenge,
	eRxedTruthAccepted,
	eRxedTruthRejected,

	eWaitingForChallengeTx,
	eTxedDareChallenge,
	eTxedDareAccepted,
	eTxedDareRejected,
	eTxedTruthChallenge,
	eTxedTruthAccepted,
	eTxedTruthRejected,

	eTxedOffer,
	eOfferRejected,

	eMaxGameStatus // must be last
};

enum EGameButton
{
	eGameButtonDare,
	eGameButtonTruth,
	eMaxGameButton
};

class TodPlayerStats
{
public:
	TodPlayerStats();

	int32_t						getVar( ETodGameVarId eVarId );
	void					setVar( ETodGameVarId eVarId, int32_t s32Value );

	//=== vars ===//
	int32_t						m_s32DareChallengeCnt;
	int32_t						m_s32DareAcceptedCnt;
	int32_t						m_s32DareRejectedCnt;
	int32_t						m_s32TruthChallengeCnt;
	int32_t						m_s32TruthAcceptedCnt;
	int32_t						m_s32TruthRejectedCnt;
};

class TodGameStats
{
public:
	//=== vars ===//
	TodPlayerStats			m_MyPlayerStats;
	TodPlayerStats			m_FriendPlayerStats;
};
