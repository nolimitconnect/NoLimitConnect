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

#include "ui_AppletHackerList.h"

#include <QMutex>
#include <QFile>

#include <CoreLib/VxDebug.h>

class VxHackerRecord;

class AppletHackerList : public AppletBase
{
    Q_OBJECT
public:
    AppletHackerList( AppCommon& app, QWidget* parent );
    virtual ~AppletHackerList();

signals:
    void                        signalHackerListMsg( const QString& logMsg );

protected slots:
    void                        slotCopyToClipboardClicked( void );

protected:
    void                        refreshHackerList( void );
    void                        addHacker( VxHackerRecord& hackerRec );

    Ui::AppletHackerListUi      ui;
};


