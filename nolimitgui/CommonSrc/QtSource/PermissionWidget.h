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

#include "ui_PermissionWidget.h"

class AppCommon;
class AppletBase;

class PermissionWidget : public QWidget
{
	Q_OBJECT

public:
    PermissionWidget( QWidget* parent = nullptr );

    void						setPluginType( EPluginType pluginType, int subType = 0 );
    EPluginType					getPluginType( void )           { return m_PluginType; }

    void						setPermissionLevel( EFriendState permLevel );
    EFriendState                getPermissionLevel( void );
    VxPushButton *              getPluginRunButton()            { return ui.m_PluginRunButton; }
    VxPushButton *              getPluginSettingsButton()       { return ui.m_PluginSettingsButton; }

protected slots:
    void                        slotHandleSelectionChanged( int );
    void                        slotShowPermissionInformation();
    void                        slotShowPluginInformation();
    void                        slotRunPlugin();
    void                        slotSetupPlugin();

protected:
	void						initPermissionWidget( void );
    void						fillPermissionComboBox( void );

    void						updatePermissionIcon( void );
    void                        updateUi( void );

    Ui::PermissionWidgetUi	    ui;
    AppCommon&                  m_MyApp;
    AppletBase*                 m_ParentApplet{ nullptr };
    EPluginType                 m_PluginType = ePluginTypeInvalid;
    int                         m_SubPluginType = 0;
    bool                        m_OrigPermissionIsSet = false;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
    bool                        m_PermissionsConnected = false;
};
