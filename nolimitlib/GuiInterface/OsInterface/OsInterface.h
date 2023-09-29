#pragma once

#include "GuiInterface/IDefs.h"
#include "GuiInterface/INlcEvents.h"

#include <string>

class INlc;
class CAppParamParser;

class OsInterface
{
public:
    OsInterface() = default;
    virtual ~OsInterface() = default;

    // exit of application error code
    virtual void                setRunResultCode( int exitCode )    { m_RunResultCode = exitCode; }
    virtual int                 getRunResultCode( void )            { return m_RunResultCode; }

    //=== stages of create ===//
    virtual bool                doPreStartup( void ) = 0;
    virtual bool                doStartup( void ) = 0;

    //=== stages of run ===//
    virtual bool                initRun( const CAppParamParser& cmdLineParams );
    virtual bool                doRun( EAppModule appModule );

    //=== stages of destroy ===//
    virtual void                doPreShutdown( void ) = 0;
    virtual void                doShutdown( void ) = 0;

    //=== utilities ===//
    virtual bool               initUserPaths( std::string& appCachePath, std::string& userWriteablePath ); // basically exe and user data paths
    virtual bool               initDirectories( void );

protected:
    const CAppParamParser*      m_CmdLineParams = nullptr;
    int                         m_RunResultCode = 0;
};
