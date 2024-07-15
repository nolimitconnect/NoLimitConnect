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
    class InformationWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class ActivityInformation;

class InformationWidget : public QWidget
{
	Q_OBJECT

public:
    InformationWidget( QWidget* parent = nullptr );

    void						setPluginType( EPluginType pluginType ) { m_PluginType = pluginType; updateInformation(); }
    EPluginType                 getPluginType() { return m_PluginType;  }

protected slots:
    void                        slotShowInformation();

protected:
	void						initInformationWidget( void );
    void						updateInformation( void );

    Ui::InformationWidgetUi&	ui;
    AppCommon&                  m_MyApp;
    ActivityInformation *       m_ActivityInfo = nullptr;
    EPluginType                 m_PluginType = ePluginTypeInvalid;
};
