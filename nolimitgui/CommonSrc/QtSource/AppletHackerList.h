#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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


