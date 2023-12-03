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

class ToGuiHardwareControlInterface
{
public:
    virtual void 				callbackToGuiShutdownHardware( void ){};

    virtual void 				callbackToGuiWantMicrophoneRecording( bool wantMicInput ){};
    virtual void				callbackToGuiMicrophoneMuted( bool isMuted ) {};

    virtual void 				callbackToGuiWantSpeakerOutput( bool wantSpeakerOutput ){};
    virtual void 				callbackToGuiSpeakerMuted( bool isMuted ) {};

    virtual void				callbackToGuiWantVideoCapture( bool wantVideoCapture ){};

    
};

