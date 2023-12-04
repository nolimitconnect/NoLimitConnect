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

#include "ui_PermissionListItemWidget.h"

#include <QListWidgetItem>

class AppletBase;

class PermissionListItem : public QWidget, public QListWidgetItem
{
	Q_OBJECT

public:
    PermissionListItem( QListWidget * parent, AppletBase *parentApplet );

    void						initPermissionListItem( EPluginType pluginType, int subType = 0 );

    void						setPermissionLevel( EFriendState permLevel );
    EFriendState                getPermissionLevel( void );

protected slots:
    void                        slotHandleSelectionChanged( int );
    void                        slotShowPermissionInformation();
    void                        slotShowPluginInformation();
    void                        slotRunPlugin();
    void                        slotSetupPlugin();

protected:
    void						fillPermissionComboBox( void );
    void						fillPluginInfo( void );

    void						updatePermissionIcon( void );
    void                        updateUi( void );

    Ui::PermissionListItemUi	ui;
    AppCommon&                  m_MyApp;
    AppletBase *                m_ParentApplet{ nullptr };
    QSize                       m_SizeHint;
    EPluginType                 m_PluginType = ePluginTypeInvalid;
    int                         m_SubPluginType = 0;
    bool                        m_OrigPermissionIsSet = false;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
    bool                        m_PermissionsConnected = false;
};
