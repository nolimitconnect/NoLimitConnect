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

#include "AppletBase.h"

#include <QMutex>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletSocketListUi;
}
QT_END_NAMESPACE

class VxSktStatRecord;

class AppletSocketList : public AppletBase
{
    Q_OBJECT
public:
    AppletSocketList( AppCommon& app, QWidget* parent );
    virtual ~AppletSocketList();

    void                        logMsg( const char* logMsg, ... );

signals:
    void                        signalSocketListMsg( const QString& logMsg );

protected slots:
    void                        slotCopyToClipboardClicked( void );

protected:
    void                        refreshSktList( void );
    void                        addSocketStat( VxSktStatRecord& sktStat );

    Ui::AppletSocketListUi&     ui;
};


