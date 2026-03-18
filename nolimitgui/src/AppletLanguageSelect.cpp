//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLanguageSelect.h"

#include "ActivityMsgBoxYesNo.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "AppSettings.h"
#include "GuiHelpers.h"

#include <P2PEngine/EngineSettings.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

#include "ui_AppletLanguageSelect.h"

//============================================================================
AppletLanguageSelect::AppletLanguageSelect( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_LANGUAGE_SELECT, app, parent )
, ui(*(new Ui::LanguageSelectUi ))
{
    setAppletType( eAppletLanguageSelect );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    GuiHelpers::fillLanguage( ui.m_LanguageComboBox );

    connect( ui.m_ApplyButton, SIGNAL(clicked()), this, SLOT(slotApplyButtonClicked() ) );
}

//============================================================================
void AppletLanguageSelect::slotApplyButtonClicked( void )
{
    qDebug() << "Language selected: " << ui.m_LanguageComboBox->currentText();
    onLanguageSelected();
}

//============================================================================
void AppletLanguageSelect::onLanguageSelected( void )
{

}
