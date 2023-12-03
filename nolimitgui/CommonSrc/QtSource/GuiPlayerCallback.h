#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <inttypes.h>

class VxGUID;
class QImage;

class GuiPlayerCallback
{
public:
	virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 ) = 0;
	virtual void				callbackGuiPlayVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame ) {};
	virtual void				callbackGuiMediaPlayerNlcReady( bool isReady ) {};
};

