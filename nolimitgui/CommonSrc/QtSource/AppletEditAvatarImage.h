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
#include "UserProfile.h"

#include "ui_AppletEditAvatarImage.h"

class AssetMgr;

class AppletEditAvatarImage : public AppletBase
{
	Q_OBJECT
public:
    AppletEditAvatarImage( AppCommon& app, QWidget* parent );
	virtual ~AppletEditAvatarImage() = default;

signals:
    void                        signalAvatarImageChanged( ThumbInfo* avatarThumb );
    void                        signalAvatarImageRemoved( void );

public slots:
    void						onApplyButClick( void );
    void						onRemoveButClick( void );

protected:

    //=== constants ===//
    static const int SELECT_IMAGE = 0; // selector for image gallery call
    static const int CAMERA_SNAPSHOT = 1; // selector for image gallery call

    //=== vars ===//
    Ui::AppletEditAvatarImageUi	ui;
    ThumbMgr&                   m_ThumbMgr;
    UserProfile 				m_UserProfile;
    VxNetIdent*				m_MyIdent = nullptr;
    QString                     m_strOrigOnlineName;
    QString                     m_strOrigMoodMessage;
};


