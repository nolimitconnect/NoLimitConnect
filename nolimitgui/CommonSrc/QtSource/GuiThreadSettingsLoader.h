#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QThread>

class AppSettings;

class GuiThreadSettingsLoader : public QThread
{
    Q_OBJECT

public:
    GuiThreadSettingsLoader( AppSettings& appSettings, QObject* parent = nullptr )
    : QThread(parent)
    , m_AppSettings( appSettings )
    {
    }

    ~GuiThreadSettingsLoader() = default;

    void                setIsRootStorageSet( bool isComplete )    { m_RootStorageIsSet = isComplete; }
    bool                getIsRootStorageSet( void )               { return m_RootStorageIsSet; }

    void                setIsSettingsLoaded( bool isLoaded )     { m_SettingsLoaded = isLoaded; }
    bool                getIsSettingsLoaded( void )              { return m_SettingsLoaded; }

protected:
    void run() override;

    void                setupRootStorageDirectory( void );

    AppSettings&        m_AppSettings;
    bool                m_RootStorageIsSet{ false };
    bool                m_SettingsLoaded{ false };
};