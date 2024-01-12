//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityChooseTestWebsiteUrl.h"
#include "AppCommon.h"

#include <P2PEngine/EngineSettings.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>

#include <QMessageBox>

//============================================================================
ActivityChooseTestWebsiteUrl::ActivityChooseTestWebsiteUrl(	AppCommon& app, QWidget* parent )
: ActivityBase(  OBJNAME_ACTIVITY_CHOOSE_TEST_WEBSITE_URL, app, parent )
, m_MyApp( app )
, m_Engine( app.getEngine() )
{
	ui.setupUi(this);
    connectBarWidgets();

	connect( ui.m_TitleBarWidget, SIGNAL(signalBackButtonClicked()), this, SLOT(reject()) );
	connect( ui.CancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
	connect( ui.OkButton, SIGNAL(clicked()), this, SLOT(applyResultsAndExit()) );

	updateValues();
}

//============================================================================
void ActivityChooseTestWebsiteUrl::updateValues()
{
	std::string strWebsiteUrl;
	m_Engine.getEngineSettings().getConnectTestUrl(strWebsiteUrl);
	QString strUrl = strWebsiteUrl.c_str();
	ui.TestWebsiteEdit->setText(strUrl);
}

//============================================================================
void ActivityChooseTestWebsiteUrl::applyResultsAndExit()
{
	QString strResult = ui.TestWebsiteEdit->text();
	std::string strWebsiteUrl = strResult.toUtf8().constData();
	m_Engine.getEngineSettings().setConnectTestUrl(strWebsiteUrl);
	accept();
}

