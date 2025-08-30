#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "GuiPluginMgrCallback.h"

#include <QObject>

class AppCommon;

class GuiPluginMgr : public QObject
{
    Q_OBJECT
public:
    GuiPluginMgr();
    GuiPluginMgr( const GuiPluginMgr& rhs ) = delete;
    virtual ~GuiPluginMgr() = default;

    void						setPluginVisible( EPluginType pluginType, bool isVisible );
    bool						getIsPluginVisible( EPluginType pluginType );

    void						setIsCamServerEnabled( bool camServerIsEnabled )        { m_CamServerIsEnabled = camServerIsEnabled; }
    int						    getIsCamServerEnabled( void )                           { return m_CamServerIsEnabled; }
    void						setCamServerClientCount( int camServerClientCount )     { m_CamServerClientCount = camServerClientCount; }
    int						    getCamServerClientCount( void )                         { return m_CamServerClientCount; }

    void                        toGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue );

    void                        wantPluginMgrCallbacks( GuiPluginMgrCallback* client, bool enable );

signals:
    void						signalInternalToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue );

protected slots:
    void						slotInternalToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue );

protected:
    std::vector<GuiPluginMgrCallback*> m_CallbackClients;
    QVector<EPluginType>		m_VisiblePluginsList;

    bool                        m_CamServerIsEnabled{ false };
    int                         m_CamServerClientCount{ -1 };
};
