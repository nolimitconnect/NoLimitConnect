//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityAppSetup.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QClipboard>

#include "ui_ActivityAppSetup.h"

TitleBarWidget*	    ActivityAppSetup::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget*	ActivityAppSetup::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityAppSetup::ActivityAppSetup( AppCommon& app, QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_APP_SETUP, app, parent, eActivityAppSetup, true, false, true )
, ui(*(new Ui::ActivityAppSetupUi))
{
    initActivityAppSetup();
}

//============================================================================
void ActivityAppSetup::initActivityAppSetup( void )
{
    ui.setupUi( this );
    ui.m_TitleBarWidget->setTitleBarText( QObject::tr( "Initialize Application " ) );
    ui.m_TitleBarWidget->setBackButtonVisibility( false );
    ui.m_TitleBarWidget->setHomeButtonVisibility( false );

    ui.m_BottomBarWidget->setExpandWindowVisibility( false );

    ui.m_AvailSpaceBefore->setText( GuiHelpers::getAvailableStorageSpaceText() );

    connectBarWidgets();
    m_AppSetup = new AppSetup();
    connect( m_AppSetup, SIGNAL( signalSetupResult( QObject *, int, qint64 ) ), this, SLOT( slotCopyResult( QObject *, int, qint64 ) ) );
    m_AppSetup->initializeUserAssets();
}

//============================================================================
void ActivityAppSetup::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
}

//============================================================================
void ActivityAppSetup::slotCopyToClipboardButtonClicked( void )
{
    //QClipboard * clipboard = QApplication::clipboard();
   // clipboard->setText( ui.m_InfoText->toPlainText() );
}

//============================================================================
void ActivityAppSetup::slotCopyResult( QObject * workerObj, int rc, qint64 bytesCopied )
{
    if( rc )
    {
        LogMsg( LOG_INFO, "Setup error %d at %" PRId64 " bytes", rc, bytesCopied );
        ui.m_TitleBarWidget->setBackButtonVisibility( true );
    }
    else
    {
        LogMsg( LOG_INFO, "Setup copied %" PRId64 " bytes", bytesCopied );
        QString bytesStr = GuiParams::describeFileLength( bytesCopied );

        QMessageBox::information( this, QObject::tr("Setup completed"), QObject::tr( "Setup success. Bytes Copied " ) + bytesStr, QMessageBox::Ok );
        close();
    }

    m_SetupCompleted = true;
}
