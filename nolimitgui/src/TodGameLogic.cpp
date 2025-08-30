//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TodGameLogic.h"

#include "AppCommon.h"
#include "TodGameWidget.h"

#include "VxLabel.h"
#include "VxPushButton.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

#include <vector>

#include <QFrame>
#include <QLabel>

namespace
{
	std::vector<VxGUID>	g_EmoticonIdList{
{ 3913462368200503545U, 2760340527898317750U },	  // 1 !364F694A1A7330F9264EB315D07E53B6! // 0 thumbs up
{ 13999558228189016709U, 7413105485242018473U },  // 2 !C2486C09239A728566E0A4299A59AAA9! // 1 thumbs down
{ 10829822772292086897U, 16991265692937849009U }, // 3 !964B426EBAE7EC71EBCD1A1FC7D788B1! // 2 smile
{ 7945445069844048192U, 12229757484046445461U },  // 4 !6E43E431BA6EE940A9B8D3A2BDC36B95! // 3 sun glasses
{ 2324257314457588032U, 17711723986212541605U },  // 5 !20416BB68AD91140F5CCAF1FEDF6D4A5! // 4 angel
{ 10756322002679464500U, 16760991902078506408U }, // 6 !954621DF3B6D8234E89B0154D69695A8! // 5 devil
{ 3837853253425696308U, 1553238764248287121U },   // 34 !3542CB333E840E34158E366D3A62C791! // 6 hand on chin (thinking)
{ 13352407061259212155U, 13577725499120930184U }, // 16 !B94D477A66C5D57BBC6DC56750DDCD88! // 7 poop
	};

	VxGUID getEmoticonGuid( ETodEmoticon emoticon )
	{
		switch( emoticon )
		{
		case eTodEmiticonSmile:
			return g_EmoticonIdList[2];
		case eTodEmoticonThumbsUp:
			return g_EmoticonIdList[0];
		case eTodEmoticonThumbsDown:
			return g_EmoticonIdList[1];
		case eTodEmiticonAngel:
			return g_EmoticonIdList[4];
		case eTodEmiticonDevil:
			return g_EmoticonIdList[5];
		case eTodEmiticonThinking:
			return g_EmoticonIdList[6];
		default:
			return g_EmoticonIdList[7];
		}
	}

	const char* DescribeGameStatus( EGameStatus gameStatus )
	{
		switch( gameStatus )
		{
		case eWaitingForChoiceRx:
			return "eWaitingForChoiceRx";
		case eRxedDareChoice:
			return "eRxedDareChoice";
		case eRxedDareAccepted:
			return "eRxedDareAccepted";		
		case eRxedDareRejected:
			return "eRxedDareRejected";		
		case eRxedTruthChoice:
			return "eRxedTruthChoice";		
		case eRxedTruthAccepted:
			return "eRxedTruthAccepted";		
		case eRxedTruthRejected:
			return "eRxedTruthRejected";		
		case eWaitingForChoiceTx:
			return "eWaitingForChoiceTx";		
		case eTxedChoiceDare:
			return "eTxedChoiceDare";
		case eTxedChoiceTruth:
			return "eTxedChoiceTruth";
		case eTxedEvaluation:
			return "eTxedEvaluation";
		case eWaitingGameStart:
			return "eWaitingGameStart";
		case eGameEnded:
			return "eGameEnded";
		default:
			return "GameStatus UNKNOWN";
		}
	}
};

//============================================================================
TodGameLogic::TodGameLogic( AppCommon& myApp, P2PEngine& engine, EPluginType pluginType, QWidget* parent )
: QWidget( parent )
, m_MyApp( myApp )
, m_Engine( engine )
, m_PluginType( pluginType )
, m_MySettings( myApp.getEngine().getEngineSettings() )
{
	m_CallbacksRequested = true;
	m_MyApp.getTodGameMgr().wantTodGamCallbacks( this, true );
}

//============================================================================
TodGameLogic::~TodGameLogic()
{
	if( m_CallbacksRequested )
	{
		m_MyApp.getTodGameMgr().wantTodGamCallbacks( this, false );
	}
}

//============================================================================
void TodGameLogic::setGuiWidgets( GuiUser* hisIdent, TodGameWidget* todGameWidget, TodStatsWidget* myStatsWidget, TodStatsWidget* hisStatsWidget )
{
	m_HisIdent = hisIdent;
	m_TodGameWidget	= todGameWidget;
	m_MyStats.setStatsWidget( m_MyApp.getUserMgr().getMyIdent(), myStatsWidget );
	m_HisStats.setStatsWidget( hisIdent, hisStatsWidget );

	connect( m_TodGameWidget->getTruthButton(), SIGNAL(clicked()), this, SLOT(slotTruthButtonClicked()));
	connect( m_TodGameWidget->getDareButton(), SIGNAL(clicked()), this, SLOT(slotDareButtonClicked()));
	setGameStatus( eWaitingGameStart );
}

//============================================================================
void TodGameLogic::setChallengeStatus( ETodEmoticon emoticon, QString statusText )
{
	VxGUID thumbId = getEmoticonGuid( emoticon );
	QImage emoteImage;
	m_MyApp.getThumbMgr().getThumbImage( thumbId, emoteImage );
	if( !emoteImage.isNull() )
	{
		m_TodGameWidget->getChallengeEmoticon()->setIconOverrideImage( emoteImage );
	}

	m_TodGameWidget->getChallengeTextLabel()->setText(statusText);
	if( m_eGameStatus != eWaitingGameStart )
	{
		emit signalGameStatus( statusText );
	}
}

//============================================================================
void TodGameLogic::saveMyGameStats( void )
{
	VxNetIdent* myIdent = m_MyApp.getMyNetIdent();

	uint32_t val = m_MyStats.getVar( eTodGameVarIdDareAcceptedCnt );
	myIdent->setDareAcceptCount( val );
	uint32_t rejectedCnt = m_MyStats.getVar( eTodGameVarIdDareRejectedCnt );
	myIdent->setDareRejectCount( rejectedCnt );

	val = m_MyStats.getVar( eTodGameVarIdTruthAcceptedCnt );
	myIdent->setTruthAcceptCount( val );
	rejectedCnt = m_MyStats.getVar( eTodGameVarIdTruthRejectedCnt );
	myIdent->setTruthRejectCount( rejectedCnt );

    m_MyApp.updateMyIdent( myIdent, true );
}

//============================================================================
void TodGameLogic::showGameButtons( bool show )
{
	m_TodGameWidget->getButtonFrame()->setVisible(show);
}

//============================================================================
void TodGameLogic::setGameButtonsText( ETodButtonText todButtonText )
{
	switch( todButtonText )
	{
	case eTodButtonTextSendChoice:
		setGameButtonText( eGameButtonTruth, QObject::tr( "Choose Truth" ) );
		setGameButtonText( eGameButtonDare, QObject::tr( "Choose Dare" ) );
		break;
	case eTodButtonTextEvaluateDarePerformance:
		setGameButtonText( eGameButtonTruth, QObject::tr( "Dare Performed" ) );
		setGameButtonText( eGameButtonDare, QObject::tr( "Dare NOT Performed" ) );
		break;
	case eTodButtonTextEvaluateTruthPerformance:
		setGameButtonText( eGameButtonTruth, QObject::tr( "I Believe" ) );
		setGameButtonText( eGameButtonDare, QObject::tr( "I Do NOT Believe" ) );
		break;
	default:
		break;
	}
}

//============================================================================
void TodGameLogic::setGameButtonText( EGameButton eButton, QString strText )
{
	switch( eButton )
	{
	case eGameButtonDare:
		m_TodGameWidget->getDareButton()->setText(strText);
		break;
	case eGameButtonTruth:
		m_TodGameWidget->getTruthButton()->setText(strText);
		break;
	default:
		break;
	}
}

//============================================================================
void TodGameLogic::setGameStatus( EGameStatus eGameStatus )
{
	m_eGameStatus = eGameStatus;
	LogMsg( LOG_VERBOSE, "TodGameLogic::%s %s", __func__, DescribeGameStatus( m_eGameStatus ) );
	switch( m_eGameStatus )
	{
	case eWaitingGameStart:
		showGameButtons( false );
		setChallengeStatus( eTodEmiticonSmile, QObject::tr( "Waiting for Game Start" ) );
		break;

	case eGameEnded:
		showGameButtons( false );
		setChallengeStatus( eTodEmiticonSmile, QObject::tr( "Game Ended" ) );
		break;

		// WAITING
	case eWaitingForChoiceRx:
		showGameButtons( false );
		setChallengeStatus( eTodEmiticonThinking, QObject::tr("Wait for Truth or Dare Choice") );
		break;

	case eTxedEvaluation:
	case eWaitingForChoiceTx:
		showGameButtons( true );
		setGameButtonsText( eTodButtonTextSendChoice );
		setChallengeStatus( eTodEmiticonThinking, QObject::tr("Press Choose Truth Or Dare Button") );
		break;

		// TXED
	case eTxedChoiceDare:
		showGameButtons( false );
		setChallengeStatus( eTodEmiticonDevil, QObject::tr( "Perform Dare" ) );
		break;

	case eTxedChoiceTruth:
		showGameButtons( false );
		setChallengeStatus( eTodEmiticonAngel, QObject::tr( "Tell Truth" ) );
		break;

		// RXED
		// dare
	case eRxedDareChoice:
		m_MyApp.playSound( eSndDefUserBellMessage );
		showGameButtons( true );
		setGameButtonsText( eTodButtonTextEvaluateDarePerformance );
		setChallengeStatus( eTodEmiticonDevil, QObject::tr("Evaluate Dare") );
		break;

	case eRxedDareAccepted:
		m_MyApp.playSound( eSndDefYes );
		showGameButtons( false );
		m_MyStats.setVar( eTodGameVarIdDareAcceptedCnt, m_MyStats.getVar( eTodGameVarIdDareAcceptedCnt ) + 1 );
		setChallengeStatus( eTodEmiticonThinking, QObject::tr( "You have gained 1 Dare Point" ) );
		saveMyGameStats();
		break;

	case eRxedDareRejected:
		m_MyApp.playSound( eSndDefOfferRejected );
		showGameButtons( false );
		m_MyStats.setVar( eTodGameVarIdDareRejectedCnt, m_MyStats.getVar( eTodGameVarIdDareRejectedCnt ) + 1 );
		setChallengeStatus( eTodEmiticonThinking, QObject::tr( "Dare Perfomance was Rejected" ) );
		saveMyGameStats();
		break;

		// truth
	case eRxedTruthChoice:
		m_MyApp.playSound( eSndDefUserBellMessage );
		showGameButtons( true );
		setGameButtonsText( eTodButtonTextEvaluateTruthPerformance );
		m_HisStats.setVar( eTodGameVarIdTruthChoiceCnt, m_HisStats.getVar( eTodGameVarIdTruthChoiceCnt ) + 1 );
		setChallengeStatus( eTodEmiticonAngel, QObject::tr("Evaluate Truth") );
		break;

	case eRxedTruthAccepted:
		m_MyApp.playSound( eSndDefYes );
		showGameButtons( false );
		m_MyStats.setVar( eTodGameVarIdTruthAcceptedCnt, m_MyStats.getVar( eTodGameVarIdTruthAcceptedCnt ) + 1 );
		setChallengeStatus( eTodEmiticonThinking, QObject::tr( "You have gained 1 Truth Point" ) );
		saveMyGameStats();
		break;

	case eRxedTruthRejected:
		m_MyApp.playSound( eSndDefOfferRejected );
		showGameButtons( false );
		m_MyStats.setVar( eTodGameVarIdTruthRejectedCnt, m_MyStats.getVar( eTodGameVarIdTruthRejectedCnt ) + 1 );
		setChallengeStatus( eTodEmiticonThinking, QObject::tr( "Your Truth was not believed" ) );
		saveMyGameStats();
		break;

	default:
		break;
	}
}

//============================================================================
void TodGameLogic::slotTruthButtonClicked( void )
{
	LogMsg( LOG_VERBOSE, "TodGameLogic::%s %s", __func__, DescribeGameStatus( m_eGameStatus ) );
	switch( m_eGameStatus )
	{
	case eWaitingForChoiceRx:
		break;

	case eRxedDareChoice:
		m_HisStats.setVar( eTodGameVarIdDareAcceptedCnt, m_HisStats.getVar( eTodGameVarIdDareAcceptedCnt ) + 1 );
		sendGameAction( eTodGameActionDareAccepted );
		setGameStatus( eWaitingForChoiceTx );
		break;

	case eRxedTruthChoice:
		m_HisStats.setVar( eTodGameVarIdTruthAcceptedCnt, m_HisStats.getVar( eTodGameVarIdTruthAcceptedCnt ) + 1 );
		sendGameAction( eTodGameActionTruthAccepted );
		setGameStatus( eWaitingForChoiceTx );
		break;

	case eWaitingForChoiceTx:
		sendGameAction( eTodGameActionChoiceTruth );
		setGameStatus( eTxedChoiceTruth );
		break;

	default:
		LogMsg( LOG_ERROR, "odGameLogic::%s unhandled status %s", __func__, DescribeGameStatus( m_eGameStatus ) );
		break;
	};
}

//============================================================================
void TodGameLogic::slotDareButtonClicked( void )
{
	LogMsg( LOG_VERBOSE, "TodGameLogic::%s %s", __func__, DescribeGameStatus( m_eGameStatus ) );
	switch( m_eGameStatus )
	{
	case eWaitingForChoiceRx:
		break;

	case eRxedDareChoice:
		m_HisStats.setVar( eTodGameVarIdDareRejectedCnt, m_HisStats.getVar( eTodGameVarIdDareRejectedCnt ) + 1 );
		sendGameAction( eTodGameActionDareRejected );
		setGameStatus( eWaitingForChoiceTx );
		break;

	case eRxedTruthChoice:
		m_HisStats.setVar( eTodGameVarIdTruthRejectedCnt, m_HisStats.getVar( eTodGameVarIdTruthRejectedCnt ) + 1 );
		sendGameAction( eTodGameActionTruthRejected );
		setGameStatus( eWaitingForChoiceTx );
		break;


	case eWaitingForChoiceTx:
		sendGameAction( eTodGameActionChoiceDare );
		setGameStatus( eTxedChoiceDare );
		break;

	default:
		LogMsg( LOG_ERROR, "odGameLogic::%s unhandled status %s", __func__, DescribeGameStatus( m_eGameStatus ) );
		break;
	};
}

//============================================================================
void TodGameLogic::handleUserWentOffline( void )
{
	emit signalUserWentOffline();
}

//============================================================================
void TodGameLogic::beginGame( bool isChallenger )
{
	m_IsChallenger = isChallenger;
    setGameStatus( m_IsChallenger ? eWaitingForChoiceTx : eWaitingForChoiceRx );
	if( !m_CallbacksRequested )
	{
		m_CallbacksRequested = true;
		m_MyApp.getTodGameMgr().wantTodGamCallbacks( this, true );
	}
}

//============================================================================
void TodGameLogic::endGame( void )
{
	if( m_CallbacksRequested )
	{
		m_CallbacksRequested = false;
		m_MyApp.getTodGameMgr().wantTodGamCallbacks( this, false );
	}

	saveMyGameStats();
    setGameStatus( eGameEnded );
}

//============================================================================
void TodGameLogic::toGuiTodGameAction( EPluginType pluginType, VxGUID& onlineId, ETodGameAction todGameAction )
{
	if( pluginType != m_PluginType ||
		!m_HisIdent ||
		!( onlineId == m_HisIdent->getMyOnlineId() ) )
	{
		LogMsg( LOG_VERBOSE, "TodGameLogic::%s not for us", __func__ );
		return;
	}

	LogMsg( LOG_VERBOSE, "TodGameLogic::%s plugin %s action %s", __func__, DescribePluginType( pluginType ), DescribeTodGameAction( todGameAction ) );
	switch( todGameAction )
	{
	case eTodGameActionChoiceDare:
		setGameStatus( eRxedDareChoice );
		break;

	case eTodGameActionDareAccepted:
		setGameStatus( eRxedDareAccepted );
		break;

	case eTodGameActionDareRejected:
		setGameStatus( eRxedDareRejected );
		break;

	case eTodGameActionChoiceTruth:
		setGameStatus( eRxedTruthChoice );
		break;

	case eTodGameActionTruthAccepted:
		setGameStatus( eRxedTruthAccepted );
		break;

	case eTodGameActionTruthRejected:
		setGameStatus( eRxedTruthRejected );
		break;

	default:
		break;
	}
}

//============================================================================
bool TodGameLogic::sendGameAction( ETodGameAction todGameAction )
{
	IFromGui& poFromGui = m_Engine.getFromGuiInterface();
	bool bResult = poFromGui.fromGuiTodGameActionSend( m_PluginType, m_HisIdent->getMyOnlineId(), todGameAction );
	if( false == bResult )
	{
		handleUserWentOffline();
	}

	return bResult;
}
