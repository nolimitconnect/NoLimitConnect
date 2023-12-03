//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include "AccountDb.h"

class AccountMgr
{
public:
    AccountMgr();
    ~AccountMgr() = default;

    void                        startupAccountMgr( const char* dbPathAndName );
    //! insert new account
    bool						insertAccount( VxNetIdent& userAccount );
    //! update existing account
    bool						updateAccount( VxNetIdent& userAccount );
    //! retrieve account by name
    bool						getAccountByName( const char* name, VxNetIdent& userAccount );
    //! remove account by name
    bool 						removeAccountByName( const char* name );
    //! retrieve all accounts
    bool						getAllAccounts( std::vector<VxNetIdent>& accountList );

    //=========================================================================
    //=== about me page ===//
    //! get profile data for account
    bool						getUserProfile( VxNetIdent& userAccount, UserProfile& oProfile );
    //! update friend profile
    bool						updateUserProfile( VxNetIdent& userAccount, UserProfile& oProfile );
    //=========================================================================
    //=== login ===//
    //! update last login name
    bool						updateLastLogin( std::string& strThingName ) { return updateLastLogin( strThingName.c_str() ); }
    //! update last login name
    bool						updateLastLogin( const char* pThingName );
    //! get last login name.. return "" if doesn't exist
    std::string					getLastLogin();
    //=========================================================================
    //=== host settings ===//
    bool						updateNetHostSetting( NetHostSetting& anchorSetting );
    bool						getNetHostSettingByName( const char* name, NetHostSetting& anchorSetting );
    bool						getAllNetHostSettings( std::vector<NetHostSetting>& anchorSettingList );
    bool 						removeNetHostSettingByName( const char* name );

    bool						updateLastNetHostSettingName( const char* name );
    std::string 				getLastNetHostSettingName( void );
    //=========================================================================

protected:
    AccountDb                   m_AccountDb;

};