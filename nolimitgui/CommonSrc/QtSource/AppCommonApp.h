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

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "AppDefs.h"
#include "AppGlobals.h"
#include "CamLogic.h"
#include "HomeWindow.h"
#include "FriendList.h"
#include "GuiConnectIdListMgr.h"
#include "GuiFavoriteMgr.h"
#include "GuiFileXferMgr.h"
#include "GuiGroupieListMgr.h"
#include "GuiHostedListMgr.h"
#include "GuiHostServerJoinMgr.h"
#include "GuiMemberActiveMgr.h"
#include "GuiOfferMgr.h"
#include "GuiPlayerMgr.h"
#include "GuiPluginMgr.h"
#include "GuiUserJoinMgr.h"
#include "GuiUserMgr.h"
#include "GuiThumbMgr.h"
#include "GuiWebPageMgr.h"
#include "MyIcons.h"
#include "VxAppTheme.h"
#include "VxAppStyle.h"
#include "VxAppDisplay.h" 

#include "ToGuiActivityInterface.h"
#include "ToGuiHardwareControlInterface.h"
#include "ToGuiUserUpdateInterface.h"
#include "ToGuiThumbUpdateInterface.h"

#include "GuiInterface/IToGui.h"
#include "GuiInterface/INlcRender.h"
#include "GuiInterface/INlcEvents.h"
#include "GuiInterface/IAudioInterface.h"

#include <ptop_src/ptop_engine_src/BlobXferMgr/BlobInfo.h>
#include <ptop_src/ptop_engine_src/Plugins/FileInfo.h>

class AccountMgr;

class ActivityAppSetup;
class ActivityBase;
class ActivityCreateAccount;
class ActivityShowHelp;
class ActivityOfferListDlg;

class AppletMultiMessenger;
class AppletDownloads;
class AppletMgr;
class AppletUploads;

class AppSettings;

class BlobInfo;
class FriendListEntryWidget;
class FileListReplySession;
class GuiFileXferSession;
class GuiOfferSession;
class KodiThread;
class MediaPlayerNlc;
class PopupMenu;
class RenderGlWidget;

class VxPeerMgr;
class VxTilePositioner;

// media
class CRenderBuffer;

class AppCommon : public QWidget, public IToGui, public INlcRender, public INlcEvents, public IAudioRequests, public IAudioCallbacks
{
    Q_OBJECT

public:
    AppCommon( QApplication& myQApp,
               EDefaultAppMode appDefaultMode,
               AppSettings& appSettings,
               AccountMgr& myDataHelper,
               INlc& nlc );
    AppCommon( const AppCommon& rhs ) = delete;
    virtual ~AppCommon() override = default;

    // elapsed high resolution timer 
    int64_t                     elapsedMilliseconds( void );

    int64_t                     elapsedSeconds( void ) { return elapsedMilliseconds() / 1000; }

    // load profile and icons etc without using thread to avoid linux crash
    void                        loadWithoutThread( void );
    // cannot launch any applets until logon is completed
    void                        setLoginCompleted( bool completed ) { m_LoginComplete = completed; }
    bool                        getLoginCompleted( void ) { return m_LoginComplete; }

    // some applets cannot be launched until application is fully ready for network use
    void                        setIsAppInitialized( bool initialized ) { m_AppInitialized = initialized; }
    bool                        getIsAppInitialized( void ) { return m_AppInitialized; }

    // diagnose to much cpu usage in gui thread
    void                        setGuiCpuTimeEnable( bool enable ) { m_GuiCpuTimeEnable = enable; }
    bool                        getGuiCpuTimeEnable( void ) { return m_GuiCpuTimeEnable; }

    AccountMgr&                 getAccountMgr( void ) { return m_AccountMgr; }
    VxAppDisplay&               getAppDisplay( void ) { return m_AppDisplay; }
    AppGlobals&                 getAppGlobals( void ) { return m_AppGlobals; }
    QFrame*                     getAppletFrame( EApplet applet );
    AppletMgr&                  getAppletMgr( void ) { return m_AppletMgr; }
    AppSettings&                getAppSettings( void ) { return m_AppSettings; }
    QString&                    getAppShortName( void ) { return m_AppShortName; }
    VxAppStyle&                 getAppStyle( void ) { return m_AppStyle; }
    QString&                    getAppTitle( void ) { return m_AppTitle; }
    VxAppTheme&                 getAppTheme( void ) { return m_AppTheme; }
    CamLogic&                   getCamLogic( void ) { return m_CamLogic; }
    P2PEngine&                  getEngine( void );
    IFromGui&                   getFromGuiInterface( void );
    INlc&                       getNlc( void ) { return m_Nlc; }
    HomeWindow&                 getHomePage( void ) { return m_HomePage; }
    bool						getIsVidCaptureEnabled( void ) { return m_VidCaptureEnabled; }
    bool						getIsMicrophoneHardwareEnabled( void ) { return m_MicrophoneHardwareEnabled; }
    bool						getIsSpeakerHardwareEnabled( void ) { return m_SpeakerHardwareEnabled; }
    MyIcons&                    getMyIcons( void ) { return m_MyIcons; }
    VxNetIdent*                 getMyNetIdent( void );
    VxGUID&                     getMyOnlineId( void );
    ENetworkStateType			getNetworkState( void ) { return m_LastNetworkState; }
    GuiOfferMgr&                getOfferMgr( void ) { return m_OfferMgr; }

    RenderGlWidget*             getRenderConsumer( void );
    SoundMgr&                   getSoundMgr( void ) { return m_SoundMgr; }
    VxTilePositioner&           getTilePositioner( void ) { return m_TilePositioner; }
    GuiConnectIdListMgr&        getConnectIdListMgr( void ) { return m_ConnectIdListMgr; }
    GuiFavoriteMgr&             getFavoriteMgr( void ) { return m_FavoriteMgr; }
    GuiFileXferMgr&             getFileXferMgr( void ) { return m_FileXferMgr; }
    GuiGroupieListMgr&          getGroupieListMgr( void ) { return m_GroupieListMgr; }
    GuiHostedListMgr&           getHostedListMgr( void ) { return m_HostedListMgr; }
    GuiHostJoinMgr&             getHostJoinMgr( void ) { return m_HostJoinMgr; }
    GuiUserJoinMgr&             getUserJoinMgr( void ) { return m_UserJoinMgr; }
    GuiWebPageMgr&              getWebPageMgr( void ) { return m_WebPageMgr; }
    GuiUserMgr&                 getUserMgr( void ) { return m_UserMgr; }
    GuiThumbMgr&                getThumbMgr( void ) { return m_ThumbMgr; }
    GuiMemberActiveMgr&  getMemberActiveMgr( void ) { return m_MemberActiveMgr; }
    GuiPlayerMgr&               getPlayerMgr( void ) { return m_PlayerMgr; }
    GuiPluginMgr&               getPluginMgr( void ) { return m_PluginMgr; }

    MediaPlayerNlc&             getPlayerNlc( void );

    QApplication&               getQApplication( void ) { return m_QApp; }

    void						setCamCaptureRotation( uint32_t rot );
    int							getCamCaptureRotation( void ) { return m_CamCaptureRotation; }

    void 						setAccountUserName( const char* name ) { m_strAccountUserName = name; }
    std::string					getAccountUserName( void ) { return m_strAccountUserName; }

    void						setIsMaxScreenSize( bool isMessagerFrame, bool isFullSizeWindow );
    bool						getIsMaxScreenSize( bool isMessagerFrame );

    // permanent applets for lifetime of application
    void						setAppletMultiMessenger( AppletMultiMessenger* applet ) { m_AppletMultiMessenger = applet; }
    AppletMultiMessenger*       getAppletMultiMessenger( void ) { return m_AppletMultiMessenger; };
    void						setAppletDownloads( AppletDownloads* applet ) { m_AppletDownloads = applet; }
    AppletDownloads*            getAppletDownloads( void ) { return m_AppletDownloads; };
    void						setAppletUploads( AppletUploads* applet ) { m_AppletUploads = applet; }
    AppletUploads*              getAppletUploads( void ) { return m_AppletUploads; };

    bool                        hasExistingAccount( void );

    void						switchWindowFocus( QWidget* appIconButton );

    void						applySoundSettings( bool useDefaultsInsteadOfSettings = false );
    void						playSound( ESndDef sndDef );
    void						insertKeystroke( int keyNum );

    void						forceOrientationChange( void );

    virtual void				okMessageBox( QString title, QString msg );
    virtual void				okMessageBox2( QString title, const char* msg, ... );
    virtual bool				yesNoMessageBox( QString title, QString msg );
    virtual bool				yesNoMessageBox2( QString title, const char* msg, ... );
    virtual void				errMessageBox( QString title, QString msg );
    virtual void				errMessageBox2( QString title, const char* msg, ... );

    //=== app methods ===//
    virtual void				startupAppCommon( QFrame* appletFrame, QFrame* messangerFrame );

    virtual void				doLogin( void );
    virtual void				doLoginStep2( void );
    virtual void				completeLogin( void );

    // prompt user to confirm wants to shutdown app.. caller must call appCommonShutdown if answer is yes
    virtual bool				confirmAppShutdown( QWidget* parentWindow );
    virtual void				shutdownAppCommon( void );

    void						loadAccountSpecificSettings( const char* userName );
    void                        copyAssetsToFoldersIfRequired( void );

    ActivityBase*               launchApplet( EApplet applet, QWidget* parent );
    ActivityBase*               launchApplet( EApplet applet, QWidget* parent, QString launchParam, VxGUID& assetId );

    bool						launchOfferSendApplet( EPluginType pluginType, GuiUser* guiUser, QWidget* parent = nullptr );
    bool                        launchOfferSendSession( EPluginType pluginType, GuiUser* guiUser, GuiOfferSession* existingOffer, QWidget* parent = nullptr );
    bool                        launchOfferResponseAccept( GuiOfferSession* offerSession, QWidget* contentFrame = nullptr );

    void						activityStateChange( ActivityBase* activity, bool isCreated );

    void						setIsLibraryActivityActive( bool isActive ) { m_LibraryActivityActive = isActive; }
    bool						getIsLibraryActivityActive( void ) { return m_LibraryActivityActive; }

    void						wantToGuiActivityCallbacks( ToGuiActivityInterface* callback, bool wantCallback );
    void						wantToGuiHardwareCtrlCallbacks( ToGuiHardwareControlInterface* callback, bool wantCallback );
    void						wantToGuiUserUpdateCallbacks( ToGuiUserUpdateInterface* callback, bool	wantCallback );

    //============================================================================
    //=== to palayer-nlc lib events ===//
    //============================================================================
    virtual void                fromGuiKeyPressEvent( EAppModule appModule, int key, int mod ) override;
    virtual void                fromGuiKeyReleaseEvent( EAppModule appModule, int key, int mod ) override;

    virtual void                fromGuiMousePressEvent( EAppModule appModule, int mouseXPos, int mouseyPos, int mouseButton ) override;
    virtual void                fromGuiMouseReleaseEvent( EAppModule appModule, int mouseXPos, int mouseyPos, int mouseButton ) override;
    virtual void                fromGuiMouseMoveEvent( EAppModule appModule, int mouseXPos, int mouseyPos ) override;

    virtual void                fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight ) override;
    virtual void                fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight ) override;
    virtual void                fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight ) override;

    virtual void                fromGuiCloseEvent( EAppModule appModule ) override;
    virtual void                fromGuiVisibleEvent( EAppModule appModule, bool isVisible ) override;

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

    //============================================================================
    //=== to gui media/render ===//
    //============================================================================

    void                        toGuiMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal = 0, const char* fileName = "" ) override;
    void                        toGuiMediaError( EAppModule appModule, EMediaError mediaError, const char* msg ) override;

    virtual void                verifyGlState( const char* msg = nullptr ) override; // show gl error if any

    //=== textures ===//
    void                        setActiveGlTexture( unsigned int activeTextureNum = 0 /* 0 == GL_TEXTURE0 , 1 == GL_TEXTURE1 etc*/ ) override;

    void                        createTextureObject( CTextureQt* texture ) override;
    void                        destroyTextureObject( CTextureQt* texture ) override;
    bool                        loadToGPU( CTextureQt* texture ) override;
    void                        bindToUnit( CTextureQt* texture, unsigned int unit ) override;

    void                        beginGuiTexture( CGUITextureQt* guiTexture, NlcColor color ) override;
    void                        drawGuiTexture( CGUITextureQt* guiTexture, float* x, float* y, float* z, const NlcRect& texture, const NlcRect& diffuse, int orientation ) override;
    void                        endGuiTexture( CGUITextureQt* guiTexture ) override;
    void                        drawQuad( const NlcRect& rect, NlcColor color, CTextureBase* texture, const NlcRect* texCoords ) override;

    bool                        firstBegin( CGUIFontTTFQt* font )  override;
    void                        lastEnd( CGUIFontTTFQt* font ) override;
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    CVertexBuffer               createVertexBuffer( CGUIFontTTFQt* font, const std::vector<SVertex>& vertices )  override;
#endif // ENABLE_KODI
    void                        destroyVertexBuffer( CGUIFontTTFQt* font, CVertexBuffer& buffer )  override;

    virtual void                deleteHardwareTexture( CGUIFontTTFQt* font )  override;
    virtual void                createStaticVertexBuffers( CGUIFontTTFQt* font )  override;
    virtual void                destroyStaticVertexBuffers( CGUIFontTTFQt* font )  override;

    //=== render ===//
    void                        captureScreen( CScreenshotSurface* screenCaptrue, NlcRect& captureArea ) override;

    void                        toGuiRenderVideoFrame( int textureIdx, CRenderBuffer* videoBuffer );
    bool                        initRenderSystem() override;
    bool                        destroyRenderSystem() override;
    bool                        resetRenderSystem( int width, int height ) override;

    int                         getMaxTextureSize() override;

    bool                        beginRender() override;
    bool                        endRender() override;
    void                        presentRender( bool rendered, bool videoLayer ) override;
    bool                        clearBuffers( NlcColor color ) override;
    bool                        isExtSupported( const char* extension ) override;

    void                        setVSync( bool vsync ) override;
    void                        resetVSync() override {  }

    void                        setViewPort( const NlcRect& viewPort ) override;
    void                        getViewPort( NlcRect& viewPort ) override;

    bool                        scissorsCanEffectClipping() override;
    NlcRect                     clipRectToScissorRect( const NlcRect& rect ) override;
    void                        setScissors( const NlcRect& rect ) override;
    void                        resetScissors() override;

    void                        captureStateBlock() override;
    void                        applyStateBlock() override;

    void                        setCameraPosition( const NlcPoint& camera, int screenWidth, int screenHeight, float stereoFactor = 0.0f ) override;

    void                        applyHardwareTransform( const TransformMatrix& matrix ) override;
    void                        restoreHardwareTransform() override;
    bool                        supportsStereo( RENDER_STEREO_MODE mode ) const override { return false; }

    bool                        testRender() override;

    void                        project( float& x, float& y, float& z ) override;

    //=== shaders ===//
    std::string                 getShaderPath( const std::string& filename ) override { return ""; }

    void                        initialiseShaders() override;
    void                        releaseShaders() override;
    bool                        enableShader( ESHADERMETHOD method ) override;
    bool                        isShaderValid( ESHADERMETHOD method ) override;
    void                        disableShader( ESHADERMETHOD method ) override;
    void                        disableGUIShader() override;

    int                         shaderGetPos()  override;
    int                         shaderGetCol()  override;
    int                         shaderGetModel()  override;
    int                         shaderGetCoord0()  override;
    int                         shaderGetCoord1()  override;
    int                         shaderGetUniCol()  override;

    // yuv shader
    virtual void                shaderSetField( ESHADERMETHOD shader, int field )   override;
    virtual void                shaderSetWidth( ESHADERMETHOD shader, int w )   override;
    virtual void                shaderSetHeight( ESHADERMETHOD shader, int h )  override;

    virtual void                shaderSetBlack( ESHADERMETHOD shader, float black ) override;
    virtual void                shaderSetContrast( ESHADERMETHOD shader, float contrast ) override;
    virtual void                shaderSetConvertFullColorRange( ESHADERMETHOD shader, bool convertFullRange ) override;

    virtual int                 shaderGetVertexLoc( ESHADERMETHOD shader ) override;
    virtual int                 shaderGetYcoordLoc( ESHADERMETHOD shader ) override;
    virtual int                 shaderGetUcoordLoc( ESHADERMETHOD shader ) override;
    virtual int                 shaderGetVcoordLoc( ESHADERMETHOD shader ) override;

    virtual void                shaderSetMatrices( ESHADERMETHOD shader, const float* p, const float* m ) override;
    virtual void                shaderSetAlpha( ESHADERMETHOD shader, float alpha ) override;

    virtual void                shaderSetFlags( ESHADERMETHOD shader, unsigned int flags ) override;
    virtual void                shaderSetFormat( ESHADERMETHOD shader, EShaderFormat format ) override;
    virtual void                shaderSourceTexture( ESHADERMETHOD shader, int ytex ) override;
    virtual void                shaderSetStepX( ESHADERMETHOD shader, float stepX ) override;
    virtual void                shaderSetStepY( ESHADERMETHOD shader, float stepY )  override;

    // filter shader
    virtual bool                shaderGetTextureFilter( ESHADERMETHOD shader, int& filter ) override;
    virtual int                 shaderGetcoordLoc( ESHADERMETHOD shader ) override;

    // renderqt
    virtual int                 shaderVertexAttribPointer( ESHADERMETHOD shader, unsigned int index, int size, int type, bool normalized, int stride, const void* pointer ) override;
    virtual void                shaderEnableVertexAttribArray( ESHADERMETHOD shader, int arrayId ) override;
    virtual void                shaderDisableVertexAttribArray( ESHADERMETHOD shader, int arrayId ) override;

    // frame buffers
    virtual void                frameBufferGen( int bufCount, unsigned int* fboId ) override;
    virtual void                frameBufferDelete( int bufCount, unsigned int* fboId ) override;
    virtual void                frameBufferTexture2D( int target, unsigned int texureId )  override;
    virtual void                frameBufferBind( unsigned int fboId ) override;
    virtual bool                frameBufferStatus() override;

    // gl functions
    void                        glFuncDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices ) override;
    void                        glFuncDisable( GLenum cap ) override;
    void                        glFuncBindTexture( GLenum target, GLuint texture ) override;
    void                        glFuncViewport( GLint x, GLint y, GLsizei width, GLsizei height ) override;
    void                        glFuncScissor( GLint x, GLint y, GLsizei width, GLsizei height ) override;

    virtual void                glFuncGenTextures( GLsizei n, GLuint* textures ) override;
    virtual void                glFuncDeleteTextures( GLsizei n, const GLuint* textures ) override;
    virtual void                glFuncTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels ) override;
    virtual void                glFuncTexParameteri( GLenum target, GLenum pname, GLint param ) override;
    virtual void                glFuncReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels ) override;
    virtual void                glFuncPixelStorei( GLenum pname, GLint param ) override;
    virtual void                glFuncFinish() override;

    virtual void                glFuncEnable( GLenum cap ) override;
    virtual void                glFuncTexSubImage2D( GLenum target, GLint level,
                                                     GLint xoffset, GLint yoffset,
                                                     GLsizei width, GLsizei height,
                                                     GLenum format, GLenum type,
                                                     const GLvoid* pixels ) override;
    virtual void                glFuncBlendFunc( GLenum sfactor, GLenum dfactor ) override;
    virtual void                glFuncVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer ) override;
    virtual void                glFuncDisableVertexAttribArray( GLuint index ) override;
    virtual void                glFuncEnableVertexAttribArray( GLuint index ) override;
    virtual void                glFuncDrawArrays( GLenum mode, GLint first, GLsizei count ) override;


    //============================================================================
    //=== end to gui media/render ===//
    //============================================================================



    //============================================================================
    //=== to gui ===//
    //============================================================================

    virtual void                toGuiSetIsAppModuleRunning( EAppModule appModule, bool isRunning ) override;
    virtual bool                toGuiGetIsAppModuleRunning( EAppModule appModule ) override;
    virtual bool                toGuiRunModule( EAppModule appModule ) override;

    virtual void                toGuiCreateUserDirs( void ) override;

    virtual void				toGuiPlayNlcMedia( AssetBaseInfo* assetInfo ) override;
    virtual void				toGuiLog( int logFlags, const char* pMsg ) override;
    virtual void				toGuiAppErr( EAppErr eAppErr, const char* errMsg = "" ) override;
    virtual void				toGuiStatusMessage( const char* errMsg ) override;
    // NOTE: toGuiUserMessage should be called from in gui on gui thread only
    virtual void				toGuiUserMessage( const char* userMsg, ... );
    virtual void				toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, std::string& paramMsg ) override;
    virtual void				toGuiPluginCommError( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, ECommErr commErr ) override;

    /// Send Network available status to GUI for display
    virtual void				toGuiNetAvailableStatus( ENetAvailStatus eNetAvailStatus ) override;
    virtual void				toGuiNetworkState( ENetworkStateType eNetworkState, const char* stateMsg = "" ) override;

    virtual void				toGuiHostAnnounceStatus( EHostType hostType, VxGUID& sessionId, EHostAnnounceStatus joinStatus, const char* msg = "" ) override;
    virtual void				toGuiHostJoinStatus( EHostType hostType, VxGUID& sessionId, EHostJoinStatus joinStatus, const char* msg = "" ) override;

    virtual void				toGuiHostSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr = eCommErrNone, const char* msg = "" ) override;
    virtual void				toGuiHostSearchResult( EHostType hostType, VxGUID& sessionId, HostedInfo& hostedInfo ) override;
    virtual void				toGuiHostSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				toGuiGroupieSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr = eCommErrNone, const char* msg = "" ) override;
    virtual void				toGuiGroupieSearchResult( EHostType hostType, VxGUID& sessionId, GroupieInfo& hostedInfo ) override;
    virtual void				toGuiGroupieSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				toGuiIsPortOpenStatus( EIsPortOpenStatus eIsPortOpenStatus, const char* msg = "" ) override;
    virtual void				toGuiRunTestStatus( const char* testName, ERunTestStatus eRunTestStatus, const char* msg = "" ) override;
    virtual void				toGuiRandomConnectStatus( ERandomConnectStatus eRandomConnectStatus, const char* msg = "" ) override;

    // return true if any microphone device is available to be enabled
    virtual bool				toGuiIsMicrophoneDeviceAvailable( void ) override;

    virtual void				toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput ) override;
    // enable disable microphone for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput ) override;

    virtual void				toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput ) override;
    // enable disable speaker for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput ) override;

    virtual int				    toGuiModuleAudioFrame( EAppModule appModule, int16_t* pu16PcmData, int pcmDataLenInBytes, bool isSilence ) override;
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    virtual int				    toGuiPlayerNlcAudio( EAppModule appModule, float* audioDataFload, int audioDataLenInBytes ) override;

    virtual float               toGuiGetAudioDelaySeconds( EAppModule appModule ) override;

    virtual float               toGuiGetAudioCacheFreeSpace( EAppModule appModule ) override;

    virtual float               toGuiGetAudioCacheTotalSeconds( EAppModule appModule ) override;
#endif // ENABLE_KODI

    virtual void				toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture ) override;
    virtual void				toGuiPlayVideoFrame( VxGUID& onlineId, uint8_t* pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 ) override;
    virtual int				    toGuiPlayVideoFrame( VxGUID& onlineId, uint8_t* picBuf, uint32_t picBufLen, int picWidth, int picHeight ) override;

    // user update interface
    virtual void				toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    virtual void				toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override;

    virtual void				toGuiContactAdded( VxNetIdent* netIdent ) override;
    virtual void				toGuiContactRemoved( VxGUID& onlineId ) override;

    virtual void				toGuiContactOnline( VxNetIdent* netIdent ) override;

    //virtual void				toGuiContactNameChange( VxNetIdent* netIdent ) override;
    //virtual void				toGuiContactDescChange( VxNetIdent* netIdent ) override;
    //virtual void				toGuiContactFriendshipChange( VxNetIdent* netIdent ) override;

    //virtual void				toGuiPluginPermissionChange( VxNetIdent* netIdent ) override;
    //virtual void				toGuiContactSearchFlagsChange( VxNetIdent* netIdent ) override;

    virtual void				toGuiContactAnythingChange( VxNetIdent* netIdent ) override;
    virtual void				toGuiContactLastSessionTimeChange( VxNetIdent* netIdent ) override;

    virtual void				toGuiUpdateMyIdent( VxNetIdent* netIdent ) override;
    virtual void				toGuiSaveMyIdent( VxNetIdent* netIdent ) override;

    virtual void				toGuiRxedPluginOffer( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;
    virtual void				toGuiRxedOfferReply( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;

    virtual void				toGuiPluginSessionEnded( VxNetIdent* netIdent, EPluginType pluginType, VxGUID& lclSessionId ) override;

    virtual void				toGuiPluginStatus( EPluginType		pluginType,
                                                   int				statusType,
                                                   int				statusValue ) override;

    virtual void				toGuiInstMsg( VxNetIdent* netIdent, EPluginType	pluginType, const char* pMsg ) override;


    virtual void				toGuiSetGameValueVar( EPluginType	    pluginType,
                                                      VxGUID& onlineId,
                                                      int32_t			s32VarId,
                                                      int32_t			s32VarValue ) override;

    virtual void				toGuiSetGameActionVar( EPluginType	    pluginType,
                                                       VxGUID& onlineId,
                                                       int32_t			s32VarId,
                                                       int32_t			s32VarValue ) override;

    //=== to gui file ===//
    virtual void				toGuiFileListReply( VxGUID& onlineId, EPluginType pluginType, FileInfo& fileInfo ) override;

    virtual void				toGuiFileUploadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) override;

    virtual void				toGuiFileDownloadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) override;

    virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSession, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ) override;
    virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError ) override;
    virtual void				toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError ) override;

    virtual void				toGuiFileList( FileInfo& fileInfo ) override;
    virtual void				toGuiFileListCompleted( void ) override;
    //=== to gui search ===//
    virtual void				toGuiScanResultSuccess( EScanType eScanType, VxNetIdent* netIdent ) override;
    virtual void				toGuiSearchResultError( EScanType eScanType, VxNetIdent* netIdent, int errCode ) override;
    virtual void				toGuiScanSearchComplete( EScanType eScanType ) override;

    virtual void				toGuiSearchResultProfilePic( VxNetIdent* netIdent, uint8_t* pu8JpgData, uint32_t u32JpgDataLen ) override;

    virtual void				toGuiSearchResultFileSearch( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) override;

    //=== to gui asset ===//
    virtual void				toGuiAssetAdded( AssetBaseInfo* assetInfo ) override;
    virtual void				toGuiAssetUpdated( AssetBaseInfo* assetInfo ) override;
    virtual void				toGuiAssetRemoved( AssetBaseInfo* assetInfo ) override;

    virtual void				toGuiAssetXferState( VxGUID& assetUniqueId, EAssetSendState assetSendState, int param ) override;

    virtual void				toGuiAssetSessionHistory( AssetBaseInfo* assetInfo ) override;
    virtual void				toGuiAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;
    virtual void				toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000 ) override;

    //=== to gui host list ===//
    virtual void				toGuiBlobAdded( BlobInfo* assetInfo ) override;
    virtual void				toGuiBlobSessionHistory( BlobInfo* assetInfo ) override;
    virtual void				toGuiBlobAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

    virtual void				toGuiPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus ) override;
    /// a module has changed state
    virtual void				toGuiModuleState( EAppModule moduleNum, EModuleState moduleState )  override;

    virtual void				toGuiNetworkIsTested( bool requiresRelay, std::string& ipAddr, uint16_t ipPort )  override;

    //============================================================================
    //=== implementation ===//
    //============================================================================

    bool						userCanceled( void );

    // returns true if showed activity
    bool 						offerToFriendPluginSession( GuiUser* guiUser, EPluginType pluginType, bool inGroup = false, QWidget* parent = nullptr );
    void						offerToFriendSendFile( GuiUser* guiUser, bool inGroup = false, QWidget* parent = nullptr );

    void						createAccountForUser( std::string& strUserName, VxNetIdent& userAccountIdent, const char* moodMsg, int gender,
                                                      EAgeType age, int primaryLanguage, int contentType );
    void                        setupAccountResources( VxNetIdent& userAccountIdent );
    // updates my ident in database and engine and global ident
    void                        updateMyIdent( VxNetIdent* myIdent, bool permissionAndStateOnly = false );

    std::string					getUserXferDirectoryFromAccountUserName( const char* userName );
    std::string 				getUserSpecificDataDirectoryFromAccountUserName( const char* userName );

    void						refreshFriend( VxGUID& onlineId ); // called to emit signalRefreshFriend
    bool						loadLastUserAccount( void );
    void                        onMessengerReady( bool isReady );
    bool                        isMessengerReady( void ) { return m_IsMessengerReady; }
    void                        onUserLoggedOn( void );
    bool                        checkSystemReady( void );

    std::string                 describeGroupieId( GroupieId& groupieId );
    std::string                 describeUser( VxGUID& onlineId );

    void                        doOnlineStatusChange( VxGUID onlineId, bool isOnline ); // called on gui thread by GuiConnectIdListMgr

signals:
    void						signalMessengerReady( bool isReady );    // emitted when messenger ready state changes
    void						signalMainWindowResized( void );    // emitted if main window is resized
    void						signalMainWindowMoved( void );      // emitted if main window is moved

    void						signalFinishedLoadingGui( void );
    void						signalFinishedLoadingEngine( void );
    void						signalPlaySound( ESndDef sndDef );
    void						signalLog( int iPluginNum, QString strMsg );
    void						signalAppErr( EAppErr eAppErr, QString errMsg );
    void						signalStatusMsg( QString strMsg );
    void						signalUserMsg( QString strMsg );

    void						signalHostAnnounceStatus( EHostType hostType, VxGUID sessionId, EHostAnnounceStatus hostStatus, QString strMsg );
    void						signalHostJoinStatus( EHostType hostType, VxGUID sessionId, EHostJoinStatus hostStatus, QString strMsg );

    void						signalHostSearchStatus( EHostType hostType, VxGUID sessionId, EHostSearchStatus hostStatus, QString strMsg );
    void						signalGroupieSearchStatus( EHostType hostType, VxGUID sessionId, EHostSearchStatus hostStatus, QString strMsg );

    void						signalIsPortOpenStatus( EIsPortOpenStatus eIsPortOpenStatus, QString strMsg );
    void						signalRunTestStatus( QString testName, ERunTestStatus eRunTestStatus, QString strMsg );
    void						signalRandomConnectStatus( ERandomConnectStatus eRandomConnectStatus, QString strMsg );
    void						signalNetworkStateChanged( ENetworkStateType eNetworkState );
    void						signalNetAvailStatus( ENetAvailStatus eNetAvailStatus );

    void						signalRefreshFriend( VxGUID onlineId ); // emitted if friend has changed
    void						signalAssetViewMsgAction( EAssetAction, VxGUID onlineId, int pos0to100000 );
    void						signalBlobViewMsgAction( EAssetAction, VxGUID onlineId, int pos0to100000 );

    void						signalToGuiInstMsg( GuiUser* guiUser, EPluginType pluginType, QString pMsg );

    void						signalMicrophonePeak( int peekVal0to32768 );

    void						signalInternalWantMicrophoneRecording( EAppModule appModule, bool enableMicInput );
    void                        signalInternalWantUserVoiceMicrophone( EAppModule appModule, VxGUID onlineId, bool wantMicInput );

    void						signalInternalWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput );
    void						signalInternalWantUserVoiceSpeaker( EAppModule appModule, VxGUID onlineId, bool wantSpeakerOutput );

    void						signalInternalWantVideoCapture( EAppModule appModule, bool enableCapture );

    void						signalSetRelayHelpButtonVisibility( bool isVisible );

    void                        signalSystemReady( bool isReady );

    void						signalInternalToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue );

    void                        signalInternalNetAvailStatus( ENetAvailStatus netAvailStatus );
    void                        signalInternalPluginMessage( EPluginType pluginType, VxGUID onlineId, EPluginMsgType msgType, QString paramValue );
    void                        signalInternalPluginErrorMsg( EPluginType pluginType, VxGUID onlineId, EPluginMsgType msgType, ECommErr commError );

    void                        signalInternalToGuiFileDownloadStart( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo );
    void                        signalInternalToGuiFileDownloadComplete( EPluginType pluginType, VxGUID lclSessionId, QString fileName, EXferError xferError );
    void                        signalInternalToGuiFileUploadStart( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo );
    void                        signalInternalToGuiFileUploadComplete( EPluginType pluginType, VxGUID lclSessionId, QString fileName, EXferError xferError );
    void                        signalInternalToGuiFileXferState( EPluginType pluginType, VxGUID lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 );
    void                        signalInternalToGuiFileList( FileInfo fileInfo );
    void                        signalInternalToGuiFileListCompleted( void );

    void                        signalInternalToGuiSetGameValueVar( EPluginType pluginType, VxGUID onlineId, int32_t s32VarId, int32_t s32VarValue );
    void                        signalInternalToGuiSetGameActionVar( EPluginType pluginType, VxGUID onlineId, int32_t s32VarId, int32_t s32VarValue );

    void				        signalInternalToGuiAssetAdded( AssetBaseInfo assetInfo );
    void				        signalInternalToGuiAssetUpdated( AssetBaseInfo assetInfo );
    void				        signalInternalToGuiAssetRemoved( AssetBaseInfo assetInfo );

    void				        signalInternalToGuiAssetXferState( VxGUID assetUniqueId, EAssetSendState assetSendState, int param );

    void				        signalInternalToGuiAssetSessionHistory( AssetBaseInfo* assetInfo );
    void				        signalInternalToGuiAssetAction( EAssetAction assetAction, VxGUID assetId, int pos0to100000 );

    void                        signalInternalMultiSessionAction( VxGUID onlineId, EMSessionAction mSessionAction, int pos0to100000 );

    void                        signalInternalBlobAction( EAssetAction assetAction, VxGUID assetId, int pos0to100000 );
    void                        signalInternalBlobAdded( BlobInfo blobInfo );
    void                        signalInternalBlobSessionHistory( BlobInfo blobInfo );

    void                        signalInternalToGuiIndentListUpdate( EUserViewType listType, VxGUID onlineId, uint64_t timestamp );
    void                        signalInternalToGuiIndentListRemove( EUserViewType listType, VxGUID onlineId );

    void                        signalInternalToGuiContactAdded( VxNetIdent netIdent );
    void                        signalInternalToGuiContactRemoved( VxGUID onlineId );

    void                        signalInternalToGuiContactOnline( VxNetIdent netIdent );

    void                        signalInternalToGuiContactUpdated( VxNetIdent netIdent );

    void                        signalInternalToGuiContactLastSessionTimeChange( VxNetIdent netIdent );

    void                        signalInternalToGuiUpdateIdent( VxNetIdent netIdent );
    void                        signalInternalToGuiSaveMyIdent( VxNetIdent netIdent );

    void                        signalInternalToGuiScanSearchComplete( EScanType eScanType );
    void                        signalInternalToGuiScanResultSuccess( EScanType eScanType, VxNetIdent netIdent );
    void                        signalInternalToGuiSearchResultError( EScanType eScanType, VxNetIdent netIdent, int errCode );

    void                        signalInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus );

    void                        signalInternalNetworkIsTested( bool requiresRelay, QString ipAddr, uint16_t ipPort );

    void                        signalInternalPlayNlcMedia( AssetBaseInfo assetInfo );

    void                        signalInternalToGuiSearchResultFileSearch( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo );
    void                        signalInternalToGuiFileListReply( VxGUID onlineId, EPluginType pluginType, FileInfo fileInfo );

    void                        signalInternalToGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo offerInfo );
    void                        signalInternalToGuiRxedOfferReply( VxGUID onlineId, OfferBaseInfo offerInfo );

    void                        signalInternalToGuiPluginSessionEnded( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId );

    void                        signalInternalMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal, QString fileName );
    void                        signalInternalMediaError( EAppModule appModule, EMediaError mediaError, QString msg );

private slots:
    void						slotInternalToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue );

    void                        slotInternalNetAvailStatus( ENetAvailStatus netAvailStatus );
    void                        slotInternalPluginMessage( EPluginType pluginType, VxGUID onlineId, EPluginMsgType msgType, QString paramValue );
    void                        slotInternalPluginErrorMsg( EPluginType pluginType, VxGUID onlineId, EPluginMsgType msgType, ECommErr commError );

    void                        slotInternalToGuiFileDownloadStart( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo );
    void                        slotInternalToGuiFileDownloadComplete( EPluginType pluginType, VxGUID lclSessionId, QString fileName, EXferError xferError );
    void                        slotInternalToGuiFileUploadStart( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo );
    void                        slotInternalToGuiFileUploadComplete( EPluginType pluginType, VxGUID lclSessionId, QString fileName, EXferError xferError );
    void                        slotInternalToGuiFileXferState( EPluginType pluginType, VxGUID lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 );
    void                        slotInternalToGuiFileList( FileInfo fileInfo );
    void                        slotInternalToGuiFileListCompleted( void );

    void                        slotInternalToGuiSetGameValueVar( EPluginType pluginType, VxGUID onlineId, int32_t s32VarId, int32_t s32VarValue );
    void                        slotInternalToGuiSetGameActionVar( EPluginType pluginType, VxGUID onlineId, int32_t s32VarId, int32_t s32VarValue );

    void				        slotInternalToGuiAssetAdded( AssetBaseInfo assetInfo );
    void				        slotInternalToGuiAssetUpdated( AssetBaseInfo assetInfo );
    void				        slotInternalToGuiAssetRemoved( AssetBaseInfo assetInfo );

    void				        slotInternalToGuiAssetXferState( VxGUID assetUniqueId, EAssetSendState assetSendState, int param );

    void				        slotInternalToGuiAssetSessionHistory( AssetBaseInfo* assetInfo );
    void				        slotInternalToGuiAssetAction( EAssetAction assetAction, VxGUID assetId, int pos0to100000 );

    void                        slotInternalMultiSessionAction( VxGUID onlineId, EMSessionAction mSessionAction, int pos0to100000 );

    void                        slotInternalBlobAction( EAssetAction assetAction, VxGUID assetId, int pos0to100000 );
    void                        slotInternalBlobAdded( BlobInfo blobInfo );
    void                        slotInternalBlobSessionHistory( BlobInfo blobInfo );

    void                        slotInternalToGuiIndentListUpdate( EUserViewType listType, VxGUID onlineId, uint64_t timestamp );
    void                        slotInternalToGuiIndentListRemove( EUserViewType listType, VxGUID onlineId );

    void                        slotInternalToGuiContactAdded( VxNetIdent netIdent );
    void                        slotInternalToGuiContactRemoved( VxGUID onlineId );

    void                        slotInternalToGuiContactOnline( VxNetIdent netIdent );

    void                        slotInternalToGuiContactUpdated( VxNetIdent netIdent );

    void                        slotInternalToGuiContactLastSessionTimeChange( VxNetIdent netIdent );

    void                        slotInternalToGuiUpdateIdent( VxNetIdent netIdent );
    void                        slotInternalToGuiSaveMyIdent( VxNetIdent netIdent );

    void                        slotInternalToGuiScanSearchComplete( EScanType eScanType );
    void                        slotInternalToGuiScanResultSuccess( EScanType eScanType, VxNetIdent netIdent );
    void                        slotInternalToGuiSearchResultError( EScanType eScanType, VxNetIdent netIdent, int errCode );

    void						slotInternalWantMicrophoneRecording( EAppModule appModule, bool wantMicInput );
    void                        slotInternalWantUserVoiceMicrophone( EAppModule appModule, VxGUID onlineId, bool wantMicInput );

    void						slotInternalWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput );
    void						slotInternalWantUserVoiceSpeaker( EAppModule appModule, VxGUID onlineId, bool wantSpeakerOutput );

    void						slotInternalWantVideoCapture( EAppModule appModule, bool enableCapture );

    void                        slotInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus );

    void                        slotInternalNetworkIsTested( bool requiresRelay, QString ipAddr, uint16_t ipPort );

    void                        slotInternalPlayNlcMedia( AssetBaseInfo assetInfo );

    void                        slotInternalToGuiSearchResultFileSearch( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId, FileInfo fileInfo );
    void                        slotInternalToGuiFileListReply( VxGUID onlineId, EPluginType pluginType, FileInfo fileInfo );

    void                        slotInternalToGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo offerInfo );
    void                        slotInternalToGuiRxedOfferReply( VxGUID onlineId, OfferBaseInfo offerInfo );

    void                        slotInternalToGuiPluginSessionEnded( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId );

    void                        slotInternalMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal, QString fileName );
    void                        slotInternalMediaError( EAppModule appModule, EMediaError mediaError, QString msg );

protected slots:
    void						slotMainWindowResized( void );
    void						slotMainWindowMoved( void );

    void						slotPlaySound( ESndDef sndDef );
    void						slotStatusMsg( QString strMsg );
    void						slotAppErr( EAppErr eAppErr, QString errMsg );

    void						slotOnNotifyIconFlashTimeout( bool bWhite );

    void						slotToGuiInstMsg( GuiUser* guiUser, EPluginType pluginType, QString pMsg );

    void						slotListViewTypeChanged( int viewSelectedIdx );

    void						slotRelayHelpButtonClicked( void );
    void						slotSetRelayHelpButtonVisibility( bool isVisible );
    void						slotNetworkStateChanged( ENetworkStateType eNetworkState );

    void						onMenuFileSelected( int iMenuId, PopupMenu* popupMenu, ActivityBase* contentFrame );
    void						onMenuSearchSelected( int iMenuId, PopupMenu* popupMenu, ActivityBase* contentFrame );

    void						onOncePerSecond( void );

    void						onUpdateMyIdent( VxNetIdent* poMyIdent );

    void						slotGuiStartupTimer( void );
    void						slotCheckSetupTimer( void );

protected:
    void                        connectSignals( void );

    void						showUserNameInTitle();
    void						sendAppSettingsToEngine( void );
    void						startNetworkMonitor( void );

    void						removePluginSessionOffer( EPluginType pluginType, GuiUser* poFriend );

    void						updateFriendList( GuiUser* guiUser, bool sessionTimeChange = false );

    void						clearToGuiActivityInterfaceList( void );
    void						clearFileXferClientList( void );
    void						clearHardwareCtrlList( void );
    void						clearUserUpdateClientList( void );

    void						registerMetaData();
    void						doAccountStartup( void );

    void                        checkReadyToLaunchAfterLogonApplets( void );
    bool                        isReadyToLaunchAfterLogonApplets( void );

    void                        checkReadyToConnectToLastConnectedHost( void );


    //=== vars ===//
    QApplication&               m_QApp;
    EDefaultAppMode				m_AppDefaultMode;
    AppGlobals					m_AppGlobals;
    AppSettings&                m_AppSettings;
    QString						m_AppShortName;
    QString						m_AppTitle;
    AccountMgr&                 m_AccountMgr;
    INlc&                       m_Nlc;

    GuiConnectIdListMgr			m_ConnectIdListMgr;
    GuiFavoriteMgr			    m_FavoriteMgr;
    GuiFileXferMgr              m_FileXferMgr;
    GuiThumbMgr					m_ThumbMgr;
    GuiMemberActiveMgr          m_MemberActiveMgr;
    GuiOfferMgr		            m_OfferMgr;
    GuiUserMgr					m_UserMgr;
    GuiGroupieListMgr			m_GroupieListMgr;
    GuiHostedListMgr			m_HostedListMgr;
    GuiHostJoinMgr				m_HostJoinMgr;
    GuiUserJoinMgr				m_UserJoinMgr;
    GuiPlayerMgr                m_PlayerMgr;
    GuiPluginMgr                m_PluginMgr;
    GuiWebPageMgr               m_WebPageMgr;

    MyIcons					    m_MyIcons;
    VxAppTheme					m_AppTheme;
    VxAppStyle					m_AppStyle;
    VxAppDisplay				m_AppDisplay;
    VxTilePositioner&           m_TilePositioner;
    CamLogic                    m_CamLogic;

    SoundMgr&                   m_SoundMgr;

    HomeWindow					m_HomePage;

    ActivityCreateAccount*      m_CreateAccountDlg{ nullptr };
    ActivityShowHelp*           m_ActivityShowHelpDlg{ nullptr };

    AppletMultiMessenger*       m_AppletMultiMessenger{ nullptr };
    AppletDownloads*            m_AppletDownloads{ nullptr };
    AppletUploads*              m_AppletUploads{ nullptr };

    std::string					m_strAccountUserName;

    QTimer*                     m_OncePerSecondTimer;
    EFriendViewType				m_eLastSelectedWhichContactsToView; // last selection of which friends to view

    bool						m_bUserCanceledCreateProfile;
    VxMutex						m_AppMutex;
    std::vector<QString>		m_DebugLogQue;
    std::vector<QString>		m_AppErrLogQue;
    ENetworkStateType			m_LastNetworkState;

    uint32_t					m_CamSourceId;
    uint32_t					m_CamCaptureRotation;

    bool	                    m_ToGuiActivityInterfaceBusy{ false };
    bool	                    m_ToGuiFileXferInterfaceBusy{ false };
    bool                        m_ToGuiHardwareCtrlBusy{ false };
    bool                        m_ToGuiUserUpdateClientBusy{ false };

    std::vector<ToGuiActivityInterface*>	    m_ToGuiActivityInterfaceList;
    std::vector<ToGuiHardwareControlInterface*> m_ToGuiHardwareCtrlList;
    std::vector<ToGuiUserUpdateInterface*>      m_ToGuiUserUpdateClientList;

    bool						m_LibraryActivityActive = false;
    bool						m_VidCaptureEnabled = false;
    bool						m_MicrophoneHardwareEnabled = false;
    bool						m_SpeakerHardwareEnabled = false;
    AppletMgr&                  m_AppletMgr;
    bool                        m_AppCommonInitialized = false;
    bool                        m_LoginBegin = false;
    bool                        m_LoginComplete = false;
    bool                        m_AppInitialized = false;

    ActivityAppSetup*           m_AppSetupDlg = nullptr;
    bool                        m_IsMessengerReady{ false };
    bool                        m_IsLoggedOn{ false };
    bool                        m_IsGuiSystemReady{ false };
    bool                        m_PtopNetworkReady{ false };

    bool                        m_GuiCpuTimeEnable{ false };

    QTimer*                     m_GuiStartupTimer = nullptr;
    QTimer*                     m_CheckSetupTimer = nullptr;

    bool                        m_LauchedAfterLogonApplets{ false };
    bool                        m_ConnectToLastConnectedHost{ false };
};

AppCommon& CreateAppInstance( INlc& nlc, QApplication* myApp );

AppCommon& GetAppInstance( void );

void DestroyAppInstance( );

