//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletAboutUser.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIconsDefs.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiHelpers.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QScrollBar>

#include "ui_AppletAboutUser.h"

namespace
{
    const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit *            AppletAboutUser::getInfoEdit( void )     { return ui.m_InfoPlainTextEdit; }

//============================================================================
AppletAboutUser::AppletAboutUser( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_ABOUT_USER, app, parent )
, ui(*(new Ui::AppletAboutUserUi))
{
    setAppletType( eAppletAboutUser );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    getInfoEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
    getInfoEdit()->setReadOnly( true );

	m_MyApp.activityStateChange( this, true );
    wantActivityCallbacks( true );
}

//============================================================================
AppletAboutUser::~AppletAboutUser()
{
    wantActivityCallbacks( false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletAboutUser::setUser( GuiUser* guiUser )
{
    if( !guiUser )
    {
        LogMsg( LOG_ERROR, "AppletAboutUser::%s null guiUser", __func__ );
        vx_assert( false );
        return;
    }

    m_GuiUser = guiUser;

    ui.m_IdentWidget->setupIdentLogic();
    ui.m_IdentWidget->updateIdentity( guiUser );
    fillUserDetails( guiUser );
}

//============================================================================
void AppletAboutUser::fillUserDetails( GuiUser* guiUser )
{
    ui.m_UrlText->setText( QString( guiUser->getMyOnlineUrl().c_str() ) );
    VxNetIdent& netIdent = guiUser->getNetIdent();
    ui.m_AgeText->setText( GuiParams::describeAge( netIdent.getAgeType() ) );
    ui.m_GenderText->setText( GuiParams::describeGender( netIdent.getGender() ) );
    ui.m_LanguageText->setText( GuiParams::describeLanguage( netIdent.getPrimaryLanguage() ) );
    ui.m_ContentText->setText( GuiParams::describeContentRating( netIdent.getPreferredContent() ) );
    ui.m_MyFriendship->setText( GuiParams::describeFriendship( netIdent.getMyFriendshipToHim() ) );
    ui.m_UsersFrienshipText->setText( GuiParams::describeFriendship( netIdent.getHisFriendshipToMe() ) );
    ui.m_TruthsText->setText( QString::number( netIdent.getTruthCount() ) );
    ui.m_DaresText->setText( QString::number( netIdent.getDareCount() ) );
    infoMsg( "URL: %s", guiUser->getMyOnlineUrl().c_str() );
    if( netIdent.requiresRelay() )
    {
        infoMsg( QObject::tr( "Requires Relay" ) );
    }
    else
    {
        infoMsg( QObject::tr( "Can Direct Connect" ) );
    }

    bool hadDisabledPlugins{ false };
    for( int i = 1; i < eMaxPermissionPluginType; i++ )
    {
        EPluginType pluginType = (EPluginType)i;
        if( netIdent.getPluginPermission( pluginType ) == eFriendStateIgnore )
        {
            hadDisabledPlugins = true;
            continue;
        }

        infoMsg( GuiParams::describePluginType( pluginType ) + " - Permission Required: " + GuiParams::describeFriendState( netIdent.getPluginPermission( pluginType ) ) );
        if( ePluginTypeFileShareServer == pluginType )
        {
            QString typeText( " -- " );
            uint8_t fileTypes = netIdent.getSharedFileTypes();
            if( !fileTypes )
            {
                typeText += QObject::tr("no shared files");
            }
            else
            {
                typeText += GuiParams::describeFileTypes( fileTypes );
            }

            infoMsg( typeText );
        }

        if( ePluginTypeAboutMePageServer == pluginType )
        {
            if( netIdent.hasProfilePicture() )
            {
                infoMsg( QObject::tr( " -- has about me picture" ) );
            }
            else
            {
                infoMsg( QObject::tr( " -- no about me picture" ) );
            }
        }      
    }

    if( hadDisabledPlugins )
    {
        infoMsg( QObject::tr( "=== Disabled Plugins ===" ) );
        for( int i = 1; i < eMaxPermissionPluginType; i++ )
        {
            EPluginType pluginType = (EPluginType)i;
            if( netIdent.getPluginPermission( pluginType ) != eFriendStateIgnore )
            {
                continue;
            }

            infoMsg( GuiParams::describePluginType( pluginType ) );
        }
    }
}

//============================================================================
void AppletAboutUser::infoMsg( const QString& text )
{
    getInfoEdit()->appendPlainText( text ); // Adds the message to the widget
    getInfoEdit()->verticalScrollBar()->setValue( getInfoEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}

//============================================================================
void AppletAboutUser::infoMsg( std::string text )
{
    getInfoEdit()->appendPlainText( QString( text.c_str() ) ); // Adds the message to the widget
    getInfoEdit()->verticalScrollBar()->setValue( getInfoEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}

//============================================================================
void AppletAboutUser::infoMsg( const char* msg, ... )
{
    std::array<char, MAX_INFO_MSG_SIZE> szBuffer;
	va_list arg_ptr;
	va_start(arg_ptr, msg);
    vsnprintf(szBuffer.data(), MAX_INFO_MSG_SIZE, msg, arg_ptr);
	szBuffer.data()[MAX_INFO_MSG_SIZE - 1] = 0;
	va_end(arg_ptr);

    getInfoEdit()->appendPlainText( QString( szBuffer.data() ) ); // Adds the message to the widget
    getInfoEdit()->verticalScrollBar()->setValue( getInfoEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom
}
