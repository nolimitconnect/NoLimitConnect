#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "AppDefs.h"

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxGUID.h>

class ActivityBase;
class AppCommon;
class HomeWindow;
class QFrame;
class RenderGlWidget;
class AppletLaunchWidget;

class AppletMgr : public QWidget
{
	Q_OBJECT
public:
	AppletMgr( AppCommon& myMpp, QWidget* parent );
	virtual ~AppletMgr(){}

    QFrame*						getAppletFrame( EApplet applet );
    ActivityBase*				launchApplet( EApplet applet, QWidget* parent = nullptr, QString launchParam = "", VxGUID assetId = VxGUID::nullVxGUID());
	bool						isAppletLaunched( EApplet applet );
	void						activityStateChange( ActivityBase* activity, bool isCreated );

	ActivityBase*				findAppletDialog( EApplet applet );

    RenderGlWidget *            getRenderConsumer( void );

	QFrame*                     getLaunchParentFrame( ELaunchFrame launchFrame );

protected:
	void						addApplet( ActivityBase* activity );
	void						removeApplet( EApplet applet );
    void						removeApplet( ActivityBase* activity );

	void						bringAppletToFront( ActivityBase* appletDialog );
    ActivityBase*				findAppletDialog( ActivityBase* activity );

    void                        makeMessengerFullSized( void );

	QWidget*					getActiveWindow( void );

	bool						viewMyServerAllowed( EApplet applet );
	bool						launchAppletAllowed( EApplet applet );
	bool						isServiceEnabled( EPluginType pluginType );

	AppCommon&					m_MyApp;
    QVector<AppletLaunchWidget *>		m_AppletList;
	QVector<ActivityBase*>		m_ActivityList;
};

