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

class AppCommon;

class GuiThreadAppLoader : public QThread
{
    Q_OBJECT

public:
    GuiThreadAppLoader( AppCommon& myApp, QObject *parent = nullptr);
    ~GuiThreadAppLoader() = default;


    void                setIsAccountMgrLoaded( bool isLoaded )   { m_AccountMgrLoaded = isLoaded; }
    bool                getIsAccountMgrLoaded( void )            { return m_AccountMgrLoaded; }

    void                setIsIconsLoaded( bool isLoaded )        { m_IconsLoaded = isLoaded; }
    bool                getIsIconsLoaded( void )                 { return m_IconsLoaded; }

    void                setIsLoadComplete( bool isComplete )     { m_LoadComplete = isComplete; }
    bool                getIsLoadComplete( void )                { return m_LoadComplete; }

protected:
    void run() override;

private:
    AppCommon&          m_MyApp;

    bool                m_SettingsLoaded{ false };
    bool                m_AccountMgrLoaded{ false };
    bool                m_IconsLoaded{ false };

    bool                m_LoadComplete{ false };
};