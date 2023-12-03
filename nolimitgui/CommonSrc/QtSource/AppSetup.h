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

#include "AppDefs.h"
#include <QObject>

class AppSetupWorker : public QObject 
{
    Q_OBJECT
public:
    AppSetupWorker() = default;
    ~AppSetupWorker() = default;

public slots:
    void process();

signals:
    void finished();
    void error( QString err );
    void signalSetupResult( int rc, qint64 bytesCopied );

private:
    // add your variables here
};


class AppSetup : public QObject
{
    Q_OBJECT
public:
    AppSetup() = default;
    ~AppSetup() = default;

    const char* SETUP_COMPLETE_FILE = "setup_completed.txt";

    bool areUserAssetsInitilized();
    void initializeUserAssets();

signals:
    void signalSetupResult( QObject * appSetupInst, int rc, qint64 bytesCopied );

protected slots:
    void slotError( QString err );
    void slotSetupResult( int rc, qint64 bytesCopied );

protected:
    std::string getAssetsInitilizedFileName();
};