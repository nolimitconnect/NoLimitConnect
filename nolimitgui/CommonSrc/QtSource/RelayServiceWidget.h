#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"

#include "ui_RelayServiceWidget.h"

class RelayServiceWidget : public QWidget
{
	Q_OBJECT

public:
    RelayServiceWidget( QWidget* parent = nullptr );

    void						setPluginType( EPluginType pluginType, int subType = 0 );
    void						setPermissionLevel( EFriendState permLevel );
    EFriendState                getPermissionLevel( void );

protected slots:
    void                        slotHandleSelectionChanged( int );
    void                        slotShowPermissionInformation();
    void                        slotShowPluginInformation();

protected:
	void						initRelayServiceWidget( void );
    void						fillPermissionComboBox( void );

    void						updatePermissionIcon( void );
    void                        updateUi( void );

    Ui::RelayServiceWidgetUi	    ui;
    AppCommon&                  m_MyApp;
    EPluginType                 m_PluginType = ePluginTypeInvalid;
    int                         m_SubPluginType = 0;
    bool                        m_OrigPermissionIsSet = false;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
    bool                        m_PermissionsConnected = false;
};
