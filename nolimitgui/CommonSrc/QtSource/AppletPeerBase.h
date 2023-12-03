#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostBase.h"

#include "GuiPlayerCallback.h"
#include "OfferSessionLogic.h"

class GuiOfferSession;
class GuiUser;
class VidWidget;
class VxLabel;
class VxPushButton;

class AppletPeerBase : public AppletHostBase, public GuiPlayerCallback
{
	Q_OBJECT
public:
	AppletPeerBase( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletPeerBase();

    virtual void				setPluginType( EPluginType pluginType ) override   { m_ePluginType = pluginType; m_OfferSessionLogic.setPluginType( pluginType ); }
    virtual void                setGuiOfferSession( GuiOfferSession* offerSession ) { m_OfferSessionLogic.setGuiOfferSession( offerSession ); }

	void						setupBaseWidgets(	IdentWidget *		friendIdentWidget = 0, 
													VxPushButton *		permissionButton = 0, 
													QLabel *			permissionLabel = 0 );

	void						setVidCamWidget( VidWidget * camWidget )			{ m_VidCamWidget = camWidget; }
	VidWidget *					getVidCamWidget( void )								{ return m_VidCamWidget; }

    virtual bool				offerSession( GuiUser* guiUser, GuiOfferSession* existingOffer = nullptr );

	virtual bool				fromGuiSetGameValueVar(	int32_t varId, int32_t varValue );
	virtual bool				fromGuiSetGameActionVar( int32_t actionId, int32_t actionValue );

    virtual void				onActivityFinish( void ) override;

protected:
    void				        callbackUserAdded( GuiUser* guiUser ) override; 
    void				        callbackUserRemoved( VxGUID& onlineId ) override;
    void                        callbackUserUpdated( GuiUser* guiUser ) override;
    void                        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

    virtual void				callbackToGuiRxedPluginOffer( GuiOfferSession* offer ) override;
    virtual void				callbackToGuiRxedOfferReply( GuiOfferSession* offerReply ) override;
    virtual void				callbackToGuiPluginSessionEnded( GuiOfferSession* offer ) override;

	virtual void				callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 ) override;

    virtual void				toGuiSetGameValueVar(	EPluginType     pluginType,
                                                        VxGUID&         onlineId,
														int32_t			s32VarId, 
                                                        int32_t			s32VarValue ) override;

    virtual void				toGuiSetGameActionVar(	EPluginType     pluginType,
                                                        VxGUID&         onlineId,
														int32_t			s32VarId, 
                                                        int32_t			s32VarValue ) override;

    virtual void				showEvent( QShowEvent* ev ) override;
    virtual void				hideEvent( QHideEvent* ev ) override;
    virtual void				closeEvent( QCloseEvent * ev ) override;

	//=== vars ===//
    bool						m_bFirstMsg{false};
    VidWidget *					m_VidCamWidget{nullptr};
	OfferSessionLogic			m_OfferSessionLogic;

private:
    void						setupAppletPeerBase( void );
};
