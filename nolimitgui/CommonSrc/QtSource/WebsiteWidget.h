#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFrame>

#include "AppDefs.h"

enum WebsiteUrlType
{
	eWebsiteUrlNlc,
	eWebsiteUrlVpn
};

QT_BEGIN_NAMESPACE
namespace Ui {
    class WebsiteWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;

class WebsiteWidget : public QFrame
{
	Q_OBJECT
public:
	WebsiteWidget( QWidget* parent = nullptr );
	virtual ~WebsiteWidget() {};

	void							setUrlType( WebsiteUrlType urlType );

protected slots:
	void							slotGoToWebsite( void );

protected:
	AppCommon&						m_MyApp;
	Ui::WebsiteWidgetUi&			ui;
	QString							m_WebsiteUrl;
};

