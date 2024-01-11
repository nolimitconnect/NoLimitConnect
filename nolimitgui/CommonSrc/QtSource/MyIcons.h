#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MyIconsDefs.h"

#include <GuiInterface/IDefs.h>

#include <QIcon>

#include <vector>

class AppCommon;
class OfferBaseInfo;
class QIcon;
class QPainter;

//! stores all icons
class MyIcons
{
public:
	MyIcons() = default;
	~MyIcons() = default;

	//! load all icons
	void						myIconsStartup( void );

	//! get preloaded icon
    QIcon&						getIcon( enum EMyIcons eMyIcon );
	//! get pixmap of given icon, size and color
    QPixmap						getIconPixmap( enum EMyIcons myIcon, QSize iconSize, QColor& iconColor );
	//! get pixmap of given icon and size but do not colorize
    QPixmap						getIconPixmap( enum EMyIcons myIcon, QSize iconSize );
	//! get icon for give friendship
    EMyIcons					getFriendshipIcon( enum EFriendState eFriendship );
    //! get setting icon for given plugin
    EMyIcons					getPluginSettingsIcon( enum EPluginType pluginType );
	//! get icon for given plugin
    EMyIcons					getPluginIcon( enum EPluginType pluginType, EPluginAccess ePluginAccess = ePluginAccessOk );
    //! get setup icon for given plugin
    EMyIcons                    getPluginSetupIcon( enum EPluginType pluginType );
	//! get locked plugin icon
    EMyIcons					getLockedPluginIcon( enum EPluginType pluginType );
	//! get disabled plugin icon
    EMyIcons					getDisabledPluginIcon( enum EPluginType pluginType );
	//! get disabled plugin icon
    EMyIcons					getIgnoredPluginIcon( enum EPluginType pluginType );
	//! get icon of given file type
	EMyIcons					getFileIcon( uint8_t u8FileType );
	//! get icon of offer
	EMyIcons					getOfferIcon( OfferBaseInfo& offerInfo, enum EPluginType pluginType );
	//! get icon of host type for service
	EMyIcons					getHostTypeHostIcon( enum EHostType hostType );
	//! get icon of host type for client
	EMyIcons					getHostTypeClientIcon( enum EHostType hostType );

	//! get path to icon file
	QString						getIconFile( enum EMyIcons eMyIcon );
	bool						isSvgFile( QString& iconFileName );

    //! draw a icon with given color
    void                        drawIcon( enum EMyIcons eIcon, QPainter* painter, const QRect& rect, QColor& iconColor );


    std::vector<QIcon>          m_aoIcons;
};

