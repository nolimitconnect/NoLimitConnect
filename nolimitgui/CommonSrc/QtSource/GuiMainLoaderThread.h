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

class GuiMainLoaderThread : public QThread
{
    Q_OBJECT

public:
    GuiMainLoaderThread(QObject *parent = nullptr);
    ~GuiMainLoaderThread();

    void                setIsLoadComplete( bool isComplete )    { m_LoadComplete = isComplete; }
    bool                getIsLoadComplete( void )               { return m_LoadComplete; }

protected:
    void run() override;

private:
    QElapsedTimer       m_ElapsedTimer;
    bool                m_LoadComplete{ false };
};