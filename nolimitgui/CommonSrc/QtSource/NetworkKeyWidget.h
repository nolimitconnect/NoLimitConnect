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

#include <QWidget>

#include <GuiInterface/IDefs.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class NetworkKeyWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;

class NetworkKeyWidget : public QWidget
{
	Q_OBJECT

public:
    NetworkKeyWidget( QWidget* parent = nullptr );

    QString						getNetworkKey( void );

protected slots:
    void                        slotShowNetworkKeyInformation( void );
    void                        slotApplyNetworkKey( void );

protected:
	void						initNetworkKeyWidget( void );
    void						fillNetworkKeyEditField( void );

    void						updateNetworkKeyIcon( void );
    void                        updateUi( void );
    bool                        verifyNetworkKey( QString& keyVal );

    AppCommon&                  m_MyApp;
    EPluginType                 m_PluginType = ePluginTypeInvalid;
	Ui::NetworkKeyWidgetUi&	    ui;
};
