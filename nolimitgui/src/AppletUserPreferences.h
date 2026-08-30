#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "AppDefs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletUserPreferencesUi;
}
QT_END_NAMESPACE

class QComboBox;
class QLabel;

class AppletUserPreferences : public AppletBase
{
	Q_OBJECT
public:
    AppletUserPreferences( AppCommon& app, QWidget* parent );
	virtual ~AppletUserPreferences();

protected slots:
	void						slotSaveSettings( void );
    void                        slotCancel( void );
	void                        slotMaxMsgHistoryInfoButtonClicked( void );
	void                        slotMaxMsgHistoryValueChanged( int maxHistory );

protected:
	virtual void				showEvent( QShowEvent* ev ) override;
	virtual void				hideEvent( QHideEvent* ev ) override;

    void						updateDlgFromSettings( void );
    void						updateSettingsFromDlg( void );

	Ui::AppletUserPreferencesUi&	ui;

    //! not part of AppletUserPreferences.ui -- added programmatically ( that .ui is shared/
    //! widely used, deliberately not hand-edited -- see event-calendar design notes ). backed
    //! directly by IFromGui::fromGuiSetPurgeEventHistoryType/fromGuiGetPurgeEventHistoryType
    //! ( engine-side-only setting, not AppSettings -- see EngineSettings::
    //! setPurgeEventHistoryType ), unlike the other .ui-declared controls on this screen which
    //! go through AppSettings.
    QLabel*                     m_PurgeEventHistoryLabel{ nullptr };
    QComboBox*                  m_PurgeEventHistoryCombo{ nullptr };
};


