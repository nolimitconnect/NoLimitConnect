//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TodGameDefs.h"

//============================================================================
TodPlayerStats::TodPlayerStats()
: m_s32DareChallengeCnt(0)
, m_s32DareAcceptedCnt(0)
, m_s32DareRejectedCnt(0)
, m_s32TruthChallengeCnt(0)
, m_s32TruthAcceptedCnt(0)
, m_s32TruthRejectedCnt(0)
{
}

//============================================================================
int32_t TodPlayerStats::getVar( ETodGameVarId eVarId )
{
	switch(eVarId)
	{
	case eTodGameVarIdDareChallengeCnt:
		return m_s32DareChallengeCnt;
	case eTodGameVarIdDareAcceptedCnt:
		return m_s32DareAcceptedCnt;
	case eTodGameVarIdDareRejectedCnt:
		return m_s32DareRejectedCnt;
	case eTodGameVarIdTruthChallengeCnt:
		return m_s32TruthChallengeCnt;
	case eTodGameVarIdTruthAcceptedCnt:
		return m_s32TruthAcceptedCnt;
	case eTodGameVarIdTruthRejectedCnt:
		return m_s32TruthRejectedCnt;
	default:
		return 0;
	}
}

//============================================================================
void TodPlayerStats::setVar( ETodGameVarId eVarId, int32_t s32Value )
{
	switch(eVarId)
	{
	case eTodGameVarIdDareChallengeCnt:
		m_s32DareChallengeCnt = s32Value;
		break;
	case eTodGameVarIdDareAcceptedCnt:
		m_s32DareAcceptedCnt = s32Value;
		break;
	case eTodGameVarIdDareRejectedCnt:
		m_s32DareRejectedCnt = s32Value;
		break;
	case eTodGameVarIdTruthChallengeCnt:
		m_s32TruthChallengeCnt = s32Value;
		break;
	case eTodGameVarIdTruthAcceptedCnt:
		m_s32TruthAcceptedCnt = s32Value;
		break;
	case eTodGameVarIdTruthRejectedCnt:
		m_s32TruthRejectedCnt = s32Value;
		break;
	default:
		break;
	}
}
