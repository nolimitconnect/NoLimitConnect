#pragma once
//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#if defined(SANDBOX_APP)

#include "AppDefs.h"
#include "AppGlobals.h"

#include "MyIcons.h"
#include "VxAppTheme.h"
#include "VxAppStyle.h"
#include "VxAppDisplay.h" 

#include "GuiInterface/IAudioInterface.h"

#include <QComboBox>
#include <QMessageBox>
#include <QDialog>
#include <QElapsedTimer>
#include <QMutex>

class AccountMgr;
class AppSettings;

class ActivityAppSetup;
class ActivityBase;
class ActivityCreateAccount;
class ActivityShowHelp;
class ActivityOfferListDlg;

class AppletMultiMessenger;
class AppletDownloads;
class AppletUploads;

class AppletMgr;
class BlobInfo;
class FriendListEntryWidget;
class FileListReplySession;
class GuiFileXferSession;
class GuiOfferSession;
class INlc;
class KodiThread;
class PopupMenu;
class RenderGlWidget;

class ToGuiHardwareControlInterface;

class VxPeerMgr;
class VxTilePositioner;

// media
class CRenderBuffer;

class AppCommon : public QWidget, public IAudioCallbacks
{
    Q_OBJECT

public:
    AppCommon( QApplication& myQApp, EDefaultAppMode appDefaultMode, AppSettings& appSettings );
    AppCommon( const AppCommon& rhs ) = delete;
    virtual ~AppCommon() override = default;

    // elapsed high resolution timer 
    int64_t                     elapsedMilliseconds( void );

    int64_t                     elapsedSeconds( void )                      { return elapsedMilliseconds() / 1000; }

    // load profile and icons etc without using thread to avoid linux crash
    void                        loadWithoutThread( void );
    // cannot launch any applets until logon is completed
    void                        setLoginCompleted( bool completed )         { m_LoginComplete = completed; }
    bool                        getLoginCompleted( void )                   { return m_LoginComplete; }

    // some applets cannot be launched until application is fully ready for network use
    void                        setIsAppInitialized( bool initialized )     { m_AppInitialized = initialized; }
    bool                        getIsAppInitialized( void )                 { return m_AppInitialized; }

    void                        setLoopbackMyselfTestAllowed( bool allow )  { m_LoopbackMyselfTestAllowed = allow; }
    bool                        getLoopbackMyselfTestAllowed( void )        { return m_LoopbackMyselfTestAllowed; }

    // diagnose to much cpu usage in gui thread
    void                        setGuiCpuTimeEnable( bool enable )          { m_GuiCpuTimeEnable = enable; }
    bool                        getGuiCpuTimeEnable( void )                 { return m_GuiCpuTimeEnable; }

    VxAppDisplay&				getAppDisplay( void )                       { return m_AppDisplay; }
 
	AppSettings&				getAppSettings( void )						{ return m_AppSettings; }
    QString&					getAppShortName( void )						{ return m_AppShortName; }
    VxAppStyle&					getAppStyle( void )							{ return m_AppStyle; }
    QString&					getAppTitle( void )							{ return m_AppTitle; }
    VxAppTheme&					getAppTheme( void )							{ return m_AppTheme; }

    bool						getIsVidCaptureEnabled( void )				{ return m_VidCaptureEnabled; }
    bool						getIsMicrophoneHardwareEnabled( void )		{ return m_MicrophoneHardwareEnabled; }
    bool						getIsSpeakerHardwareEnabled( void )			{ return m_SpeakerHardwareEnabled; }
    MyIcons&					getMyIcons( void )							{ return m_MyIcons; }

    RenderGlWidget *            getRenderConsumer( void );
    SoundMgr&					getSoundMgr( void )							{ return m_SoundMgr; }

    QApplication&				getQApplication( void )						{ return m_QApp; }

	void						setCamCaptureRotation( uint32_t rot );
	int							getCamCaptureRotation( void )				{ return m_CamCaptureRotation; }

	void 						setAccountUserName( const char* name )		{ m_strAccountUserName = name; } 
	std::string					getAccountUserName( void )					{ return m_strAccountUserName; } 

	void						setIsMaxScreenSize(  bool isMessagerFrame, bool isFullSizeWindow );
	bool						getIsMaxScreenSize( bool isMessagerFrame );

	void						switchWindowFocus( QWidget* appIconButton );

	void						applySoundSettings( bool useDefaultsInsteadOfSettings = false );

	void						insertKeystroke( int keyNum );

	virtual void				okMessageBox( QString title, QString msg );
	virtual void				okMessageBox2( QString title, const char* msg, ... );
	virtual bool				yesNoMessageBox( QString title, QString msg );
	virtual bool				yesNoMessageBox2( QString title, const char* msg, ... );
	virtual void				errMessageBox( QString title, QString msg );
	virtual void				errMessageBox2( QString title, const char* msg, ... );

    void						wantToGuiHardwareCtrlCallbacks( ToGuiHardwareControlInterface* callback, bool wantCallback );

    //============================================================================
    //=== from gui audio callbacks ===//
    //============================================================================

    /// Mute/Unmute microphone
    virtual void				fromGuiMuteMicrophone( bool muteMic ) override;
    /// Returns true if microphone is muted
    virtual bool				fromGuiIsMicrophoneMuted( void ) override;
    /// Mute/Unmute speaker
    virtual void				fromGuiMuteSpeaker( bool muteSpeaker ) override;
    /// Returns true if speaker is muted
    virtual bool				fromGuiIsSpeakerMuted( void ) override;

    virtual void				fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnt, bool isSilence );
    virtual void				fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLen );

    void                        setMainWindow( QWidget* mainWindow ) { m_MainWindow = mainWindow; }
    void                        startupAppCommon( void );

protected:
    void                        registerMetaData( void );

    void                        shutdownAppCommon( void );

    void                        clearHardwareCtrlList( void );

    QWidget&                    getHomePage( void )                     { return *m_MainWindow; }

	//=== vars ===//
	QApplication&				m_QApp;
	EDefaultAppMode				m_AppDefaultMode;
    QWidget*                    m_MainWindow{ nullptr };

	AppSettings&				m_AppSettings;
    QString						m_AppShortName;
    QString						m_AppTitle;

	MyIcons					    m_MyIcons;
	VxAppTheme					m_AppTheme;
	VxAppStyle					m_AppStyle;
    VxAppDisplay				m_AppDisplay;

	SoundMgr&					m_SoundMgr;

	std::string					m_strAccountUserName;

	QTimer*					    m_OncePerSecondTimer;

	uint32_t					m_CamSourceId;
	uint32_t					m_CamCaptureRotation;

    bool	                    m_ToGuiActivityInterfaceBusy{ false };
    bool	                    m_ToGuiFileXferInterfaceBusy{ false };
    bool                        m_ToGuiHardwareCtrlBusy{ false };
    bool                        m_ToGuiUserUpdateClientBusy{ false };

    std::vector<ToGuiHardwareControlInterface*> m_ToGuiHardwareCtrlList;

	bool						m_LibraryActivityActive = false;
	bool						m_VidCaptureEnabled = false;
	bool						m_MicrophoneHardwareEnabled = false;
	bool						m_SpeakerHardwareEnabled = false;

    bool                        m_AppCommonInitialized = false;
    bool                        m_LoginBegin = false;
    bool                        m_LoginComplete = false;
    bool                        m_AppInitialized = false;
    QTimer *                    m_CheckSetupTimer = nullptr;

    bool                        m_IsMessengerReady{ false };
    bool                        m_IsLoggedOn{ false };
    bool                        m_IsSystemReady{ false };
    bool                        m_LoopbackMyselfTestAllowed{ false };
    bool                        m_GuiCpuTimeEnable{ false };

};

AppCommon& CreateAppInstance( QApplication* myApp );

AppCommon& GetAppInstance( void );

void DestroyAppInstance( );
#endif // defined(SANDBOX_APP)