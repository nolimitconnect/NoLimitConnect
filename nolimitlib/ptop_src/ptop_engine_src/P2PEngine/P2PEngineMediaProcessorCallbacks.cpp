//============================================================================
// Copyright (C) 2015 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <P2PEngine/P2PEngine.h>
#include <GuiInterface/IToGui.h>

//============================================================================
void P2PEngine::callbackVideoJpgBig( void * /*userData*/, VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen )
{
	IToGui::getToGui().toGuiPlayVideoFrame( vidFeedId, jpgData, jpgDataLen, 0 );
}

//============================================================================
void P2PEngine::callbackVideoJpgSmall( void * /*userData*/, VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen, int motion0to100000 )
{
	IToGui::getToGui().toGuiPlayVideoFrame( vidFeedId, jpgData, jpgDataLen, motion0to100000 );
}
