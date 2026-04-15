#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "GuiThumb.h"
#include "GuiThumbCallback.h"
#include "GuiThumbList.h"

#include <ThumbMgr/ThumbCallbackInterface.h>

#include <CoreLib/VxMutex.h>

#include <QMutex>
#include <QObject>
#include <QPixmap>
#include <QTimer>

#include <vector>

class AppCommon;
class AppletBase;
class GuiThumbCallbackInterface;
class GuiUser;

class GuiThumbMgr : public QObject, public ThumbCallbackInterface
{
    Q_OBJECT
public:
    GuiThumbMgr() = delete;
    GuiThumbMgr( AppCommon& app );
    GuiThumbMgr( const GuiThumbMgr& rhs ) = delete;
	virtual ~GuiThumbMgr() = default;

    virtual void                onAppCommonCreated( void );
    virtual void                onMessengerReady( bool ready )              { }
    virtual bool                isMessengerReady( void );
    virtual void                onSystemReady( bool ready );

    void                        wantGuiThumbCallbacks( GuiThumbCallback* callback, bool wantCallback );

    VxGUID                      getMyOnlineId( void )                       { return m_MyOnlineId; }  

    // Emoticon pixmap cache API
    bool                        getEmoticonPixmap( int emoticonNum, QSize imageSize, QPixmap& retPixmap );
    bool                        getAllEmoticonPixmaps( QSize imageSize, QVector<QPixmap>& retPixmaps );
    bool                        isEmoticonCacheReady( void ) const          { return m_EmoticonCacheReady; }
    void                        startEmoticonCacheLoad( void );

    // Legacy API - redirects to cache
    void                        generateEmoticonsIfNeeded( AppletBase* applet );
    bool                        getEmoticonImage( int emoticonNum, QSize imageSize, QPixmap& retImage );

    bool                        requestAvatarImage( GuiUser* guiUser, EHostType requestedThumbType, QImage& retAvatarImage, bool requestFromUserIfValid = false );
    bool                        requestAvatarImage( GuiUser* guiUser, EPluginType requestedThumbType, QImage& retAvatarImage, bool requestFromUserIfValid = false );

    virtual void				callbackThumbAdded( ThumbInfo* thumbInfo ) override; 
    virtual void				callbackThumbUpdated( ThumbInfo* thumbInfo ) override; 
    virtual void				callbackThumbRemoved( VxGUID& thumbId ) override; 

    void                        onThumbAdded( GuiThumb* guiThumb );
    void                        onThumbRemoved( VxGUID& thumbId );
    void                        onThumbUpdated( GuiThumb* guiThumb );

    GuiThumb*                   getThumb( VxGUID& thumbId );
    bool                        getAvatarImage( VxGUID& thumbId, QImage& image ); // get rounded image for avatar
    bool                        getThumbImage( VxGUID& thumbId, QImage& image );
    GuiThumbList&               getThumbList( void )             { return m_ThumbList; }

    void				        addThumbIfDoesNotExist( ThumbInfo* thumbInfo );

signals:
    void                        signalInternalThumbAdded( ThumbInfo thumbInfo );
    void                        signalInternalThumbUpdated( ThumbInfo thumbInfo );
    void                        signalInternalThumbRemoved( VxGUID thumbId );
    void                        signalEmoticonCacheReady( void );

private slots:
    void                        slotInternalThumbAdded( ThumbInfo thumbInfo );
    void                        slotInternalThumbUpdated( ThumbInfo thumbInfo );
    void                        slotInternalThumbRemoved( VxGUID onlineId );
    void                        slotEmoticonCacheLoadTick( void );

protected:
    GuiThumb*                   updateThumb( ThumbInfo& thumbInfo  );
    void                        removeThumb( VxGUID& thumbId );
    GuiThumb*                   findThumb( VxGUID& thumbId );
    GuiThumb*                   findOrCreateEmoticonThumb( VxGUID& thumbId );
    GuiThumb*                   generateEmoticon( VxGUID& thumbId, bool checkIfExists = true );
    
    // Emoticon cache helpers
    bool                        loadEmoticonToCache( int emoticonNum );
    QString                     getEmoticonNltPath( int emoticonNum );
    QString                     getEmoticonSvgPath( int emoticonNum );

    AppCommon&                  m_MyApp;
    GuiThumbList                m_ThumbList;
    GuiThumb*                   m_MyIdent{ nullptr };
    VxGUID                      m_MyOnlineId;
    std::vector<VxGUID>         m_EmoticonList;
    std::vector<GuiThumbCallback*> m_GuiThumbClientList;

    // Emoticon pixmap cache (48x48 base size)
    static constexpr int        kEmoticonCacheSize = 48;
    static constexpr int        kEmoticonCount = 50;
    static constexpr int        kEmoticonLoadChunkSize = 5;
    std::vector<QPixmap>        m_EmoticonPixmapCache;
    std::vector<bool>           m_EmoticonLoaded;
    bool                        m_EmoticonCacheReady{ false };
    int                         m_EmoticonCacheLoadIndex{ 1 };
    QTimer*                     m_EmoticonCacheLoadTimer{ nullptr };
    mutable QMutex              m_EmoticonCacheMutex;
};
