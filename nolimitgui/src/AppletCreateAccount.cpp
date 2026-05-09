//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletCreateAccount.h"

#include "ActivityMsgBoxYesNo.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/EngineSettings.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

#include "ui_AppletCreateAccount.h"

//============================================================================
AppletCreateAccount::AppletCreateAccount( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_CREATE_ACCOUNT, app, parent )
, ui(*(new Ui::CreateAccountUi ))
{
    setAppletType( eAppletCreateAccount );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    GuiHelpers::fillGender( ui.m_GenderComboBox );
    GuiHelpers::fillLanguage( ui.m_LanguageComboBox );
    GuiHelpers::fillContentRating( ui.m_ContentComboBox );
    GuiHelpers::fillAge( ui.m_AgeComboBox );

    std::string dataDir = VxGetRootUserDataDirectory();
    ui.m_StoragePath->setText( QString::fromStdString( dataDir ) );

    uint64_t bytesFree = VxFileUtil::getDiskFreeSpace( dataDir.c_str() );
    ui.m_SpaceAvail->setText( GuiParams::describeFileLength( bytesFree ) );
    #if defined(FLATPAKBUILD)
        ui.m_ExtraInfo->setText( "FLATPAK: YES" );
    #else
        ui.m_ExtraInfo->setText( "FLATPAK: NO" );
    #endif

    connect( ui.m_LoginButton, SIGNAL(clicked()), this, SLOT(slotButtonLoginClicked() ) );
}

//============================================================================
//! button clicked
void AppletCreateAccount::slotButtonLoginClicked( void )
{
    if( accountValidate() )
    {
        std::string strUserName = ui.m_UserNameEdit->text().toUtf8().data();

        m_MyApp.loadAccountSpecificSettings( strUserName.c_str() );
        ELanguageType selectedLanguage = GuiHelpers::getLanguage( ui.m_LanguageComboBox );
        m_MyApp.createAccountForUser( strUserName, m_UserAccount, (const char*)ui.m_MoodMessageEdit->text().toUtf8().constData(), 
                          ui.m_GenderComboBox->currentIndex(), (EAgeType)ui.m_AgeComboBox->currentIndex(), (int)selectedLanguage, ui.m_ContentComboBox->currentIndex() );
        m_MyApp.setAccountUserName( strUserName.c_str() );
        m_UserAccount.setAgeType( (EAgeType)ui.m_AgeComboBox->currentIndex() );
        m_UserAccount.setGender( (EGenderType)ui.m_GenderComboBox->currentIndex() );
        m_UserAccount.setPrimaryLanguage( selectedLanguage );
        m_UserAccount.setPreferredContent( (EContentRating)ui.m_ContentComboBox->currentIndex() );
        m_UserAccount.setOnlineDescription( ( const char* )ui.m_MoodMessageEdit->text().toUtf8().constData() );

        m_UserAccount.setPluginPermissionsToDefaultValues();

        // save ident to global
        memcpy( m_MyApp.getAppGlobals().getMyNetIdent(), &m_UserAccount, sizeof( VxNetIdent ) );
        m_MyApp.getUserMgr().updateMyIdent( m_MyApp.getAppGlobals().getMyNetIdent() );

        // fill in database info
        LogMsg( LOG_VERBOSE, "created acct name %s UUID %s", m_UserAccount.getOnlineName(), m_UserAccount.getMyOnlineId().toOnlineIdString().c_str() );
        if( m_MyApp.getAccountMgr().insertAccount( m_UserAccount ) )
        {
            m_MyApp.getAccountMgr().updateLastLogin( m_UserAccount.getOnlineName() );
            emit signalAccountCreated( true );
            closeApplet();
        }
        else
        {
            LogMsg( LOG_ERROR, "Insert New User into Database failed." );
            QMessageBox::warning( this, QObject::tr( "Application" ), QObject::tr( "Insert New User into Database failed." ) );
        }
    }
}

//============================================================================
//! validate user input
bool AppletCreateAccount::accountValidate( void )
{
    QString strUserName = ui.m_UserNameEdit->text();
    bool validAccount = GuiHelpers::validateUserName( this, strUserName );

    QString strMoodMsg = ui.m_MoodMessageEdit->text();
    validAccount &= GuiHelpers::validateMoodMessage( this, strMoodMsg );

    validAccount &= GuiHelpers::validateAge( this, ui.m_AgeComboBox->currentIndex() );

    return validAccount;
}

//============================================================================
bool AppletCreateAccount::wasLoginNameEntered( void )
{
    QString strUserName = ui.m_UserNameEdit->text();
    return strUserName.length() > 3 ? true : false;
}

//============================================================================
void AppletCreateAccount::onBackButtonClicked( void )
{
    QString title = QObject::tr( "Create an account or exit" );
    QString msg = QObject::tr( "User must create an account or exit\nDo you want to exit the application?" );
    ActivityMsgBoxYesNo msgBox( m_MyApp, this, title, msg );
    if( QDialog::Accepted == msgBox.exec() )
    {
        m_MyApp.shutdownAppCommon();
    }
}
