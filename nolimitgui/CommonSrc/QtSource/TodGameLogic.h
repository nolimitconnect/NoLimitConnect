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
#include <PktLib/VxCommon.h>

class AppCommon;
class EngineSettings;
class GuiUser;
class P2PEngine;
class QFrame;
class QLabel;
class TodGameWidget;
class VxPushButton;
class VidWidget;

class TodGameLogic : public QWidget
{
	Q_OBJECT

public:
	TodGameLogic( AppCommon& myApp, P2PEngine& engine, EPluginType pluginType, QWidget* parent );

	void						setGuiWidgets(	GuiUser*	hisIdent,
												TodGameWidget *	todGameWidget );
	virtual void				setVisible( bool visible );

	bool						beginGame( bool isChallenger );
	void						endGame( void );

	void						setGameValueVar( long s32VarId, long s32VarValue );
	void						setGameActionVar( long s32VarId, long s32VarValue );

	void						showChallengeStatus( bool showChallenge );
	void						setChallengeStatus( QString statusIcon, QString statusText );

	void						setGameStatus( EGameStatus eGameStatus );

signals:
	void						signalUserWentOffline( void );

protected slots:
	void						slotToGuiSetGameValueVar( long s32VarId, long s32VarValue );
	void						slotToGuiSetGameActionVar( long s32ActionId, long s32VarValue );

	void						slotTruthButtonClicked( void );
	void						slotDareButtonClicked( void );

protected:
	bool						sendGameStats( void );
	void						enableGameButton( EGameButton eButton, bool bEnable );
	void						setGameButtonText( EGameButton eButton, QString strText );

	void						loadMyGameStats( void );
	void						saveMyGameStats( void );
	void						updateMyStats( void );
	void						updateFriendStats( void );
	void						setTodStatusMsg( QString strStatus );
	bool						fromGuiSetGameValueVar(	int32_t	s32VarId, int32_t s32VarValue );
	bool						fromGuiSetGameActionVar(int32_t	s32VarId, int32_t s32VarValue );
	void						handleUserWentOffline( void );

	AppCommon& 				    m_MyApp;
	P2PEngine&					m_Engine; 
	EPluginType					m_ePluginType;
	EngineSettings&				m_MySettings;
    GuiUser*				    m_HisIdent;
	TodGameWidget *				m_TodGameWidget;
	TodGameStats				m_TodGameStats;
	EGameStatus					m_eGameStatus;
	bool						m_IsChallenger;
};
