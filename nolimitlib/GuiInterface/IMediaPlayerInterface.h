#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <string>

class AssetBaseInfo;

class IMediaPlayerRequests
{
public:
    virtual void				fromGuiInitCommandLine( int argc, char** arg ) = 0;

    virtual bool				fromStartModule( EAppModule appModule ) = 0;
    virtual bool				fromStopModule( EAppModule appModule ) = 0;
    virtual bool				fromGuiIsModuleRunning( EAppModule appModule ) = 0;

    virtual bool				fromGuiPlayMedia( AssetBaseInfo& assetInfo, int pos0to100000 ) = 0;
    virtual bool				fromGuiMediaPlayerAction( EMediaPlayerAction playerAction ) = 0;
    virtual bool				fromGuiMediaPlayerSeek( int position0to100000 ) = 0;

    virtual bool				fromGuiPlayStream( std::string url, int pos0to100000 ) = 0;
};

class VxGUID;

class IMediaPlayerCallback
{
public:
    virtual void				fromMediaPlayerInitLevel( int initLevel, bool success ) = 0;

    virtual void				fromMediaPlayerPlayFile( VxGUID& feedId ) = 0;
    virtual void				fromMediaPlayerPlayStarted( VxGUID& feedId ) = 0;

    virtual void				fromMediaPlayerStopPlaying( VxGUID& feedId ) = 0;
    virtual void				fromMediaPlayerPlaybackStopped( VxGUID& feedId ) = 0;
    virtual void				fromMediaPlayerPlaybackEnded( VxGUID& feedId ) = 0;

};
