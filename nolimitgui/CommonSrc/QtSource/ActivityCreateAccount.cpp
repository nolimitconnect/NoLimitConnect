//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityCreateAccount.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "AppSettings.h"
#include "GuiHelpers.h"

#include <P2PEngine/EngineSettings.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>
#include <QUuid>

#include "ui_ActivityCreateAccount.h"

TitleBarWidget *  ActivityCreateAccount::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget * ActivityCreateAccount::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityCreateAccount::ActivityCreateAccount( AppCommon& app, QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_CREATE_ACCOUNT, app, parent, eAppletCreateAccount, true, false, true )
, ui(*(new Ui::CreateAccountClass))
{
    ui.setupUi( this );
    setTitleBarText( QObject::tr( "Create Account" ) );

    connectBarWidgets();

    GuiHelpers::fillGender( ui.m_GenderComboBox );
    GuiHelpers::fillLanguage( ui.m_LanguageComboBox );
    GuiHelpers::fillContentRating( ui.m_ContentComboBox );
    GuiHelpers::fillAge( ui.m_AgeComboBox );

    connect( ui.m_LoginButton, SIGNAL(clicked()), this, SLOT( slotButtonLoginClicked() ) );
}

//============================================================================
//! button clicked
void ActivityCreateAccount::slotButtonLoginClicked( void )
{
    if( accountValidate() )
    {
        std::string strUserName = ui.m_UserNameEdit->text().toUtf8().data();

        m_MyApp.loadAccountSpecificSettings( strUserName.c_str() );
        m_MyApp.createAccountForUser( strUserName, m_UserAccount, (const char*)ui.m_MoodMessageEdit->text().toUtf8().constData(), 
                                      ui.m_GenderComboBox->currentIndex(), (EAgeType)ui.m_AgeComboBox->currentIndex(), ui.m_LanguageComboBox->currentIndex(), ui.m_ContentComboBox->currentIndex() );
        m_MyApp.setAccountUserName( strUserName.c_str() );
        m_UserAccount.setAgeType( (EAgeType)ui.m_AgeComboBox->currentIndex() );
        m_UserAccount.setGender( ui.m_GenderComboBox->currentIndex() );
        m_UserAccount.setPrimaryLanguage( ui.m_LanguageComboBox->currentIndex() );
        m_UserAccount.setPreferredContent( ui.m_ContentComboBox->currentIndex() );
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
            accept();
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
bool ActivityCreateAccount::accountValidate( void )
{
    QString strUserName = ui.m_UserNameEdit->text();
    bool validAccount = GuiHelpers::validateUserName( this, strUserName );

    QString strMoodMsg = ui.m_MoodMessageEdit->text();
    validAccount &= GuiHelpers::validateMoodMessage( this, strMoodMsg );

    validAccount &= GuiHelpers::validateAge( this, ui.m_AgeComboBox->currentIndex() );

    return validAccount;
}

//============================================================================
bool ActivityCreateAccount::wasLoginNameEntered( void )
{
    QString strUserName = ui.m_UserNameEdit->text();
    return strUserName.length() > 3 ? true : false;
}

//============================================================================
void ActivityCreateAccount::onBackButtonClicked( void )
{
    // do nothing.. user must click login
}
