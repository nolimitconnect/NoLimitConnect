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

#include "ActivityBase.h"
#include "AppSetup.h"

#include <GuiInterface/IDefs.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ActivityAppSetupUi;
}
QT_END_NAMESPACE

class ActivityAppSetup : public ActivityBase
{
	Q_OBJECT

public:

    ActivityAppSetup( AppCommon& app, QWidget* parent );

	virtual ~ActivityAppSetup() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    TitleBarWidget*             getTitleBarWidget( void ) override;
    BottomBarWidget*            getBottomBarWidget( void ) override;

    bool                        isSetupCompleted( void ) { return m_SetupCompleted; }


protected slots:
	void						slotCopyToClipboardButtonClicked( void );
    void						slotCopyResult( QObject * workerObj, int rc, qint64 bytesCopied );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;

    void						initActivityAppSetup( void );
	//=== vars ===//
	Ui::ActivityAppSetupUi&		ui;
    AppSetup *                  m_AppSetup = nullptr;
    bool                        m_SetupCompleted = false;
};
