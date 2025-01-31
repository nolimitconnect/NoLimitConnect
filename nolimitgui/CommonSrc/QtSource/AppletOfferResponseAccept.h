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
#include <Plugins/FileInfo.h>
#include "ToGuiFileXferInterface.h"

#include <OfferBase/OfferBaseInfo.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletOfferResponseAcceptUi;
}
QT_END_NAMESPACE

class FileShareItemWidget;
class FileItemInfo;
class FileListReplySession;
class FileInfo;
class GuiUser;

class AppletOfferResponseAccept : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletOfferResponseAccept( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletOfferResponseAccept();

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
    Ui::AppletOfferResponseAcceptUi&	    ui;

    VxGUID						m_OfferId;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;
};


