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

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiAudioLevelCallback.h"
#include "GuiFriendRequest/GuiFriendRequestCallback.h"
#include "GuiHostJoinCallback.h"
#include "GuiOfferCallback.h"
#include "GuiVideoTitleBarCallback.h"
#include "GuiPluginMgrCallback.h"
#include "MyIconsDefs.h"
#include "ToGuiActivityInterface.h"
#include "ToGuiHardwareControlInterface.h"

#include <QListWidgetItem>
#include <QTimer>

#include <CoreLib/VxGUID.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class TitleBarWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;
class GuiOfferMgr;
class GuiOfferSession;
class GuiUser;
class MyIcons;
class QLabel;
class VxPushButton;

class TitleBarWidget : public QWidget,
	public ToGuiActivityInterface,
	public ToGuiHardwareControlInterface,
	public GuiAudioLevelCallback,
	public GuiPluginMgrCallback,
	public GuiOfferCallback,
	public GuiHostJoinCallback,
	public GuiVideoTitleBarCallback,
	public GuiFriendRequestListCallback
{
	Q_OBJECT

public:
	TitleBarWidget( QWidget* parent = nullptr );
	virtual ~TitleBarWidget();

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setTitleBarText( QString titleText );
	QString						getTitleBarText( void );

	VxPushButton*				getAppIconPushButton( void );
	VxPushButton*				getBackButton( void );

	//=== button visiblility ====//
	void						setPopupVisibility( bool hideBackButton = false );// for popup dialogs

    void				        setBackButtonVisibility( bool visible );
	void						setCamPreviewVisibility( bool visible );
	void						setCamViewerCountVisibility( bool visible );
	void						setCameraButtonVisibility( bool visible );

	void						setNetStatusVisibility( bool visible );

    void						setMenuTopButtonVisibility( bool visible );
	void						setMenuListButtonVisibility( bool visible );
    void						setPowerButtonVisibility( bool visible );

	void						setOfferListButtonVisibility( bool visible );
	void						setHostJoinRequestListButtonVisibility( bool visible );

	//=== button icons ====//
    void           				setPowerButtonIcon( EMyIcons myIcon = eMyIconPowerOff );
	void           				setMicrophoneIcon( EMyIcons myIcon = eMyIconMicrophoneOn );
	void           				setSpeakerIcon( EMyIcons myIcon = eMyIconSpeakerOn );
	void           				setCameraIcon( EMyIcons myIcon = eMyIconCameraNormal );

	void           				setTopMenuButtonIcon( EMyIcons myIcon = eMyIconMenu );
	void           				setBackButtonIcon( EMyIcons myIcon = eMyIconBack );

	//=== button colors ====//
	void           				setPowerButtonColor( QColor iconColor );
	void           				setMicrophoneColor( QColor iconColor );
	void           				setSpeakerColor( QColor iconColor );
	void           				setCameraColor( QColor iconColor );
	void           				setTopMenuButtonColor( QColor iconColor );
	void           				setBackButtonColor( QColor iconColor );

signals:
	void						signalPowerButtonClicked( void );

	void						signalCameraSnapshotButtonClicked( void );
	void						signalCamPreviewClicked( void );

	void						signalMenuTopButtonClicked( void );
	void						signalBackButtonClicked( void );

public slots:
	void           				updateTitleBar( void );
	void           				slotSystemReady( bool isReady );

	void           				slotApplicationIconClicked( void );
	void           				slotPowerButtonClicked( void );

	void           				slotMuteMicButtonClicked( void );
	void           				slotMuteSpeakerButtonClicked( void );
	void           				slotCameraSnapshotButtonClicked( void );
	void           				slotCamPreviewClicked( void );

	void           				slotMenuTopButtonClicked( void );
	void           				slotBackButtonClicked( void );

    void           				slotToGuiNetAvailStatus( ENetAvailStatus eNetAvailStatus );
    void           				slotCamTimeout( void );
    void           				slotSignalHelpClick( void );

    void           				slotHostJoinRequestButtonClicked( void );
    void           				slotOfferListButtonClicked( void );

	void           				slotTitleBarUserMenuButtonClicked( void );

	void           				slotFriendRequestListButtonClicked( void );

	void           				slotTestButtonClicked( void );

protected:
	QWidget*					getTitleBarParentFrame( void );
	QWidget*					getTitleBarParentPage( void );
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;
	void						resizeEvent( QResizeEvent* ev ) override;

	void						checkTitleBarIconsFit( void );

	void						callbackGuiMicrophoneLevel( int micLevel ) override;
	virtual void				callbackWantMicrophoneCount( int wantMicCount ) override;
	virtual void				callbackWantSpeakerCount( int wantSpeakerCount ) override;

	void						callbackToGuiWantMicrophoneRecording( bool wantMicInput ) override;

	void						callbackToGuiWantVideoCapture( bool wantVideoCapture ) override;
	void						callbackToGuiMicrophoneMuted( bool isMuted ) override;
	void						callbackToGuiSpeakerMuted( bool isMuted ) override;

	void						callbackToGuiCameraEnable( bool enableCamera ) override;
	void						callbackToGuiCaptureRunning( bool camCaptureRunning ) override;

	void						callbackActiveOfferCount( int activeCnt, int historyCnt ) override;
	void						callbackJoinRequestCount( int requestCnt ) override;

	void						callbackGuiVideoTitleBarPixmap( QPixmap& vidPixmap) override;

	void						callbackToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue ) override;

	void						callbackGuiFriendRequestListUpdated( GuiFriendRequest* friendRequest ) override;
    void						callbackGuiFriendRequestListRemoved( VxGUID requestId ) override;

	void						updateWebServerClientCount( void );

	void						wantCallbacks( bool enableCallbacks );

	void						updateCamCallbackRequests( void );
	void						updateAudioLevelCallbackRequests( void );
	void						updateFriendRequestNotify( void );

	void						setWantMicrophoneCount( int wantMicCount );
	void						setWantSpeakerCount( int wantSpeakerCount );

	Ui::TitleBarWidgetClass&	ui;
	AppCommon&					m_MyApp;
    GuiOfferMgr&				m_OfferMgr;
	bool						m_MutedMic{ false };
	bool						m_MutedSpeaker{ false };
	bool						m_MicrophonePlaying{ false };
	int							m_LastMicLevel{ 0 };

	bool						m_CamEnabled{ false };
	QTimer*                     m_CamTimer;
    bool                        m_CamPlaying{ false };
    uint64_t                    m_LastCamFrameTimeMs{ 0 };

    VxGUID                      m_MyOnlineId;
	bool                        m_CallbacksRequested{ false };

	bool						m_IsPopupDialog{ false };
	bool						m_VidCallbacksRequested{ false };
	bool						m_AudioLevelCallbacksRequested{ false };
	std::string					m_TitleText;
};
