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

#include "GuiUser.h"
#include "TodStatsWidget.h"

#include <CoreLib/VxDebug.h>

//============================================================================
void TodPlayerStats::setStatsWidget( GuiUser* guiUser, TodStatsWidget* statsWidget )
{
	vx_assert( guiUser );
	vx_assert( statsWidget );

	m_StatsWidget = statsWidget;
	m_GuiUser = guiUser;
	m_StatsWidget->setUserName( guiUser->getOnlineName() );
	m_DareAcceptedCnt = guiUser->getDareAcceptCount();
	m_DareRejectedCnt = guiUser->getDareRejectCount();
	m_TruthAcceptedCnt = guiUser->getTruthAcceptCount();
	m_TruthRejectedCnt = guiUser->getTruthRejectCount();

	updateStatsWidget();
}

//============================================================================
uint32_t TodPlayerStats::getVar( ETodGameVarId eVarId )
{
	switch(eVarId)
	{
	case eTodGameVarIdTruthChoiceCnt:
		return m_TruthChallengeCnt;
	case eTodGameVarIdTruthAcceptedCnt:
		return m_TruthAcceptedCnt;
	case eTodGameVarIdTruthRejectedCnt:
		return m_TruthRejectedCnt;

	case eTodGameVarIdDareChoiceCnt:
		return m_DareChallengeCnt;
	case eTodGameVarIdDareAcceptedCnt:
		return m_DareAcceptedCnt;
	case eTodGameVarIdDareRejectedCnt:
		return m_DareRejectedCnt;

	default:
		return 0;
	}
}

//============================================================================
bool TodPlayerStats::setVar( ETodGameVarId eVarId, uint32_t s32Value )
{
	vx_assert( m_GuiUser );
	vx_assert( m_StatsWidget );

	bool needUserUpdate{ false };
	switch(eVarId)
	{
	case eTodGameVarIdTruthChoiceCnt:
		m_TruthChallengeCnt = s32Value;
		break;
	case eTodGameVarIdTruthAcceptedCnt:
		m_TruthAcceptedCnt = s32Value;
		needUserUpdate = true;
		break;
	case eTodGameVarIdTruthRejectedCnt:
		m_TruthRejectedCnt = s32Value;
		needUserUpdate = true;
		break;

	case eTodGameVarIdDareChoiceCnt:
		m_DareChallengeCnt = s32Value;
		break;
	case eTodGameVarIdDareAcceptedCnt:
		m_DareAcceptedCnt = s32Value;
		needUserUpdate = true;
		break;
	case eTodGameVarIdDareRejectedCnt:
		m_DareRejectedCnt = s32Value;
		needUserUpdate = true;
		break;

	default:
		break;
	}

	updateStatsWidget();

	if( needUserUpdate )
	{
		m_StatsHaveChanged = true;
	}

	return needUserUpdate;
}

//============================================================================
void TodPlayerStats::updateStatsWidget( void )
{
	//LogMsg( LOG_VERBOSE, "TodPlayerStats::%s %s %s truth challenges %d truths %d rejects %d", __func__, m_GuiUser->getOnlineName().c_str(),
	//	m_StatsWidget->getUserName().toUtf8().constData(), m_TruthChallengeCnt, m_TruthAcceptedCnt, m_TruthRejectedCnt );
	//LogMsg( LOG_VERBOSE, "TodPlayerStats::%s %s %s dares challenges %d dares %d rejects %d", __func__, m_GuiUser->getOnlineName().c_str(),
	//	m_StatsWidget->getUserName().toUtf8().constData(), m_DareChallengeCnt, m_DareAcceptedCnt, m_DareRejectedCnt );
	m_StatsWidget->setTruthChallengeCnt( m_TruthChallengeCnt );
	m_StatsWidget->setTruthAcceptCnt( m_TruthAcceptedCnt );
	m_StatsWidget->setTruthRejectCnt( m_TruthRejectedCnt );
	m_StatsWidget->setDareChallengeCnt( m_DareChallengeCnt );
	m_StatsWidget->setDareAcceptCnt( m_DareAcceptedCnt );
	m_StatsWidget->setDareRejectCnt( m_DareRejectedCnt );
	m_StatsWidget->update();
	m_StatsWidget->repaint();
}

//============================================================================
void TodPlayerStats::clear( void )
{
	m_StatsHaveChanged = false;
	m_TruthChallengeCnt = 0;
	m_TruthAcceptedCnt = 0;
	m_TruthRejectedCnt = 0;

	m_DareChallengeCnt = 0;
	m_DareAcceptedCnt = 0;
	m_DareRejectedCnt = 0;
}
