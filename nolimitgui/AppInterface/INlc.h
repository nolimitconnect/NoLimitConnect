#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiInterface/IDefs.h"
#include "GuiInterface/OsInterface/OsInterface.h"
#include "GuiInterface/ILog.h"
#include "GuiInterface/IToGui.h"
#include <GuiInterface/IFromGui.h>
#include <GuiInterface/INlcRender.h>
#include <GuiInterface/INlcEvents.h>
#include <GuiInterface/IAudioInterface.h>
#include <CoreLib/VxGUID.h>

#include <string>

class OsInterface;
class CAppParamParser;
class CApplication;
class MediaPlayerNlc;
class VxPeerMgr;
class P2PEngine;
class AppCommon;
// media player
class CRenderBuffer;

class INlc : public IToGui, public INlcRender, public INlcEvents, public IAudioRequests
{
public:
    INlc();
    virtual  ~INlc();

    static INlc&                getINlc();
    ILog&                       getILog()               { return m_ILog; }
    OsInterface&                getOsInterface()        { return m_OsInterface; }

    P2PEngine&                  getPtoP();
    AppCommon&                  getAppCommon();
#if defined(ENABLE_KODI)
    CApplication&               getKodi()               { return m_Kodi; }
#endif // ENABLE_KODI

#if defined(ENABLE_NLC_PLAYER)
    MediaPlayerNlc&             getNlcPlayer()          { return m_NlcPlayer; }
#endif // ENABLE_KODI


    //============================================================================
    //=== starup/shutdown ===//
    //============================================================================

    //=== stages of create ===//
    bool                        doPreStartup();
    bool                        doStartup();

    //=== stages of run ===//
    bool                        initRun( const CAppParamParser& params );
    bool                        doRun( EAppModule appModule );

    //=== stages of destroy ===//
    void                        doPreShutdown();
    void                        doShutdown();

    //=== running states ===//
    virtual void                toGuiSetIsAppModuleRunning( EAppModule appModule, bool isRunning ) override;
    virtual bool                toGuiGetIsAppModuleRunning( EAppModule appModule ) override                 { return m_IsRunning[ appModule ]; };
    virtual bool                toGuiRunModule( EAppModule appModule ) override;
    virtual void                toGuiCreateUserDirs( void ) override;

    //=== interface ===//
    void 				        toGuiPlayNlcMedia( AssetBaseInfo* assetInfo ) override;

    //============================================================================
    //=== from gui events ===//
    //============================================================================
    void                        fromGuiKeyPressEvent( EAppModule appModule, int key, int mod ) override;
    void                        fromGuiKeyReleaseEvent( EAppModule appModule, int key, int mod ) override;

    void                        fromGuiMousePressEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton ) override;
    void                        fromGuiMouseReleaseEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton ) override;
    void                        fromGuiMouseMoveEvent( EAppModule appModule, int mouseXPos, int mouseYPos ) override;

	void                        fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight ) override;
	void                        fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight ) override;
	void                        fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight ) override;

    void                        fromGuiCloseEvent( EAppModule appModule ) override;
    void                        fromGuiVisibleEvent( EAppModule appModule, bool isVisible ) override;

    //============================================================================
    //=== to gui media/render ===//
    //============================================================================

    virtual void                toGuiMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal = 0, const char* fileName = "" ) override;
    virtual void                toGuiMediaError( EAppModule appModule, EMediaError mediaError, const char* errMsg ) override;

    virtual void                verifyGlState( const char* msg = nullptr ) override; // show gl error if any

    //=== textures ===//
    void                        setActiveGlTexture( unsigned int activeTextureNum  /* 0 == GL_TEXTURE0 , 1 == GL_TEXTURE1 etc*/ ) override;

    void                        createTextureObject( CTextureQt * texture ) override;
    void                        destroyTextureObject( CTextureQt * texture ) override;
    bool                        loadToGPU( CTextureQt * texture ) override;
    void                        bindToUnit( CTextureQt * texture, unsigned int unit ) override;

    void                        beginGuiTexture( CGUITextureQt * guiTexture, NlcColor color ) override;
    void                        drawGuiTexture( CGUITextureQt * textrue, float * x, float * y, float * z, const NlcRect& texture, const NlcRect& diffuse, int orientation ) override;
    void                        endGuiTexture( CGUITextureQt * guiTexture ) override;
    void                        drawQuad( const NlcRect &rect, NlcColor color, CTextureBase * texture, const NlcRect * texCoords ) override;

    bool                        firstBegin( CGUIFontTTFQt * font )  override;
    void                        lastEnd( CGUIFontTTFQt * font ) override;
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    CVertexBuffer               createVertexBuffer( CGUIFontTTFQt * font, const std::vector<SVertex>& vertices )  override;
#endif // ENABLE_KODI
    void                        destroyVertexBuffer( CGUIFontTTFQt * font, CVertexBuffer& vertBuffer ) override;

    virtual void                deleteHardwareTexture( CGUIFontTTFQt * font )  override;
    virtual void                createStaticVertexBuffers( CGUIFontTTFQt * font )   override;
    virtual void                destroyStaticVertexBuffers( CGUIFontTTFQt * font )   override;

    //=== render ===//
    void                        captureScreen( CScreenshotSurface * screenCaptrue, NlcRect& captureArea ) override;

    void                        toGuiRenderVideoFrame( int textureIdx, CRenderBuffer* videoBuffer );
    bool                        initRenderSystem() override;
    bool                        destroyRenderSystem() override;
    bool                        resetRenderSystem( int width, int height ) override;

    int                         getMaxTextureSize() override;

    bool                        beginRender() override;
    bool                        endRender() override;
    void                        presentRender( bool rendered, bool videoLayer ) override;
    bool                        clearBuffers( NlcColor color ) override;
    bool                        isExtSupported( const char* extension )  override;

    void                        setVSync( bool vsync )  override;
    void                        resetVSync() override {  }

    void                        setViewPort( const NlcRect& viewPort ) override;
    void                        getViewPort( NlcRect& viewPort ) override;

    bool                        scissorsCanEffectClipping() override;
    NlcRect                     clipRectToScissorRect( const NlcRect &rect ) override;
    void                        setScissors( const NlcRect& rect ) override;
    void                        resetScissors() override;

    void                        captureStateBlock() override;
    void                        applyStateBlock() override;

    void                        setCameraPosition( const NlcPoint& camera, int screenWidth, int screenHeight, float stereoFactor = 0.0f ) override;

    void                        applyHardwareTransform( const TransformMatrix &matrix ) override;
    void                        restoreHardwareTransform() override;
    bool                        supportsStereo( RENDER_STEREO_MODE mode ) const  override { return false; }

    bool                        testRender() override;

    void                        project( float &x, float &y, float &z ) override;

    //=== shaders ===//
    std::string                 getShaderPath( const std::string &filename ) override { return ""; }

    void                        initialiseShaders() override;
    void                        releaseShaders() override;
    bool                        enableShader( ESHADERMETHOD method ) override;
    bool                        isShaderValid( ESHADERMETHOD method ) override;
    void                        disableShader( ESHADERMETHOD method ) override;
    void                        disableGUIShader() override;

    int                         shaderGetPos( )  override;
    int                         shaderGetCol()  override;
    int                         shaderGetModel()  override;
    int                         shaderGetCoord0( )  override;
    int                         shaderGetCoord1( )  override;
    int                         shaderGetUniCol( )  override;

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

    virtual void                shaderSetMatrices( ESHADERMETHOD shader, const float *p, const float *m ) override;
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
    virtual int                 shaderVertexAttribPointer( ESHADERMETHOD shader, unsigned int index, int size, int type, bool normalized, int stride, const void * pointer ) override;
    virtual void                shaderEnableVertexAttribArray( ESHADERMETHOD shader, int arrayId ) override;
    virtual void                shaderDisableVertexAttribArray( ESHADERMETHOD shader, int arrayId ) override;

    // frame buffers
    virtual void                frameBufferGen( int bufCount, unsigned int* fboId ) override;
    virtual void                frameBufferDelete( int bufCount, unsigned int* fboId ) override;
    virtual void                frameBufferTexture2D( int target, unsigned int texureId )  override;
    virtual void                frameBufferBind( unsigned int fboId ) override;
    virtual bool                frameBufferStatus( ) override;

    // gl functions
    void                        glFuncDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) override;
    void                        glFuncDisable( GLenum cap ) override;
    void                        glFuncBindTexture( GLenum target, GLuint texture ) override;
    void                        glFuncViewport( GLint x, GLint y, GLsizei width, GLsizei height ) override;
    void                        glFuncScissor( GLint x, GLint y, GLsizei width, GLsizei height ) override;

    virtual void                glFuncGenTextures( GLsizei n, GLuint * textures ) override;
    virtual void                glFuncDeleteTextures( GLsizei n, const GLuint *textures ) override;
    virtual void                glFuncTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels ) override;
    virtual void                glFuncTexParameteri( GLenum target, GLenum pname, GLint param ) override;
    virtual void                glFuncReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) override;
    virtual void                glFuncPixelStorei( GLenum pname, GLint param ) override;
    virtual void                glFuncFinish() override;
    
    virtual void                glFuncEnable( GLenum cap ) override;
    virtual void                glFuncTexSubImage2D( GLenum target, GLint level,
                                                       GLint xoffset, GLint yoffset,
                                                       GLsizei width, GLsizei height,
                                                       GLenum format, GLenum type,
                                                       const GLvoid *pixels ) override;
    virtual void                glFuncBlendFunc( GLenum sfactor, GLenum dfactor ) override;
    virtual void                glFuncVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer ) override;
    virtual void                glFuncDisableVertexAttribArray( GLuint index ) override;
    virtual void                glFuncEnableVertexAttribArray( GLuint index ) override;
    virtual void                glFuncDrawArrays( GLenum mode, GLint first, GLsizei count ) override;
    

    //============================================================================
    //=== end to gui media/render ===//
    //============================================================================

    //============================================================================
    //=== to gui ===//
    //============================================================================
    void				        toGuiLog( int logFlags, const char* pMsg ) override;
    void				        toGuiAppErr( EAppErr eAppErr, const char* errMsg = "" ) override;
    void				        toGuiStatusMessage( const char* statusMsg ) override;
    virtual void				toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, const char* paramMsg, ... ) override;
    virtual void				toGuiPluginMsg( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, std::string& paramMsg ) override;
    virtual void				toGuiPluginCommError( EPluginType pluginType, VxGUID& onlineId, EPluginMsgType msgType, ECommErr commErr ) override;
    /// a module has changed state
    virtual void				toGuiModuleState( EAppModule moduleNum, EModuleState moduleState )  override;

    // return true if any microphone device is available to be enabled
    virtual bool				toGuiIsMicrophoneDeviceAvailable( void ) override;

    virtual void				toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput ) override;
    // enable disable microphone for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput ) override;

    virtual void				toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput ) override;
    // enable disable speaker for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput ) override;

    // add audio data to play.. assumes pcm mono 16000 Hz
    virtual int				    toGuiModuleAudioFrame( EAppModule appModule, int16_t * pu16PcmData, int pcmDataLenInBytes, bool isSilence ) override;

#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    virtual int				    toGuiPlayerNlcAudio( EAppModule appModule, float * audioSamples48000, int dataLenInBytes ) override;

    virtual float               toGuiGetAudioDelaySeconds( EAppModule appModule ) override;

    virtual float               toGuiGetAudioCacheFreeSpace( EAppModule appModule ) override;

    virtual float               toGuiGetAudioCacheTotalSeconds( EAppModule appModule ) override;
#endif // ENABLE_KODI

    virtual void				toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture ) override;
    virtual void				toGuiPlayVideoFrame( VxGUID& onlineId, uint8_t * pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 ) override;
    virtual int				    toGuiPlayVideoFrame( VxGUID& onlineId, uint8_t * picBuf, uint32_t picBufLen, int picWidth, int picHeight ) override;

    virtual void				toGuiHostAnnounceStatus( EHostType hostType, VxGUID& sessionId, EHostAnnounceStatus joinStatus, const char* msg = "" ) override;
    virtual void				toGuiHostJoinStatus( EHostType hostType, VxGUID& sessionId, EHostJoinStatus joinStatus, const char* msg = "" ) override;

    virtual void				toGuiHostSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr = eCommErrNone, const char* msg = "" ) override;
    virtual void				toGuiHostSearchResult( EHostType hostType, VxGUID& sessionId, HostedInfo& hostedInfo ) override;
    virtual void				toGuiHostSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				toGuiGroupieSearchStatus( EHostType hostType, VxGUID& sessionId, EHostSearchStatus searchStatus, ECommErr commErr = eCommErrNone, const char* msg = "" ) override;
    virtual void				toGuiGroupieSearchResult( EHostType hostType, VxGUID& sessionId, GroupieInfo& hostedInfo ) override;
    virtual void				toGuiGroupieSearchComplete( EHostType hostType, VxGUID& sessionId ) override;

    virtual void				toGuiIsPortOpenStatus( EIsPortOpenStatus eIsPortOpenStatus, const char* msg = "" ) override;

    /// Send Network available status to GUI for display
    virtual void				toGuiNetAvailableStatus( ENetAvailStatus eNetAvailStatus ) override;
    virtual void				toGuiNetworkState( ENetworkStateType eNetworkState, const char* stateMsg = "" ) override;
    virtual void				toGuiRandomConnectStatus( ERandomConnectStatus eRandomConnectStatus, const char* msg = "" ) override;
    virtual void				toGuiRunTestStatus( const char*testName, ERunTestStatus eRunTestStatus, const char* msg = "" ) override;

    virtual void				toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    virtual void				toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override;

    virtual void				toGuiContactAdded( VxNetIdent* netIdent ) override;
    virtual void				toGuiContactRemoved( VxGUID& onlineId ) override;

    virtual void				toGuiContactOnline( VxNetIdent* netIdent ) override;

    //! called when contact changes
    //virtual void				toGuiContactNameChange( VxNetIdent* netIdent ) override;
    //virtual void				toGuiContactDescChange( VxNetIdent* netIdent ) override;
    //virtual void				toGuiContactFriendshipChange( VxNetIdent* netIdent ) override;

    //virtual void				toGuiContactSearchFlagsChange( VxNetIdent* netIdent ) override;
    //virtual void				toGuiPluginPermissionChange( VxNetIdent* netIdent ) override;

    virtual void				toGuiContactAnythingChange( VxNetIdent* netIdent ) override;
    virtual void				toGuiContactLastSessionTimeChange( VxNetIdent* netIdent ) override;

    //! called from engine when need to update identity
    virtual void				toGuiUpdateMyIdent( VxNetIdent* netIdent ) override;
    //! called from engine when identity changes need saved for next bootup
    virtual void				toGuiSaveMyIdent( VxNetIdent* netIdent ) override;

    virtual void				fromGuiSetIdentHasTextOffers( VxGUID& onlineId, bool hasTextOffers );

    //! add offer to notify list
    virtual void				toGuiRxedPluginOffer( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;
    //! response to offer
    virtual void				toGuiRxedOfferReply( VxNetIdent* netIdent, OfferBaseInfo& offerInfo ) override;

    virtual void				toGuiPluginSessionEnded( VxNetIdent* netIdent, EPluginType pluginType, VxGUID& lclSessionId ) override;

    virtual void				toGuiPluginStatus( EPluginType		pluginType,
                                                   int				statusType,
                                                   int				statusValue ) override;

    virtual void				toGuiInstMsg( VxNetIdent*	    netIdent,
                                              EPluginType		pluginType,
                                              const char*	    pMsg ) override;

    virtual void				toGuiFileListReply( VxGUID& onlineId, EPluginType pluginType, FileInfo& fileInfo ) override;
    virtual void				toGuiFileList( FileInfo& fileInfo ) override;
    virtual void				toGuiFileListCompleted( void ) override;

    virtual void				toGuiFileUploadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) override;

    virtual void				toGuiFileDownloadStart( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileIn ) override;

    virtual void				toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError ) override;
    virtual void				toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, std::string& fileName, EXferError xferError ) override;

    virtual void				toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 ) override;

    //=== scan ===//
    virtual void				toGuiScanSearchComplete( EScanType eScanType ) override;
    virtual void				toGuiScanResultSuccess( EScanType eScanType, VxNetIdent*	netIdent ) override;
    virtual void				toGuiSearchResultError( EScanType eScanType, VxNetIdent* netIdent, int errCode ) override;

    virtual void				toGuiSearchResultProfilePic( VxNetIdent*	netIdent,
                                                             uint8_t *      pu8JpgData,
                                                             uint32_t       u32JpgDataLen ) override;

    virtual void				toGuiSearchResultFileSearch( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo ) override;

    virtual void				toGuiSetGameValueVar( EPluginType	    pluginType,
                                                      VxGUID&	        oOnlineId,
                                                      int32_t			s32VarId,
                                                      int32_t			s32VarValue ) override;

    virtual void				toGuiSetGameActionVar( EPluginType	pluginType,
                                                       VxGUID&	    oOnlineId,
                                                       int32_t		s32VarId,
                                                       int32_t		s32VarValue ) override;

    virtual void				toGuiAssetAdded( AssetBaseInfo* assetInfo ) override;
    virtual void				toGuiAssetUpdated( AssetBaseInfo* assetInfo ) override;
    virtual void				toGuiAssetRemoved( AssetBaseInfo* assetInfo ) override;

    virtual void				toGuiAssetXferState( VxGUID& assetUniqueId, EAssetSendState assetSendState, int param ) override;

    virtual void				toGuiAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;
    virtual void				toGuiAssetSessionHistory( AssetBaseInfo* assetInfo ) override;

    virtual void				toGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000 ) override;

    virtual void				toGuiBlobAdded( BlobInfo * assetInfo ) override;
    virtual void				toGuiBlobAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;
    virtual void				toGuiBlobSessionHistory( BlobInfo * assetInfo ) override;

    virtual void				toGuiPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus ) override;

    virtual void				toGuiNetworkIsTested( bool requiresRelay, std::string& ipAddr, uint16_t ipPort )  override;

    //============================================================================
    //=== from gui ===//
    //============================================================================
    virtual void				fromGuiSetupContext( void );

    virtual void				fromGuiSetIsAppCommercial( bool isCommercial );
    virtual bool				fromGuiGetIsAppCommercial( void );
    virtual uint16_t			fromGuiGetAppVersionBinary( void );
    const char*				    fromGuiGetAppVersionString( void );
    virtual const char*		    fromGuiGetAppName( void );
    virtual const char*		    fromGuiGetAppNameNoSpaces( void );

    virtual void				fromGuiAppStartup( const char* assetDir, const char* rootDataDir );

    virtual void				fromGuiSetUserXferDir( const char* userDir );
    virtual void				fromGuiSetUserSpecificDir( const char* userDir );
    virtual uint64_t			fromGuiGetDiskFreeSpace( void );
    virtual uint64_t			fromGuiClearCache( ECacheType cacheType );
    virtual void				fromGuiAppShutdown( void );

    virtual void				fromGuiAppPauseOrResume( bool isPaused );

    virtual bool				fromGuiOrientationEvent( float f32RotX, float f32RotY, float f32RotZ );
    virtual bool				fromGuiMouseEvent( EMouseButtonType eMouseButType, EMouseEventType eMouseEventType, int iMouseXPos, int iMouseYPos );
    virtual bool				fromGuiMouseWheel( float f32MouseWheelDist );
    virtual bool				fromGuiKeyEvent( EKeyEventType eKeyEventType, EKeyCode eKey, int iFlags = 0 );

    virtual void				fromGuiNativeGlInit( void );
    virtual void				fromGuiNativeGlResize( int width, int height );
    virtual int					fromGuiNativeGlRender( void );
    virtual void				fromGuiNativeGlPauseRender( void );
    virtual void				fromGuiNativeGlResumeRender( void );
    virtual void				fromGuiNativeGlDestroy( void );

    virtual void				fromGuiNeedMorePlayData( int16_t * retAudioSamples,
                                                         int deviceReqDataLen );

    virtual void				fromGuiMuteMicrophone( bool mute );
    virtual void				fromGuiMuteSpeaker( bool mute );
    virtual bool				fromGuiIsMicrophoneMuted( void );
    virtual bool				fromGuiIsSpeakerMuted( void );

    virtual void				fromGuiWantMediaInput( VxGUID& id, EMediaInputType eMediaType, EAppModule appModule, bool wantInput );
    virtual void				fromGuiVideoData( uint32_t u32FourCc, uint8_t * pu8VidDataIn, int iWidth, int iHeight, uint32_t u32VidDataLen, int iRotation );
    virtual bool				fromGuiMovieDone( void );

    virtual void				fromGuiUserLoggedOn( VxNetIdent* netIdent );
    virtual void				fromGuiOnlineNameChanged( const char* newOnlineName );
    virtual void				fromGuiMoodMessageChanged( const char* newMoodMessage );
    virtual void				fromGuiIdentPersonalInfoChanged( int age, int gender, int language, int preferredContent );

    virtual void				fromGuiSetUserHasProfilePicture( bool haveProfilePick );
    virtual void				fromGuiUpdateMyIdent( VxNetIdent* netIdent, bool permissionAndStatsOnly );
    virtual void				fromGuiQueryMyIdent( VxNetIdent* poRetIdent );

    virtual void				fromGuiApplyNetHostSettings( NetHostSetting& netSettings );
    virtual void				fromGuiGetNetSettings( NetSettings& netSettings );
    virtual void				fromGuiSetNetSettings( NetSettings& netSettings );
    virtual void				fromGuiNetworkSettingsChanged( void );

    virtual void				fromGuiNetworkAvailable( const char* lclIp = nullptr, bool isCellularNetwork = false );
    virtual void				fromGuiNetworkLost( void );
    virtual ENetLayerState	    fromGuiGetNetLayerState( ENetLayerType netLayer = eNetLayerTypeInternet );

    virtual void				fromGuiAnnounceHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 );
    virtual void				fromGuiJoinHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 );
    virtual void				fromGuiLeaveHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 );
    virtual void				fromGuiUnJoinHost( EHostType hostType, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 );
    virtual void				fromGuiJoinLastJoinedHost( EHostType hostType, VxGUID& sessionId );

    virtual void				fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable );
    virtual void				fromGuiSendAnnouncedList( EHostType hostType, VxGUID& sessionId );

    virtual void				fromGuiDisconnectFromUser( VxGUID& onlineId );

    virtual void				fromGuiRunIsPortOpenTest( uint16_t port );
    virtual void				fromGuiRunUrlAction( VxGUID& sessionId, const char* myUrl, const char* ptopUrl, ENetCmdType testType );

    virtual void				fromGuiGetFileShareSettings( FileShareSettings& fileShareSettings );
    virtual void				fromGuiSetFileShareSettings( FileShareSettings& fileShareSettings );

    virtual void				fromGuiUpdateWebPageProfile( const char*	pProfileDir,	// directory containing user profile
                                                             const char*	strGreeting,	// greeting text
                                                             const char*	aboutMe,		// about me text
                                                             const char*	url1,			// favorite url 1
                                                             const char*	url2,			// favorite url 2
                                                             const char*	url3,			// favorite url 3
                                                             const char*	donation );     // donation

    virtual void 				fromGuiSetPluginPermission( EPluginType pluginType, int eFriendState );
    virtual int					fromGuiGetPluginPermission( EPluginType pluginType );
    virtual int					fromGuiGetPluginServerState( EPluginType pluginType );

    virtual void				fromGuiStartPluginSession( EPluginType pluginType, VxGUID oOnlineId, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() );
    virtual void				fromGuiStopPluginSession( EPluginType pluginType, VxGUID oOnlineId, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() );
    virtual bool				fromGuiIsPluginInSession( EPluginType pluginType, VxGUID oOnlineId, int pvUserData = 0, VxGUID lclSessionId = VxGUID::nullVxGUID() );

    virtual bool				fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo );
    virtual bool				fromGuiToPluginOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo );

    virtual EXferError			fromGuiFileXferControl( EPluginType pluginType, EXferAction xferAction, FileInfo& fileInfo );

    virtual bool				fromGuiInstMsg( EPluginType	pluginType, VxGUID& onlineId, const char* pMsg );
                                                                                  
    virtual bool				fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk );

    virtual bool				fromGuiChangeMyFriendshipToHim( VxGUID&	onlineId, EFriendState myFriendshipToHim, EFriendState hisFriendshipToMe );                                                              
                                                                
    virtual void				fromGuiSendContactList( EFriendViewType eFriendView, int maxContactsToSend );
    virtual void				fromGuiRefreshContactList( int maxContactsToSend );

    virtual void				fromGuiRequireRelay( bool bRequireRelay );

    virtual void				fromGuiRelayPermissionCount( int userPermittedCount, int anonymousCount );

    virtual void				fromGuiStartScan( EScanType eScanType, uint8_t searchFlags, uint8_t fileTypeFlags, const char* pSearchPattern = "" );
    virtual void				fromGuiNextScan( EScanType eScanType );
    virtual void				fromGuiStopScan( EScanType eScanType );

    virtual InetAddress			fromGuiGetMyIPv4Address( void );
    virtual InetAddress			fromGuiGetMyIPv6Address( void );

    virtual void				fromGuiUserModifiedStoryboard( void );

    virtual void				fromGuiCancelDownload( VxGUID&			fileInstanceId );
    virtual void				fromGuiCancelUpload( VxGUID&			fileInstanceId );

    virtual bool				fromGuiSetGameValueVar( EPluginType	pluginType,
                                                        VxGUID&		oOnlineId,
                                                        int32_t			s32VarId,
                                                        int32_t			s32VarValue );

    virtual bool				fromGuiSetGameActionVar( EPluginType	pluginType,
                                                         VxGUID&		oOnlineId,
                                                         int32_t			s32VarId,
                                                         int32_t			s32VarValue );

    virtual bool				fromGuiTestCmd( IFromGui::ETestParam1		eTestParam1,
                                                int							testParam2 = 0,
                                                const char*				testParam3 = NULL );
#if ENABLE_COMPONENT_NEARBY
    virtual bool				fromGuiNearbyBroadcastEnable( bool enable );
#endif // ENABLE_COMPONENT_NEARBY
    virtual void				fromGuiDebugSettings( uint32_t u32LogFlags, const char*	pLogFileName = NULL );
    virtual void				fromGuiSendLog( uint32_t u32LogFlags );

    virtual bool				fromGuiBrowseFiles( std::string& folderName, uint8_t fileFilterMask = VXFILE_TYPE_ALLNOTEXE | VXFILE_TYPE_DIRECTORY );
    virtual bool				fromGuiGetSharedFiles( uint8_t fileTypeFilter );
    virtual bool				fromGuiSetFileIsShared( FileInfo& fileInfo, bool addFisSharedile );
    virtual bool				fromGuiGetIsFileShared( std::string& fileName );
    virtual bool				fromGuiRemoveSharedFile( std::string fileName ); // for remove before deletion

    virtual int					fromGuiGetFileDownloadState( uint8_t * fileHashId );
    virtual bool				fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool isInLibrary );
    virtual bool				fromGuiSetFileIsInLibrary( std::string fileName, bool inLibrary );
    virtual void				fromGuiGetFileLibraryList( uint8_t fileTypeFilter );
    virtual bool				fromGuiGetIsFileInLibrary( std::string& fileName );
    virtual bool				fromGuiRemoveFromLibrary( std::string& fileNamee ); // for remove before deletion
    virtual bool				fromGuiIsNoLimitVideoFile( const char* fileName );
    virtual bool				fromGuiIsNoLimitAudioFile( const char* fileName );

    virtual int					fromGuiDeleteFile( std::string fileName, bool shredFile );

    virtual bool				fromGuiAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo, int pos0to100000 = 0 );
    virtual bool				fromGuiAssetAction( EPluginType pluginType, EAssetAction assetAction, VxGUID& assetId, int pos0to100000 = 0 );
    virtual bool				fromGuiSendAsset( AssetBaseInfo& assetInfo );

    virtual bool				fromGuiVideoRecord( EVideoRecordState eRecState, VxGUID& feedId, const char* fileName );
    virtual bool				fromGuiSndRecord( ESndRecordState eRecState, VxGUID& feedId, const char* fileName );

    virtual void				fromGuiQuerySessionHistory( GroupieId& groupieId );
    virtual bool				fromGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000 );

    /// return number of items currently in list
    virtual int					fromGuiGetJoinedListCount( EPluginType pluginType );
    virtual void				fromGuiListAction( EListAction listAction );
    virtual std::string			fromGuiQueryDefaultUrl( EHostType hostType );
    virtual bool                fromGuiSetDefaultUrl( EHostType hostType, std::string& hostUrl );
    virtual bool				fromGuiQueryIdentity( std::string& url, VxNetIdent& retNetIdent, bool requestIdentityIfUnknown );
    virtual bool				fromGuiQueryIdentity( const VxGUID& onlineId, VxNetIdent& retNetIdent );
    virtual bool				fromGuiQueryHosts( std::string& netHostUrl, EHostType hostType, std::vector<HostedInfo>& hostedInfoList, VxGUID& hostIdIfNullThenAll );
    virtual bool				fromGuiQueryMyHostedInfo( EHostType hostType, std::vector<HostedInfo>& hostedInfoList );
    virtual bool				fromGuiQueryHostListFromNetworkHost( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll );
    virtual bool				fromGuiQueryGroupiesFromHosted( VxPtopUrl& hostedUrl, EHostType hostType, VxGUID& onlineIdIfNullThenAll );

    virtual bool				fromGuiDownloadWebPage( EWebPageType webPageType, VxGUID& onlineId );
    virtual bool				fromGuiCancelWebPage( EWebPageType webPageType, VxGUID& onlineId );

    virtual bool				fromGuiDownloadFileList( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId, uint8_t fileTypes = 0 );
    virtual bool				fromGuiDownloadFileListCancel( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId );
  
    //============================================================================
    //=== utilities ===//
    //============================================================================

    //=== utilities ===//
    bool                        initDirectories();
    void                        createUserDirs() const;

    //=== ffmpeg ===//
    void                        startupFfmpeg();
    void                        shutdownFfmpeg( );

    //=== fopen ssl ===//
    void                        setSslCertFile( std::string certFile );
    std::string                 getSslCertFile( ) { return m_SslCertFile; }


private:
    ILog                        m_ILog; // must be first in initializer list
    OsInterface&                m_OsInterface;

#if defined(ENABLE_KODI)
    CApplication&               m_Kodi;  
#elif defined(ENABLE_NLC_PLAYER)
    MediaPlayerNlc&             m_NlcPlayer;
#endif // ENABLE_KODI
    bool                        m_IsRunning[ eMaxAppModule ];

    std::string                 m_SslCertFile;
};

P2PEngine& GetPtoPEngine();

// convenience defines
#define GetToGui()              INlc::getINlc()
#define GetINlc()               INlc::getINlc()
#define GetILog()               INlc::getINlc().getILog()
#define GetOsInterface()        INlc::getINlc().getOsInterface()
#define GetPtoP()               INlc::getINlc().getPtoP()

