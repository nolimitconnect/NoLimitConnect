//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiThumbMgr.h"

#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "HomeWindow.h"

#include <P2PEngine/P2PEngine.h>
#include <ThumbMgr/ThumbInfo.h>
#include <ThumbMgr/ThumbMgr.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <algorithm>

#include <QPainter>

//============================================================================
GuiThumbMgr::GuiThumbMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiThumbMgr::onAppCommonCreated( void )
{
    m_MyApp.getEngine().getThumbMgr().lockResources();
    const std::vector<VxGUID>& emoticonList = ThumbMgr::getEmoticonIdList();
    // LogMsg( LOG_VERBOSE, "GuiThumbMgr::onAppCommonCreated emoticon list size %d", emoticonList.size() );
    int emoticonNum = 0;
    for( auto& guid : emoticonList )
    {     
        if( guid.isVxGUIDValid() )
        {
            // LogMsg( LOG_VERBOSE, "GuiThumbMgr::onAppCommonCreated emoticon  %d is valid %s", emoticonNum, guid.toOnlineIdString().c_str() );
            m_EmoticonList.emplace_back( guid );
        }
        else
        {
            LogMsg( LOG_VERBOSE, "GuiThumbMgr::onAppCommonCreated emoticon  %d invalid %s", emoticonNum, guid.toOnlineIdString().c_str() );
        }

        emoticonNum++;
    }

    m_MyApp.getEngine().getThumbMgr().unlockResources();

    connect( this, SIGNAL(signalInternalThumbAdded(ThumbInfo) ),	this, SLOT(slotInternalThumbAdded(ThumbInfo) ), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalThumbUpdated(ThumbInfo) ), this, SLOT(slotInternalThumbUpdated(ThumbInfo) ), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalThumbRemoved(VxGUID) ),	this, SLOT(slotInternalThumbRemoved(VxGUID) ), Qt::QueuedConnection );

    m_MyApp.getEngine().getThumbMgr().addThumbMgrClient( this, true );
}

//============================================================================
bool GuiThumbMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiThumbMgr::callbackThumbAdded( ThumbInfo* guiThumb )
{
    if( guiThumb && guiThumb->isValidThumbnail() )
    {
        emit signalInternalThumbAdded( *guiThumb );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiThumbMgr::callbackThumbAdded invalid guiThumb" );
    }
}

//============================================================================
void GuiThumbMgr::callbackThumbUpdated( ThumbInfo* guiThumb )
{
    if( guiThumb && guiThumb->isValidThumbnail() )
    {
        emit signalInternalThumbUpdated( *guiThumb );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiThumbMgr::callbackThumbUpdated invalid guiThumb" );
    }
}

//============================================================================
void GuiThumbMgr::callbackThumbRemoved( VxGUID& thumbId )
{
    emit signalInternalThumbRemoved( thumbId );
}

//============================================================================
void GuiThumbMgr::slotInternalThumbAdded( ThumbInfo thumbInfo )
{
    updateThumb( thumbInfo );
}

//============================================================================
void GuiThumbMgr::slotInternalThumbUpdated( ThumbInfo thumbInfo )
{
    updateThumb( thumbInfo );
}

//============================================================================
void GuiThumbMgr::slotInternalThumbRemoved( VxGUID thumbId )
{
    GuiThumb* guiThumb = findThumb( thumbId );
    if( guiThumb )
    {
        onThumbRemoved( thumbId );
    }

    removeThumb( thumbId );
}

//============================================================================
GuiThumb* GuiThumbMgr::findThumb( VxGUID& thumbId )
{
    GuiThumb* guiThumb = m_ThumbList.findThumb( thumbId );
    if( !guiThumb && std::find( m_EmoticonList.begin(), m_EmoticonList.end(), thumbId ) != m_EmoticonList.end() )
    {
        guiThumb = generateEmoticon( thumbId, false );
    }

    return guiThumb;
}

//============================================================================
GuiThumb* GuiThumbMgr::findOrCreateEmoticonThumb( VxGUID& thumbId )
{
    GuiThumb* guiThumb = m_ThumbList.findThumb( thumbId );
    if( !guiThumb && std::find( m_EmoticonList.begin(), m_EmoticonList.end(), thumbId ) != m_EmoticonList.end() )
    {
        guiThumb = generateEmoticon( thumbId, false );
    }

    return guiThumb;
}

//============================================================================
void GuiThumbMgr::removeThumb( VxGUID& thumbId )
{
    m_ThumbList.removeThumb( thumbId );
}

//============================================================================
GuiThumb* GuiThumbMgr::getThumb( VxGUID& thumbId )
{
    GuiThumb* guiThumb = findThumb( thumbId );
    return guiThumb;
}

//============================================================================
GuiThumb* GuiThumbMgr::updateThumb( ThumbInfo& thumbInfo )
{
    if( !thumbInfo.isValidThumbnail() )
    {
        LogMsg( LOG_ERROR, "GuiThumbMgr::updateThumbOnline invalid param" );
        return nullptr;
    }

    GuiThumb* guiThumb = m_ThumbList.findThumb( thumbInfo.getAssetUniqueId() ); 
    if( guiThumb )
    {
        onThumbUpdated( guiThumb );
    }
    else
    {
        guiThumb = new GuiThumb( m_MyApp );
        guiThumb->setThumbInfo( thumbInfo );
        m_ThumbList.addThumbIfDoesntExist( guiThumb );
        m_MyApp.getEngine().getThumbMgr().fromGuiThumbCreated( thumbInfo );
        onThumbAdded( guiThumb );
    }

    return guiThumb;
}

//============================================================================
void GuiThumbMgr::onThumbAdded( GuiThumb* guiThumb )
{
    if( isMessengerReady() )
    {
        for( auto& client : m_GuiThumbClientList )
        {
            client->callbackThumbAdded( guiThumb );
        }
    }
}

//============================================================================
void GuiThumbMgr::onThumbUpdated( GuiThumb* guiThumb )
{
    if( isMessengerReady() )
    {
        for( auto& client : m_GuiThumbClientList )
        {
            client->callbackThumbUpdated( guiThumb );
        }
    }
}

//============================================================================
void GuiThumbMgr::onThumbRemoved( VxGUID& thumbId )
{
    if( isMessengerReady() )
    {
        for( auto& client : m_GuiThumbClientList )
        {
            client->callbackThumbRemoved( thumbId );
        }
    }
}

//============================================================================
bool GuiThumbMgr::requestAvatarImage( GuiUser* guiUser, EHostType hostType, QImage& retAvatarImage, bool requestFromUserIfValid )
{
    return requestAvatarImage( guiUser, HostTypeToClientPlugin( hostType ), retAvatarImage, requestFromUserIfValid );
}

//============================================================================
bool GuiThumbMgr::requestAvatarImage( GuiUser* guiUser, EPluginType pluginType, QImage& retAvatarImage, bool requestFromUserIfValid )
{
    bool foundThumb = false;
    if( guiUser )
    {
        pluginType = HostPluginToClientPluginType( pluginType );
        VxNetIdent& netIdent = guiUser->getNetIdent();
        VxGUID pluginThumbId = netIdent.getThumbId( pluginType );
        VxGUID thumbId = pluginThumbId.isVxGUIDValid() ? pluginThumbId : netIdent.getAvatarThumbGuid();
        if( thumbId.isVxGUIDValid() )
        {
            GuiThumb* guiThumb = m_ThumbList.findThumb( thumbId );
            if( guiThumb )
            {
                return guiThumb->createImage( retAvatarImage );
            }
            else if( requestFromUserIfValid )
            {
                ThumbMgr& thumbMgr = m_MyApp.getEngine().getThumbMgr();
                if( thumbMgr.isEmoticonThumbnail( thumbId ) )
                {
                    GuiThumb* guiThumb = generateEmoticon( thumbId );
                    if( guiThumb )
                    {
                        return guiThumb->createImage( retAvatarImage );
                    }
                }
                else
                {
                    GuiThumb* newThumb{ nullptr };

                    thumbMgr.lockResources();
                    AssetBaseInfo* assetInfo = thumbMgr.findAsset( thumbId );
                    ThumbInfo* thumbInfo = dynamic_cast< ThumbInfo* >( assetInfo );
                    if( thumbInfo && thumbInfo->isValidThumbnail() )
                    {
                        newThumb = new GuiThumb( m_MyApp );
                        newThumb->setThumbInfo( *thumbInfo );
                    }

                    thumbMgr.unlockResources();

                    if( newThumb )
                    {
                        m_ThumbList.addThumbIfDoesntExist( newThumb );
                        onThumbAdded( newThumb );
                        return newThumb->createImage( retAvatarImage );
                    }
                    else
                    {
                        thumbMgr.fromGuiRequestPluginThumb( &guiUser->getNetIdent(), pluginThumbId.isVxGUIDValid() ? pluginType : ePluginTypeHostPeerUser, thumbId );
                    }
                }
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "requestAvatarImage null user" );
    }

    return foundThumb;
}

//============================================================================
bool GuiThumbMgr::getAvatarImage( VxGUID& thumbId, QImage& image )
{
    bool result = getThumbImage( thumbId, image );
    if( result && !image.isNull() )
    {
        result = GuiHelpers::makeCircleImage( image );
    }

    return result;
}

//============================================================================
bool GuiThumbMgr::getThumbImage( VxGUID& thumbId, QImage& image )
{
    bool result = false;
    if( thumbId.isVxGUIDValid() )
    {
        GuiThumb* guiThumb = m_ThumbList.findThumb( thumbId );
        if( !guiThumb )
        {
            if( m_MyApp.getEngine().getThumbMgr().isEmoticonThumbnail( thumbId ) )
            {
                guiThumb = generateEmoticon( thumbId );
            }
        }
        
        if( guiThumb )
        {
            result = guiThumb->createImage( image );
        }
        else
        {
            // try the raw file name
            std::string thumbFile = m_MyApp.getEngine().getThumbMgr().fromGuiGetThumbFile( thumbId );
            if( !thumbFile.empty() && VxFileUtil::fileExists( thumbFile.c_str() ) )
            {
                result = image.load( thumbFile.c_str() ) && !image.isNull();
            }
        }
    }

    return result;
}

//============================================================================
GuiThumb* GuiThumbMgr::generateEmoticon( VxGUID& thumbId, bool checkIfExists )
{
    if( !thumbId.isVxGUIDValid() )
    {
        QString msgText = QObject::tr( "Invalid emoticon id" );
        QMessageBox::warning( &m_MyApp.getHomeWindow(), msgText, msgText, QMessageBox::Ok );
        return nullptr;
    }

    GuiThumb* guiThumb = nullptr;
    if( checkIfExists )
    {
        // see if already have it
        guiThumb = findThumb( thumbId );
        if( guiThumb )
        {
            return guiThumb;
        }
    }

    // validate is emoticon thumbnail
    ThumbMgr& thumbMgr = m_MyApp.getEngine().getThumbMgr();
    // see if engine thumbMgr manager has it
    thumbMgr.lockResources();
    ThumbInfo* existingThumbInfo = dynamic_cast< ThumbInfo* >( thumbMgr.findAsset( thumbId ) );
    if( existingThumbInfo && existingThumbInfo->isValidThumbnail() )
    {
        ThumbInfo copyThumbInfo = *existingThumbInfo;
        thumbMgr.unlockResources();
        guiThumb = updateThumb( copyThumbInfo );
        if( guiThumb )
        {
            return guiThumb;
        }
    }
    else
    {
        thumbMgr.unlockResources();
    }

    QString errMsgText = QObject::tr( "Error occured creating emoticon file" );
    // see if file exists and we just need to create the info
    QString fileName;
    if( !GuiHelpers::createThumbFileName( thumbId, fileName ) )
    {
        QMessageBox::warning( &m_MyApp.getHomeWindow(), errMsgText, errMsgText, QMessageBox::Ok );
        return nullptr;
    }

    int64_t timeNow = GetTimeStampMs();
    VxFileInfoBase fileInfo;
    if( VxFileUtil::getFileInfo( fileName.toUtf8().constData(), fileInfo ) )
    {
        // file exists so create a thumbnail instance
        ThumbInfo thumbInfo( fileInfo );
        thumbInfo.setAssetUniqueId( thumbId );
        thumbInfo.setCreatorId( m_MyApp.getEngine().getMyOnlineId() );
        thumbInfo.setCreationTime( timeNow );
        thumbInfo.setModifiedTime( timeNow );
        guiThumb = updateThumb( thumbInfo );
        if( guiThumb )
        {
            thumbMgr.lockResources();
            ThumbInfo* existingThumbInfo = dynamic_cast<ThumbInfo*>( thumbMgr.findAsset( thumbId ) );
            if( existingThumbInfo && existingThumbInfo->isValidThumbnail() )
            {
                thumbMgr.unlockResources();
                return guiThumb;
            }
            else
            {
                QMessageBox::warning( &m_MyApp.getHomeWindow(), errMsgText, errMsgText, QMessageBox::Ok );
            }

            thumbMgr.unlockResources();
        }
    }

    // static member so no need to make copy
    std::vector<VxGUID>& emoticonIdList = thumbMgr.getEmoticonIdList();
    // 0 based emoticon number but first id is always null
    int emoticonNum = 1;
    bool foundId = false;
    for( auto& assetGuid : emoticonIdList )
    {
        if( assetGuid == thumbId )
        {
            foundId = true;
            break;
        }

        emoticonNum++;
    }

    if( !foundId || emoticonNum > (int)emoticonIdList.size() )
    {
        QString msgText = QObject::tr( "Invalid emoticon id" );
        QMessageBox::warning( &m_MyApp.getHomeWindow(), QObject::tr( "Invalid emoticon id " ), msgText + thumbId.toOnlineIdString().c_str(), QMessageBox::Ok );
    }

    // create and new emoticon thumbnail
    const int emoteMargin = 20;

    QPixmap image;
    QSize imageSize( GuiParams::getThumbnailSize().width() - emoteMargin * 2, GuiParams::getThumbnailSize().height() - emoteMargin * 2 );
    if( m_MyApp.getThumbMgr().getEmoticonImage( emoticonNum, imageSize, image ) )
    {
        QPixmap finalThumbnail( GuiParams::getThumbnailSize() );
        finalThumbnail.fill( QColor( COLOR_TRANSPARENT ) );
        QPainter painter( &finalThumbnail );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.setRenderHint( QPainter::TextAntialiasing, true );
        painter.setRenderHint( QPainter::SmoothPixmapTransform, true );

        painter.drawPixmap( emoteMargin, emoteMargin, image.width(), image.height(), image );

        uint64_t fileLen = GuiHelpers::saveToPngFile( finalThumbnail, fileName );
        if( fileLen )
        {
            VxFileInfoBase fileInfo;
            if( VxFileUtil::getFileInfo( fileName.toUtf8().constData(), fileInfo ) )
            {
                ThumbInfo assetInfo( fileInfo );
                assetInfo.setAssetUniqueId( thumbId );
                assetInfo.setCreatorId( m_MyApp.getEngine().getMyOnlineId() );
                assetInfo.setCreationTime( timeNow );
                assetInfo.setModifiedTime( timeNow );
                guiThumb = updateThumb( assetInfo );

                if( !thumbMgr.fromGuiThumbCreated( assetInfo ) )
                {
                    QString msgText = QObject::tr( "Could not create emoticon asset" );
                    QMessageBox::warning( &m_MyApp.getHomeWindow(), QObject::tr( "Error occured creating emoticon asset " ) + fileName, msgText, QMessageBox::Ok );
                }
            }
            else
            {
                QString msgText = QObject::tr( "Could not get file info of emoticon png file" );
                QMessageBox::warning( &m_MyApp.getHomeWindow(), QObject::tr( "Error occured creating emoticon file " ) + fileName, msgText, QMessageBox::Ok );
            }
        }
        else
        {
            QString msgText = QObject::tr( "Could not create emoticon png file" );
            QMessageBox::warning( &m_MyApp.getHomeWindow(), QObject::tr( "Error occured creating emoticon file " ) + fileName, msgText, QMessageBox::Ok );
        }
    }
    else
    {
        QString msgText = QObject::tr( "Could not create emoticon image" );
        QMessageBox::warning( &m_MyApp.getHomeWindow(), QObject::tr( "Error occured creating emoticon image %1" ).arg( emoticonNum ), msgText, QMessageBox::Ok );
    }

    return guiThumb;
}

//============================================================================
void GuiThumbMgr::generateEmoticonsIfNeeded( AppletBase * applet )
{
    ThumbMgr& thumbMgr = m_MyApp.getEngine().getThumbMgr();
    std::vector<VxGUID>& emoticonIdList = thumbMgr.getEmoticonIdList();
    int emoticonNum = 1;
    for( auto& assetGuid : emoticonIdList )
    {
        if( assetGuid.isVxGUIDValid() )
        {
            GuiThumb *guiThumb = findOrCreateEmoticonThumb( assetGuid );
            if( !guiThumb )
            {
                QString msgText = QObject::tr( "Could create emoticon image" );
                QMessageBox::warning( ( QWidget* )applet, QObject::tr( "Error occured creating emoticon image %1" ).arg( emoticonNum ), msgText, QMessageBox::Ok );
            }
        }

        emoticonNum++;
    }
}

//============================================================================
bool GuiThumbMgr::getEmoticonImage( int emoticonNum, QSize imageSize, QPixmap& retImage )
{
    char resBuf[128];
    size_t emojCnt = m_MyApp.getEngine().getThumbMgr().getEmoticonIdList().size();
    if( emoticonNum > 0 && emoticonNum <= emojCnt )
    {
        if( emoticonNum > 9 )
        {
            sprintf( resBuf, ":/AppRes/Resources/emoj%d.svg", emoticonNum );
        }
        else
        {
            sprintf( resBuf, ":/AppRes/Resources/emoj0%d.svg", emoticonNum );
        }

        QPixmap faceImage( resBuf );
        retImage = faceImage.scaled( imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
        return !retImage.isNull();
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiThumbMgr::%s invalid icon number %d", __func__, emoticonNum );
        return false;
    }   
}

//============================================================================
void GuiThumbMgr::wantGuiThumbCallbacks( GuiThumbCallback* callback, bool wantCallback )
{
    static bool userCallbackShutdownComplete = false;
    if( VxIsAppShuttingDown() )
    {
        if( userCallbackShutdownComplete )
        {
            return;
        }

        userCallbackShutdownComplete = true;
        m_GuiThumbClientList.clear();
        return;
    }

    if( wantCallback )
    {
        for( auto& client : m_GuiThumbClientList )
        {
            if( client == callback )
            {
                LogMsg( LOG_INFO, "WARNING. Ignoring New wantToGuiUserUpdateCallbacks because already in list" );
                return;
            }
        }

        m_GuiThumbClientList.emplace_back( callback );
        return;
    }

    for( auto iter = m_GuiThumbClientList.begin(); iter != m_GuiThumbClientList.end(); ++iter )
    {
        GuiThumbCallback* client = *iter;
        if( client == callback )
        {
            m_GuiThumbClientList.erase( iter );
            return;
        }
    }

    LogMsg( LOG_INFO, "WARNING. wantToGuiUserUpdateCallbacks remove not found in list" );
}

//============================================================================
void GuiThumbMgr::addThumbIfDoesNotExist( ThumbInfo* thumbInfo )
{
    GuiThumb* guiThumb = m_ThumbList.findThumb( thumbInfo->getAssetUniqueId() ); 
    if( guiThumb )
    {
        return;
    }
    else
    {
        guiThumb = new GuiThumb( m_MyApp );
        guiThumb->setThumbInfo( *thumbInfo );
        if( m_ThumbList.addThumbIfDoesntExist( guiThumb ) )
        {
            m_MyApp.getEngine().getThumbMgr().fromGuiThumbCreated( *thumbInfo );
            onThumbAdded( guiThumb );
        }
    }
}