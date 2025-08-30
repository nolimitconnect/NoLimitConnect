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

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "TodGameDefs.h"
#include "TodGameMgr.h"

enum ETodButtonText
{
	eTodButtonTextSendChoice,
	eTodButtonTextEvaluateTruthPerformance,
	eTodButtonTextEvaluateDarePerformance,

	eMaxTodButtonText
};

enum ETodEmoticon
{
	eTodEmiticonSmile,
	eTodEmoticonThumbsUp,
	eTodEmoticonThumbsDown,
	eTodEmiticonAngel,
	eTodEmiticonDevil,
	eTodEmiticonThinking,

	eMaxTodEmoticon
};

class AppCommon;
class EngineSettings;
class GuiUser;
class P2PEngine;
class QFrame;
class QLabel;
class TodStatsWidget;
class TodGameWidget;
class VxPushButton;
class VidWidget;

class TodGameLogic : public QWidget, public TodGameStats, public TodGameCallback
{
	Q_OBJECT

public:
	TodGameLogic( AppCommon& myApp, P2PEngine& engine, EPluginType pluginType, QWidget* parent );
	~TodGameLogic();

	void						setGuiWidgets( GuiUser*	hisIdent, TodGameWidget* todGameWidget, TodStatsWidget* myStatsWidget, TodStatsWidget* hisStatsWidget );

	void						beginGame( bool isChallenger );
	void						endGame( void );

signals:
	void						signalUserWentOffline( void );
	void						signalGameStatus( QString statusText );


protected slots:
	void						slotTruthButtonClicked( void );
	void						slotDareButtonClicked( void );

protected:
	void						toGuiTodGameAction( EPluginType	pluginType, VxGUID& onlineId, ETodGameAction todGameAction ) override;

	void						setChallengeStatus( ETodEmoticon emoticon, QString statusText );

	void						setGameStatus( EGameStatus eGameStatus );

	bool						sendGameAction( ETodGameAction todGameAction );

	void						showGameButtons( bool show );
	void						setGameButtonsText( ETodButtonText todButtonText );
	void						setGameButtonText( EGameButton eButton, QString strText );

	void						saveMyGameStats( void );

	void						handleUserWentOffline( void );

	void						setEmoticonThumbnail( ETodEmoticon emoticon );

	AppCommon& 				    m_MyApp;
	P2PEngine&					m_Engine; 
	EPluginType					m_PluginType{ ePluginTypeInvalid };

	EngineSettings&				m_MySettings;
	GuiUser*					m_HisIdent{ nullptr };
	TodGameWidget *				m_TodGameWidget{ nullptr };
	EGameStatus					m_eGameStatus{ eWaitingGameStart };
	bool						m_IsChallenger{ false };
	bool						m_CallbacksRequested{ false };
};
