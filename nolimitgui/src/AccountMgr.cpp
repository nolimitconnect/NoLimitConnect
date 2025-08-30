//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "AccountMgr.h"

//============================================================================
AccountMgr::AccountMgr()
{
}

//============================================================================
void AccountMgr::startupAccountMgr( const char* dbPathAndName )
{
    m_AccountDb.dbStartup( DATA_HELPER_DB_VERSION, dbPathAndName );
}

//============================================================================
//! update last login name
bool AccountMgr::updateLastLogin( const char* onlineName )
{
    return m_AccountDb.updateLastLogin( onlineName );
}

//============================================================================
bool AccountMgr::insertAccount( VxNetIdent& userAccount )
{
    return m_AccountDb.insertAccount( userAccount );
}

//============================================================================
bool AccountMgr::updateAccount( VxNetIdent& userAccount )
{
    return m_AccountDb.updateAccount( userAccount );
}

//============================================================================
bool AccountMgr::getAccountByName( const char* name, VxNetIdent& userAccount )
{
    return m_AccountDb.getAccountByName( name, userAccount );
}

//============================================================================
bool AccountMgr::removeAccountByName( const char* name )
{
    return m_AccountDb.removeAccountByName( name );
}

//============================================================================
bool AccountMgr::getAllAccounts( std::vector<VxNetIdent>& accountList )
{
    return m_AccountDb.getAllAccounts( accountList );
}

//============================================================================
bool AccountMgr::getUserProfile( VxNetIdent& userAccount, UserProfile& oProfile )
{
    return m_AccountDb.getUserProfile( userAccount, oProfile );
}

//============================================================================
bool AccountMgr::updateUserProfile( VxNetIdent& userAccount, UserProfile& oProfile )
{
    return m_AccountDb.updateUserProfile( userAccount, oProfile );
}

//============================================================================
bool AccountMgr::updateNetHostSetting( NetHostSetting& netHostSetting )
{
    return m_AccountDb.updateNetHostSetting( netHostSetting );
}

//============================================================================
bool AccountMgr::getNetHostSettingByName( const char* name, NetHostSetting& netHostSetting )
{
    return m_AccountDb.getNetHostSettingByName( name, netHostSetting );
}

//============================================================================
std::string AccountMgr::getLastLogin()
{
    return m_AccountDb.getLastLogin();
}

//============================================================================
bool AccountMgr::getAllNetHostSettings( std::vector<NetHostSetting>& netHostSettingList )
{
    return m_AccountDb.getAllNetHostSettings( netHostSettingList );
}

//============================================================================
bool AccountMgr::removeNetHostSettingByName( const char* name )
{
    return m_AccountDb.removeNetHostSettingByName( name );
}

//============================================================================
bool AccountMgr::updateLastNetHostSettingName( const char* name )
{
    return m_AccountDb.updateLastNetHostSettingName( name );
}

//============================================================================
std::string AccountMgr::getLastNetHostSettingName( void )
{
    return m_AccountDb.getLastNetHostSettingName();
}
