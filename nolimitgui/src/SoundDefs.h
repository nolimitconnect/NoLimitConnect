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


enum ESndDef
{
	eSndDefNone,
	eSndDefIgnore, // also no joshua sound
	eSndDefCancel,
	eSndDefAlarmPleasant,
	eSndDefAlarmAnoying,
	eSndDefButtonClick,
	eSndDefChoice1,
	eSndDefChoice2,
	eSndDefSending,
	eSndDefNotify1,
	eSndDefNotify2,
	eSndDefPaperShredder,
	eSndDefPhoneRing1,
	eSndDefReject,
	eSndDefShare,
	eSndDefByeBye,
	eSndDefMessageArrived,
	eSndDefOfferAccepted,
	eSndDefOfferRejected,
	eSndDefCameraClick,
	eSndDefBusy,
	eSndDefOfferStillWaiting,
	eSndDefFileXferComplete,		// 22
	eSndDefUserBellMessage,         // 23
	eSndDefNeckSnap,				// 24
	eSndDefYes,						// 25

	eSndDefAppShutdown,				// 26

	eMaxSndDef
};

const char* DescribeSnd( enum ESndDef sndDef );