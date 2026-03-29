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
    class AppletOfferRandSessionUi;
}
QT_END_NAMESPACE

class FileShareItemWidget;
class FileItemInfo;
class FileListReplySession;
class FileInfo;
class GuiUser;

class AppletOfferRandSession : public AppletBase, public ToGuiFileXferInterface
{
	Q_OBJECT
public:
	AppletOfferRandSession( AppCommon& app, QWidget* parent, QString launchParam = "" );
	virtual ~AppletOfferRandSession();

	bool						setOffer( EPluginType pluginType, GuiUser* guiUser, std::shared_ptr<GuiOfferSession> existingOffer );
	void						setUser( GuiUser* guiUser ) override;
	void						setPluginType( EPluginType pluginType ) override;	
	void						updatePluginType( EPluginType pluginType );

	bool						setOfferInfo( OfferBaseInfo& offerInfo );
	void						setOfferSessionId( VxGUID& offerSessionId );
	void						setOfferMessage( QString msgText );

protected slots:
    void						statusMsg( QString strMsg );
    void						slotOfferSent( bool sendSuccess );
	void						slotOfferCanceled( void );

	void 						slotPluginRadioClick( void );
	void 						slotSelectTruthOrDareClick( void );
	void 						slotSelectVideoChatClick( void );
	void 						slotSelectVoicePhoneClick( void );

	void						slotSelectedPluginClick( void );
	void						slotSendRandomConnectOffer( void );
	
protected:
    bool                        verifyFile( void );
	void 						updateOfferType( EOfferType offerType );

    //=== vars ===//
    Ui::AppletOfferRandSessionUi& ui;

	VxGUID						m_OfferSessionId;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;
	std::shared_ptr<GuiOfferSession>	m_ExistingOffer{ nullptr };
};


