#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "GuiWebPageCallback.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletAboutMeClientUi;
}
QT_END_NAMESPACE

class AppletAboutMeClient : public AppletBase, public GuiWebPageCallback
{
	Q_OBJECT
public:
	AppletAboutMeClient( AppCommon& app, QWidget* parent );
	virtual ~AppletAboutMeClient();

    void                        setIdentity( GuiUser* guiUser );

protected:
    void                        loadRichTextFile( QString fileName );
    virtual void				toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, QString& paramValue ) override;

    //=== vars ===//
    Ui::AppletAboutMeClientUi&	ui;
    VxNetIdent*				    m_MyIdent = nullptr;
    QString                     m_strOrigOnlineName;
    QString                     m_strOrigMoodMessage;

    std::string					m_strDefaultPicPath;
    std::string					m_strUserSepecificDataDir;
    bool						m_bUserPickedImage = false;
    bool						m_bUsingDefaultImage = true;
    bool 					    m_CameraSourceAvail{ false };
    VxGUID                      m_HisOnlineId;
};


