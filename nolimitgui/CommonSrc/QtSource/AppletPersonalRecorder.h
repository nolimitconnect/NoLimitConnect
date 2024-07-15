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

#include "AppletBase.h"
#include "InputClientBaseCallback.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class PersonalRecorderUi;
}
QT_END_NAMESPACE

class AppletPersonalRecorder : public AppletBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
	AppletPersonalRecorder( AppCommon& app, QWidget* parent );
	virtual ~AppletPersonalRecorder();
    
	AppCommon&					getMyApp( void ) override									{ return m_MyApp; }
    EPluginType					getInputClientPluginType( void ) override					{ return getPluginType(); }

    void                        callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );

protected:
    void						setupMultiSessionActivity( void );
    void                        showEvent( QShowEvent* showEvent ) override;

    Ui::PersonalRecorderUi&	    ui;
    bool                        m_HistoryQueried{ false };
};


