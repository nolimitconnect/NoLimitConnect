#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_AppletPersonalRecorder.h"

#include "AppletBase.h"

class AppletPersonalRecorder : public AppletBase
{
	Q_OBJECT
public:
	AppletPersonalRecorder( AppCommon& app, QWidget* parent );
	virtual ~AppletPersonalRecorder();

    void                        callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );

protected:
    void						setupMultiSessionActivity( void );
    void                        showEvent( QShowEvent* showEvent ) override;

    Ui::PersonalRecorderUi	    ui;
    bool                        m_HistoryQueried{ false };
};


