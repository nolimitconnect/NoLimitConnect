#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include <ptop_src/ptop_engine_src/Plugins/FileInfo.h>
#include "ToGuiFileXferInterface.h"

#include "ui_AppletOfferView.h"

#include <PktLib/VxCommon.h>

class FileShareItemWidget;
class FileItemInfo;
class FileListReplySession;
class FileInfo;
class GuiUser;

class AppletOfferView : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletOfferView( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletOfferView();

	void						setUser( GuiUser* guiUser ) override;
	void						setPluginType( EPluginType pluginType ) override;

	bool						setOfferInfo( OfferBaseInfo& offerInfo );
	void						setOfferSessionId( VxGUID& offerSessionId );
	void						setOfferMessage( QString msgText );

protected slots:
    void						slotHomeButtonClicked( void ) override;
    void						statusMsg( QString strMsg );

protected:
    bool                        verifyFile( void );

    //=== vars ===//
    Ui::AppletOfferViewUi	    ui;

	VxGUID						m_OfferSessionId;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;
};


