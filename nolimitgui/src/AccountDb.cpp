//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AccountDb.h"

#include <CoreLib/sqlite3.h>
#include <CoreLib/VxDebug.h>
#include <PktLib/VxCommon.h>

#include <NetLib/NetHostSetting.h>
#include <NetLib/VxGetRandomPort.h>

#include <stdio.h>

namespace
{
	std::string 		DATABASE_NAME 					= "nolimitconnect_acct.db";

	std::string 		TABLE_LAST_LOGIN	 			= "last_login";
	std::string 		TABLE_ACCOUNT_LOGIN	 			= "account_login";
	std::string 		TABLE_ACCOUNT_PROFILE			= "account_profile";
	std::string 		TABLE_FRIENDS	 				= "friends";

	std::string 		TABLE_LAST_NET_HOST_SETTING	 	= "last_net_host_setting";
    std::string 		CREATE_COLUMNS_LAST_NET_HOST_SETTING = " (last_net_host_setting_name TEXT PRIMARY KEY) ";
    std::string 		COLUMNS_LAST_NET_HOST_SETTING = "last_net_host_setting_name";

	std::string 		TABLE_NET_HOST_SETTINGS	 		= "net_host_settings";
    std::string 		CREATE_COLUMNS_NET_HOST_SETTINGS = " (net_host_setting_name TEXT PRIMARY KEY, network_name TEXT, net_host_url TEXT, connect_test_url TEXT, rand_connect_url TEXT, group_host_url TEXT, chat_room_host_url TEXT, extern_ip_addr TEXT, pref_adapt_ip TEXT, use_upnp INTEGER, tcp_port INTEGER, firewall_type INTEGER )  ";
    std::string 		COLUMNS_NET_HOST_SETTINGS = "net_host_setting_name,network_name,net_host_url,connect_test_url,rand_connect_url,group_host_url,chat_room_host_url,extern_ip_addr,pref_adapt_ip,use_upnp,tcp_port, firewall_type";

	std::string 		COLUMNS_LAST_LOGIN				= " online_name ";
	std::string 		CREATE_COLUMNS_LAST_LOGIN		= " (id INTEGER PRIMARY KEY AUTOINCREMENT, online_name  TEXT) ";
	std::string 		COLUMNS_ACOUNT_LOGIN			= "online_id,online_name,ident";
	std::string 		CREATE_COLUMNS_ACOUNT_LOGIN		= " (online_id TEXT PRIMARY KEY, online_name TEXT, ident BLOB)";
	std::string 		COLUMNS_ACOUNT_PROFILE  		= "online_id,greeting,about,picture,url1,url2,url3,donation";
	std::string 		CREATE_COLUMNS_ACOUNT_PROFILE  	= " (online_id TEXT PRIMARY KEY, greeting TEXT, greet TEXT, about TEXT, picture TEXT, url1 TEXT, url2 TEXT, url3 TEXT, donation TEXT) ";
	std::string 		COLUMNS_FRIENDS  				= "online_id,his_friendship,my_friendship,ident";
	std::string 		CREATE_COLUMNS_FRIENDS  		= " (online_id TEXT PRIMARY KEY, his_friendship TINYINT, my_friendship TINYINT, ident BLOB ) ";

}

//============================================================================
//=== globals ===//
//============================================================================
AccountDb g_oDataHelper;

//============================================================================
AccountDb::AccountDb()
: DbBase( "DataHelperDb" )
{
}

//============================================================================
//! create tables in database 
int32_t AccountDb::onCreateTables( int iDbVersion )
{
    std::string exeStr = "CREATE TABLE " + TABLE_LAST_LOGIN + CREATE_COLUMNS_LAST_LOGIN;
    int32_t rc = sqlExec(exeStr);
    exeStr = "CREATE TABLE " + TABLE_ACCOUNT_LOGIN + CREATE_COLUMNS_ACOUNT_LOGIN;
    rc |= sqlExec( exeStr );
    exeStr = "CREATE TABLE " + TABLE_ACCOUNT_PROFILE + CREATE_COLUMNS_ACOUNT_PROFILE;
    rc |= sqlExec( exeStr );
    exeStr = "CREATE TABLE " + TABLE_FRIENDS + CREATE_COLUMNS_FRIENDS;
    rc |= sqlExec( exeStr );
	exeStr = "CREATE TABLE " + TABLE_LAST_NET_HOST_SETTING + CREATE_COLUMNS_LAST_NET_HOST_SETTING;
	rc |= sqlExec( exeStr );
	exeStr = "CREATE TABLE " + TABLE_NET_HOST_SETTINGS + CREATE_COLUMNS_NET_HOST_SETTINGS;
	rc |= sqlExec( exeStr );
	if( 0 == rc )
	{
		createDefaultNetworkSettings();
	}

	return rc;
}

//============================================================================
// delete tables in database
int32_t AccountDb::onDeleteTables( int oldVersion ) 
{
    int32_t rc = sqlExec("DROP TABLE IF EXISTS last_login" );
    rc |= sqlExec("DROP TABLE IF EXISTS account_login" );
    rc |= sqlExec("DROP TABLE IF EXISTS account_profile" );
    rc |= sqlExec("DROP TABLE IF EXISTS friends" );
	rc |= sqlExec("DROP TABLE IF EXISTS last_net_host_setting" );
	rc |= sqlExec("DROP TABLE IF EXISTS net_host_settings" );
	return rc;
}

//============================================================================
//! return true if online id exists in table
bool AccountDb::onlineUidExistsInTable( VxGUID& onlineId, std::string& strTableName )
{
	bool bExists = false;
	std::string strOnlineIdHex = onlineId.toHexString();

	DbCursor * cursor = startQueryInsecure(  "SELECT * FROM %s WHERE online_id='%s'", strTableName.c_str(), strOnlineIdHex.c_str() );
	if( NULL != cursor )
	{
		if( cursor->getNextRow() )
		{
			bExists = true;
		}

        cursor->close();
	}

    return bExists;
}

//=========================================================================
// login
//=========================================================================
//============================================================================
//! update last login name
bool AccountDb::updateLastLogin( const char* pThingName ) 
{
	DbBindList bindList( pThingName );
	int32_t rc = sqlExec(  "DELETE FROM last_login" );
	rc |= sqlExec(  "INSERT INTO last_login (online_name) values(?)", bindList );
	return rc ? false : true;
}

//============================================================================
//! get last login name.. return "" if doesn't exist
std::string AccountDb::getLastLogin() 
{
	std::string strThingName = "";
	DbCursor * cursor = startQueryInsecure("SELECT * FROM %s", TABLE_LAST_LOGIN.c_str() );
	if( NULL != cursor )
	{
		if(  cursor->getNextRow() )
		{
			strThingName = cursor->getString(1);
		}

		cursor->close();
	}

	return strThingName;
}

//=========================================================================
// accounts
//=========================================================================
//============================================================================
//! insert new account
bool AccountDb::insertAccount( VxNetIdent& oUserAccount ) 
{
	std::string strOnlineNameHex = oUserAccount.getMyOnlineId().toHexString();
	DbBindList bindList( oUserAccount.getOnlineName() );
	sqlExec(  "DELETE FROM account_login WHERE online_name=?", bindList );
	bindList.add( &oUserAccount, sizeof( VxNetIdent ) );
	bindList.add( strOnlineNameHex.c_str() );

	int32_t rc = sqlExec( "INSERT INTO account_login (online_name,ident,online_id) values(?,?,?)", bindList );
	if( rc )
	{
		LogMsg( LOG_ERROR, "AccountDb::insertAccount: ERROR %d %s", rc, sqlite3_errmsg(m_Db) );
	}
	
	return rc ? false : true;
}

//============================================================================
//! update existing account
bool AccountDb::updateAccount( VxNetIdent& oUserAccount ) 
{
    return insertAccount( oUserAccount );
}

//============================================================================
//! retrieve account by name
bool AccountDb::getAccountByName(const char* name, VxNetIdent& oUserAccount ) 
{
    if( NULL == name )
    {
        LogMsg( LOG_ERROR, "AccountDb::%s: null name", __func__ );
        return false;
    }

	bool bResult = false;
	DbCursor * cursor = startQueryInsecure( "SELECT ident FROM account_login WHERE online_name='%s'", name );
	if( NULL != cursor )
	{
		if( cursor->getNextRow() )
		{
			int iBlobLen = 0;
			VxNetIdent* netIdent = (VxNetIdent*)cursor->getBlob(0, &iBlobLen );
			if( iBlobLen == sizeof( VxNetIdent ) )
			{
				memcpy( &oUserAccount, netIdent, sizeof( VxNetIdent ));
				bResult = true;
			}
			else
			{
				LogMsg( LOG_ERROR, "AccountDb::getAccountByName: incorrect blob len in db.. was code changed????");
				cursor->close();
				// remove the invalid blob
				DbBindList bindList( name );
				int32_t rc = sqlExec(  "DELETE FROM account_login WHERE online_name=?", bindList );
				if( rc )
				{
					LogMsg( LOG_ERROR, "AccountDb::getAccountByName: could not remove login by name %s", name );
				}

				return false;
			}
		}

		cursor->close();
	}

	return bResult;
}

//============================================================================
//! remove account by name
bool AccountDb::removeAccountByName(const char* name ) 
{
	DbBindList bindList( name );
	int32_t rc = sqlExec( "DELETE FROM account_login WHERE online_name=?", bindList );
	if( rc == SQLITE_OK )
	{
		return true;
	}

	return false;
}

//=========================================================================
// friends
//=========================================================================
//============================================================================
//! get all known about friend
bool AccountDb::getUserProfile( VxNetIdent& oUserAccount, UserProfile& oProfile ) 
{
	bool bResult = false;
	std::string strOnlineIdHex = oUserAccount.getMyOnlineId().toHexString();

	DbCursor * cursor = startQueryInsecure("SELECT greeting,about,picture,url1,url2,url3,donation FROM account_profile WHERE online_id='%s'",
		strOnlineIdHex.c_str() );
	if( nullptr != cursor )
	{
		if( cursor->getNextRow() )
		{
			if( strlen( cursor->getString(0) ) )
			{
				oProfile.m_strGreeting = cursor->getString(0);
				if( 0 == oProfile.m_strGreeting.length() )
				{
					oProfile.m_strGreeting = oUserAccount.getOnlineDescription();
				}
			}
			else
			{
				oProfile.m_strGreeting = oUserAccount.getOnlineDescription();
			}

			oProfile.m_strAboutMe = cursor->getString(1);
			oProfile.m_strPicturePath = cursor->getString(2);
			oProfile.m_strUrl1 = cursor->getString(3);
			oProfile.m_strUrl2 = cursor->getString(4);
			oProfile.m_strUrl3 = cursor->getString(5);
            oProfile.m_strDonation = cursor->getString( 6 );

			bResult = true;
		}

		cursor->close();
	}

	return bResult;
}

//============================================================================
//! update user profile
bool AccountDb::updateUserProfile( VxNetIdent& oUserAccount, UserProfile& oProfile ) 
{
	int32_t rc = 0;
	std::string strOnlineIdHex = oUserAccount.getMyOnlineId().toHexString();
	DbBindList bindList( (const char*)oProfile.m_strGreeting.toUtf8().constData() );
	bindList.add( (const char*)oProfile.m_strAboutMe.toUtf8().constData() );
	bindList.add( (const char*)oProfile.m_strPicturePath.toUtf8().constData() );
	bindList.add( (const char*)oProfile.m_strUrl1.toUtf8().constData() );
	bindList.add( (const char*)oProfile.m_strUrl2.toUtf8().constData() );
	bindList.add( (const char*)oProfile.m_strUrl3.toUtf8().constData() );
    bindList.add( (const char*)oProfile.m_strDonation.toUtf8().constData() );
	bindList.add( strOnlineIdHex.c_str() );

	if( onlineUidExistsInTable( oUserAccount.getMyOnlineId(), TABLE_ACCOUNT_PROFILE ))
	{
		DbBindList bindList2( strOnlineIdHex.c_str() );
		rc = sqlExec(  "DELETE FROM account_profile WHERE online_id=?", bindList2 );
	}
	
	rc |= sqlExec( "INSERT INTO account_profile (greeting,about,picture,url1,url2,url3,donation,online_id) values(?,?,?,?,?,?,?,?)", bindList );


	return ( 0 == rc ) ? true : false;
}

//============================================================================
//! get list of friend with give friendship
void AccountDb::getFriendList(uint8_t u8MyFrienship,  std::vector<VxNetIdent>& aoIdent ) 
{
    DbCursor * cursor = startQueryInsecure("SELECT ident FROM friends WHERE my_frienship = %d;", u8MyFrienship );
	VxNetIdent oIdent;
	if( NULL != cursor )
	{
		while( cursor->getNextRow() ) 
		{
			int iBlobLen = 0;
			VxNetIdent* netIdent = (VxNetIdent*)cursor->getBlob(0, &iBlobLen );
			vx_assert( iBlobLen == sizeof( VxNetIdent ));
			memcpy( &oIdent, netIdent, sizeof( VxNetIdent ));
			aoIdent.push_back(oIdent); 
		}

		cursor->close();
	}
}

//============================================================================
//! update net info about friend
bool AccountDb::updateFriend( VxNetIdent& oIdent ) 
{
	int32_t rc;
	std::string strOnlineIdHex = oIdent.getMyOnlineId().toHexString();
	DbBindList bindList( oIdent.getHisFriendshipToMe() );
	bindList.add( oIdent.getMyFriendshipToHim() );
	bindList.add( &oIdent, sizeof( VxNetIdent ) );
	bindList.add( strOnlineIdHex.c_str() );

	if( onlineUidExistsInTable( oIdent.getMyOnlineId(), TABLE_FRIENDS ))
	{
		rc = sqlExec( "UPDATE friends SET his_friendship=?, my_friendship=?, ident=? WHERE online_id=?", bindList );
	}
	else
	{
		rc = sqlExec( "INSERT INTO friends (his_friendship,my_friendship,ident,online_id) values(?,?,?,?)", bindList );
	}

	return ( 0 == rc ) ? true : false;
}

//============================================================================
bool AccountDb::updateNetHostSetting( NetHostSetting& netSetting )
{
	removeNetHostSettingByName( netSetting.getNetHostSettingName().c_str() );
   
	DbBindList bindList( netSetting.getNetHostSettingName().c_str() );
	bindList.add( netSetting.getNetworkKey().c_str() );
	bindList.add( netSetting.getNetworkHostUrl().c_str() );
	bindList.add( netSetting.getConnectTestUrl().c_str() );
    bindList.add( netSetting.getRandomConnectUrl().c_str() );
    bindList.add( netSetting.getGroupHostUrl().c_str() );
    bindList.add( netSetting.getChatRoomHostUrl().c_str() );
    bindList.add( netSetting.getUserSpecifiedExternIpAddr().c_str() );
	bindList.add( netSetting.getUseIpv6() ? "1" : "0");

    bindList.add( netSetting.getUseUpnpPortForward() );
    bindList.add( netSetting.getTcpPort() );
	bindList.add( netSetting.getFirewallTestType() );


	int32_t rc = sqlExec( "INSERT INTO net_host_settings (net_host_setting_name,network_name,net_host_url,connect_test_url,rand_connect_url,group_host_url,chat_room_host_url,extern_ip_addr,pref_adapt_ip,use_upnp,tcp_port, firewall_type) values(?,?,?,?,?,?,?,?,?,?,?,?)", bindList );
	return ( 0 == rc ) ? true : false;
}

//============================================================================
bool AccountDb::getNetHostSettingByName( const char* name, NetHostSetting& netSetting )
{
	bool bResult = false;
	DbCursor * cursor = startQueryInsecure( "SELECT * FROM net_host_settings WHERE net_host_setting_name='%s'", name );
	if( NULL != cursor )
	{
		if( cursor->getNextRow() )
		{
			netSetting.setNetHostSettingName( name );
			netSetting.setNetworkKey( cursor->getString(1) );
			netSetting.setNetworkHostUrl( cursor->getString(2) );
			netSetting.setConnectTestUrl( cursor->getString(3) );
            netSetting.setRandomConnectUrl( cursor->getString( 4 ) );
            netSetting.setGroupHostUrl( cursor->getString( 5 ) );
            netSetting.setChatRoomHostUrl( cursor->getString( 6 ) );
            netSetting.setUserSpecifiedExternIpAddr( cursor->getString( 7 ) );
			std::string useIpv6Str = cursor->getString( 8 );
            netSetting.setUseIpv6( useIpv6Str == "1" );
			netSetting.setUseUpnpPortForward( ( 0 == cursor->getS32( 9 ) ) ? false : true );
            netSetting.setTcpPort( cursor->getS32( 10 ) );
			netSetting.setFirewallTestType( cursor->getS32( 11 ) );

			bResult = true;
		}

		cursor->close();
	}

	return bResult;
}

//============================================================================
bool AccountDb::getAllNetHostSettings( std::vector<NetHostSetting>& netSettingList )
{
	bool bResult = false;
	DbCursor * cursor = startQueryInsecure( "SELECT * FROM net_host_settings" );
	if( NULL != cursor )
	{
		while( cursor->getNextRow() )
		{
			NetHostSetting netSetting;

			netSetting.setNetHostSettingName( cursor->getString(0) );
			netSetting.setNetworkKey( cursor->getString(1) );
			netSetting.setNetworkHostUrl( cursor->getString(2) );
			netSetting.setConnectTestUrl( cursor->getString(3) );
            netSetting.setRandomConnectUrl( cursor->getString( 4 ) );
            netSetting.setGroupHostUrl( cursor->getString( 5 ) );
            netSetting.setChatRoomHostUrl( cursor->getString( 6 ) );

            netSetting.setUserSpecifiedExternIpAddr( cursor->getString( 7 ) );
			std::string useIpv6Str = cursor->getString( 8 );
            netSetting.setUseIpv6( useIpv6Str == "1" );

            netSetting.setUseUpnpPortForward( ( 0 == cursor->getS32( 9 ) ) ? false : true );
            netSetting.setTcpPort( cursor->getS32( 10 ) );
            netSetting.setFirewallTestType( cursor->getS32( 11 ) );
			netSettingList.push_back( netSetting );

			bResult = true;
		}

		cursor->close();
	}

	return bResult;
}

//============================================================================
bool AccountDb::removeNetHostSettingByName( const char* name )
{
	DbBindList bindList( name );
	int32_t rc = sqlExec(  "DELETE FROM net_host_settings WHERE net_host_setting_name=?", bindList );
	return rc ? false : true;
}

//============================================================================
bool AccountDb::updateLastNetHostSettingName( const char* name )
{
	DbBindList bindList( name );
	int32_t rc = sqlExec(  "DELETE FROM last_net_host_setting" );
	rc |= sqlExec(  "INSERT INTO last_net_host_setting (last_net_host_setting_name) values(?)", bindList );
	return rc ? false : true;
}

//============================================================================
std::string AccountDb::getLastNetHostSettingName( void )
{
	std::string strSettingName = "";
	DbCursor * cursor = startQueryInsecure("SELECT * FROM %s", TABLE_LAST_NET_HOST_SETTING.c_str() );
	if( NULL != cursor )
	{
		if( cursor->getNextRow() )
		{
			if( cursor->getString(0) )
			{
				strSettingName = cursor->getString(0);
			}
		}

		cursor->close();
	}

	return strSettingName;
}

//============================================================================
bool AccountDb::getAllAccounts( std::vector<VxNetIdent>& accountList )
{
    accountList.clear();

    bool bResult = false;
    DbCursor * cursor = startQueryInsecure( "SELECT * FROM account_login" );
    if( nullptr != cursor )
    {
        while( cursor->getNextRow() )
        {
            int iBlobLen = 0;
            VxNetIdent* netIdent = ( VxNetIdent* )cursor->getBlob( 2, &iBlobLen );
            if( iBlobLen == (int)sizeof( VxNetIdent ) )
            {
                accountList.push_back( *netIdent );
                bResult = true;
            }
            else if( iBlobLen >= (int)sizeof( VxConnectIdent ) )
            {
                LogMsg( LOG_ERROR, "AccountDb::getAllAccounts: incorrect blob len in db.. was code changed????" );
                cursor->close();
                // remove the invalid blob
                DbBindList bindList( netIdent->getOnlineName() );
                int32_t rc = sqlExec( "DELETE FROM account_login WHERE online_name=?", bindList );
                if( rc )
                {
                    LogMsg( LOG_ERROR, "AccountDb::getAccountByName: could not remove login by name %s", netIdent->getOnlineName() );
                }

                return false;
            }
        }

        cursor->close();
    }

    return bResult && accountList.size() > 0;
}

//============================================================================
void AccountDb::createDefaultNetworkSettings( void )
{
	// get port to listen on 
    uint16_t tcpPort = VxGetRandomTcpPort();

	NetHostSetting netHostSetting;
	// default ipv4 settings
	netHostSetting.resetToDefaultSettings( false );
	if( tcpPort )
	{
		netHostSetting.setTcpPort( tcpPort );
	}

	updateNetHostSetting( netHostSetting );
	updateLastNetHostSettingName( netHostSetting.getNetHostSettingName().c_str() );

	// default ipv6 settings
	netHostSetting.resetToDefaultSettings( true );
	if( tcpPort )
	{
		netHostSetting.setTcpPort( tcpPort );
	}

	updateNetHostSetting( netHostSetting );
}