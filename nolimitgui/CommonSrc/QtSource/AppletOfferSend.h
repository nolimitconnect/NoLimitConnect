#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include <ptop_src/ptop_engine_src/Plugins/FileInfo.h>
#include "ToGuiFileXferInterface.h"

#include "ui_AppletOfferSend.h"

#include <PktLib/VxCommon.h>

class FileShareItemWidget;
class FileItemInfo;
class FileListReplySession;
class FileInfo;
class GuiUser;

class AppletOfferSend : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletOfferSend( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletOfferSend();

	bool						setOffer( EPluginType pluginType, GuiUser* guiUser, GuiOfferSession* existingOffer );
	void						setUser( GuiUser* guiUser ) override;
	void						setPluginType( EPluginType pluginType ) override;	

	bool						setOfferInfo( OfferBaseInfo& offerInfo );
	void						setOfferSessionId( VxGUID& offerSessionId );
	void						setOfferMessage( QString msgText );

protected slots:
    void						slotHomeButtonClicked( void ) override;
    void						statusMsg( QString strMsg );
	void						slotOfferSent( void );

protected:
    bool                        verifyFile( void );

    //=== vars ===//
    Ui::AppletOfferSendUi	    ui;

	VxGUID						m_OfferSessionId;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;
	GuiOfferSession*			m_ExistingOffer{ nullptr };
};


