//============================================================================
// Copyright (C) 2017 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletMgr.h"

#include "ActivityBase.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include "AppletApplicationInfo.h"
#include "AppletAboutApp.h"
#include "AppletAboutFile.h"
#include "AppletAboutMeClient.h"
#include "AppletAboutUser.h"

#include "AppletBrowseFiles.h"

#include "AppletCamClient.h"
#include "AppletCamSettings.h"

#include "AppletChatRoomClient.h"
#include "AppletChatRoomHostAdmin.h"
#include "AppletChatRoomJoin.h"
#include "AppletChatRoomJoinSearch.h"
#include "AppletChatRoomListLocalView.h"

#include "AppletChooseThumbnail.h"

#include "AppletDownloads.h"
#include "AppletEditAboutMe.h"
#include "AppletEditAvatarImage.h"
#include "AppletEditStoryboard.h"
#include "AppletFileOfferSelect.h"
#include "AppletFriendList.h"
#include "AppletFriendRequest.h"
#include "AppletFriendRequestList.h"

#include "AppletGalleryEmoticon.h"
#include "AppletGalleryImage.h"
#include "AppletGalleryThumb.h"
#include "AppletGetStarted.h"

#include "AppletGroupClient.h"
#include "AppletGroupHostAdmin.h"
#include "AppletGroupJoin.h"
#include "AppletGroupJoinSearch.h"
#include "AppletGroupListClient.h"
#include "AppletGroupListLocalView.h"

#include "AppletHackerList.h"
#include "AppletHelpNetSignalBars.h"

#include "AppletHostJoinChoose.h"
#include "AppletHostJoinConnect.h"
#include "AppletHostJoinRequestList.h"
#include "AppletHostLeave.h"

#include "AppletHostChatRoomStatus.h"
#include "AppletHostGroupStatus.h"
#include "AppletHostNetworkStatus.h"
#include "AppletHostRandomConnectStatus.h"

#include "AppletIgnoredHosts.h"
#include "AppletIsPortOpenTest.h"
#include "AppletInviteAccept.h"
#include "AppletInviteCreate.h"
#include "AppletInvites.h"

#include "AppletLanguageSelect.h"
#include "AppletLibrary.h"
#include "AppletLog.h"
#include "AppletLogSettings.h"

#include "AppletMgrCallback.h"

#include "AppletNetHostingPage.h"
#include "AppletNetworkSettings.h"

#include "AppletOfferInfo.h"
#include "AppletOfferResponse.h"
#include "AppletOfferResponseAccept.h"
#include "AppletOfferSend.h"
#include "AppletOfferView.h"

#include "AppletPeerSessionFileOffer.h"
#include "AppletPermissionList.h"
#include "AppletPersonalRecorder.h"
#include "AppletPersonOfferList.h"

#include "AppletPlayerCamClip.h"
#include "AppletPlayerPhoto.h"
#include "AppletPlayerNlc.h"
#include "AppletPlayerVideo.h"

#include "AppletPopupMenu.h"

#include "AppletRandomConnectClient.h"
#include "AppletRandomConnectHostAdmin.h"
#include "AppletRandomConnectJoin.h"
#include "AppletRandomConnectJoinSearch.h"
#include "AppletRandomConnectListLocalView.h"

#include "AppletServiceAboutMe.h"

#include "AppletServiceConnectionTest.h"
#include "AppletServiceHostNetwork.h"
#include "AppletServiceShareFiles.h"
#include "AppletServiceShareWebCam.h"
#include "AppletServiceStoryboard.h"

#include "AppletSettingsAboutMe.h"
#include "AppletSettingsAvatarImage.h"
#include "AppletSettingsConnectionTest.h"
#include "AppletSettingsFileXfer.h"
#include "AppletSettingsFriendRequest.h"

#include "AppletSettingsHostChatRoom.h"
#include "AppletSettingsHostGroup.h"
#include "AppletSettingsHostNetwork.h"
#include "AppletSettingsHostRandomConnect.h"

#include "AppletSettingsMessenger.h"
#include "AppletSettingsPushToTalk.h"
#include "AppletSettingsRandomConnect.h"
#include "AppletSettingsShareFiles.h"
#include "AppletSettingsShareWebCam.h"
#include "AppletSettingsStoryboard.h"
#include "AppletSettingsTruthOrDare.h"
#include "AppletSettingsVideoPhone.h"
#include "AppletSettingsVoicePhone.h"

#include "AppletSettingsPage.h"
#include "AppletSearchPage.h"

#include "AppletOfferList.h"
#include "AppletShareServicesPage.h"
#include "AppletSnapshot.h"
#include "AppletSocketList.h"
#include "AppletSoundSettings.h"
#include "AppletStoryboardClient.h"

#include "AppletTestAndDebug.h"
#include "AppletTestUpnp.h"

#include "AppletTheme.h"
#include "AppletUploads.h"

#include "AppletMultiMessenger.h"
#include "AppletPeerChangeFriendship.h"
#include "AppletPeerReplyFileOffer.h"
#include "AppletPeerTodGame.h"
#include "AppletPeerVideoPhone.h"
#include "AppletFileShareClientView.h"
#include "AppletPeerVoicePhone.h"

#include "AppletAboutMeServerViewMine.h"
#include "AppletCamServerViewMine.h"
#include "AppletFileShareServerViewMine.h"
#include "AppletStoryboardServerViewMine.h"

#include "AppletUserConnections.h"
#include "AppletUserIdentity.h"
#include "AppletUserPreferences.h"

#include "GuiParams.h"
#include "HomeWindow.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>

#include <QApplication>

//============================================================================
AppletMgr::AppletMgr( AppCommon& myMpp, QWidget* parent )
: QWidget( parent )
, m_MyApp( myMpp )
{
}

//============================================================================
RenderGlWidget* AppletMgr::getRenderConsumer( void )
{
    RenderGlWidget * renderConsumer = nullptr;
    for( auto iter = m_ActivityList.begin(); iter != m_ActivityList.end(); ++iter )
    {
        if( eAppletPlayerNlc == (*iter)->getAppletType()  )
        {
            if( (*iter)->isVisible() )
            {
                renderConsumer = ( ( AppletPlayerNlc * )( *iter ) )->getRenderConsumer();
            }

            break;
        }
    }

    return renderConsumer;
}

//============================================================================
QWidget* AppletMgr::getActiveWindow( void )
{
	QWidget* activeWidget = QApplication::activeWindow();
	if( 0 == activeWidget )
	{
		activeWidget = &m_MyApp.getHomeWindow();
	}

	return activeWidget;
}

//============================================================================
QFrame* AppletMgr::getAppletFrame( EApplet applet )
{
	return m_MyApp.getAppletFrame( applet );
}

//============================================================================
ActivityBase* AppletMgr::launchApplet( EApplet applet, QWidget* parent, QString launchParam, VxGUID assetId )
{
    // these are permanent applets
    if( eAppletMultiMessenger == applet )
    {
        if( parent && m_MyApp.getAppletMultiMessenger()->parent() != parent )
        {
            m_MyApp.getAppletMultiMessenger()->setNewParentPage( parent );
        }

        bringAppletToFront( m_MyApp.getAppletMultiMessenger() );
        return m_MyApp.getAppletMultiMessenger();
    }
    else if( eAppletDownloads == applet )
    {
        if( parent && m_MyApp.getAppletDownloads()->parent() != parent )
        {
            m_MyApp.getAppletDownloads()->setNewParentPage( parent );
        }

        bringAppletToFront( m_MyApp.getAppletDownloads() );
        return m_MyApp.getAppletDownloads();
    }
    else  if( eAppletUploads == applet )
    {
        if( launchAppletAllowed( eAppletUploads ) )
        {
            if( parent && m_MyApp.getAppletUploads()->parent() != parent )
            {
                m_MyApp.getAppletUploads()->setNewParentPage( parent );
            }

            bringAppletToFront( m_MyApp.getAppletUploads() );
            return m_MyApp.getAppletUploads();
        }
        else
        {
            return nullptr;
        }
    }

    // other applets
	ActivityBase* appletDialog = findAppletDialog( applet );
	if( appletDialog )
	{
        if( parent && appletDialog->parent() != parent )
        {
            appletDialog->setNewParentPage( parent );
        }

		bringAppletToFront( appletDialog );
        if( applet == eAppletPlayerCamClip )
        {
            AppletPlayerCamClip* player = dynamic_cast<AppletPlayerCamClip*>(appletDialog);
            if( player )
            {
                player->setPlayerAssetId( assetId );
            }          
        }
        // TODO figure out why on button click and attempt to launch the applet is done multiple times
//#ifdef DEBUG
//        m_MyApp.errMessageBox2( QObject::tr( "AppletMgr::launchApplet" ).toUtf8().constData(), QObject::tr( "Applet enum %d already launched\n" ).toUtf8().constData(), applet );
//#endif // DEBUG

		return appletDialog;
	}

	if( 0 == parent )
	{
		parent = getActiveWindow();
	}

    QWidget* launchFrame = m_MyApp.getHomeWindow().getLaunchPageFrame();

	QString appletMissingTitle = QObject::tr( "Applet Not Yet Implemented" );
	switch( applet )
	{
    case eAppletAboutMeServerViewMine:
        if( viewMyServerAllowed( eAppletAboutMeServerViewMine ) )
        {
            AppletAboutMeServerViewMine* myViewDlg = new AppletAboutMeServerViewMine( m_MyApp, parent );
            if( myViewDlg )
            {
                myViewDlg->setIdentity( m_MyApp.getUserMgr().getMyIdent() );
                appletDialog = myViewDlg;
            }
        }
        break;

    case eAppletCamServerViewMine:
        if( viewMyServerAllowed( eAppletCamServerViewMine ) )
        {
            AppletCamServerViewMine* myViewDlg = new AppletCamServerViewMine( m_MyApp, parent );
            if( myViewDlg )
            {
                myViewDlg->setupCamFeed( m_MyApp.getUserMgr().getMyIdent() );
                appletDialog = myViewDlg;
            }
        }
        break;

    case eAppletStoryboardServerViewMine:
        if( viewMyServerAllowed( eAppletStoryboardServerViewMine ) )
        {
            AppletStoryboardServerViewMine* myViewDlg = new AppletStoryboardServerViewMine( m_MyApp, parent );
            if( myViewDlg )
            {
                myViewDlg->setIdentity( m_MyApp.getUserMgr().getMyIdent() );
                appletDialog = myViewDlg;
            }
        }
        break;

    case eAppletFileShareServerViewMine:
        if( viewMyServerAllowed( eAppletFileShareServerViewMine ) )
        {
            AppletFileShareServerViewMine* myViewDlg = new AppletFileShareServerViewMine( m_MyApp, parent );
            if( myViewDlg )
            {
                myViewDlg->setIdentity( m_MyApp.getUserMgr().getMyIdent() );
                appletDialog = myViewDlg;
            }
        }
        break;

    case eAppletOfferInfo:                  if( launchAppletAllowed( eAppletOfferInfo ) ) appletDialog = new AppletOfferInfo( m_MyApp, parent ); break;
    case eAppletOfferResponse:              if( launchAppletAllowed( eAppletOfferResponse ) ) appletDialog = new AppletOfferResponse( m_MyApp, parent ); break;
    case eAppletOfferResponseAccept:        if( launchAppletAllowed( eAppletOfferResponseAccept ) ) appletDialog = new AppletOfferResponseAccept( m_MyApp, parent ); break;
    case eAppletOfferSend:                  if( launchAppletAllowed( eAppletOfferSend ) ) appletDialog = new AppletOfferSend( m_MyApp, parent ); break;
    case eAppletOfferView:                  if( launchAppletAllowed( eAppletOfferView ) ) appletDialog = new AppletOfferView( m_MyApp, parent ); break;
    case eAppletFileOfferSelect:            if( launchAppletAllowed( eAppletFileOfferSelect ) ) appletDialog = new AppletFileOfferSelect( m_MyApp, parent ); break;
    case eAppletFileShareClientView:        if( launchAppletAllowed( eAppletFileShareClientView ) ) appletDialog = new AppletFileShareClientView( m_MyApp, parent ); break;

    case eAppletAboutFile:                  if( launchAppletAllowed( eAppletAboutFile ) ) appletDialog = new AppletAboutFile( m_MyApp, parent ); break;
    case eAppletAboutMeClient:              if( launchAppletAllowed( eAppletAboutMeClient ) ) appletDialog = new AppletAboutMeClient( m_MyApp, parent ); break;
    case eAppletAboutNoLimitConnect:        if( launchAppletAllowed( eAppletAboutNoLimitConnect ) ) appletDialog = new AppletAboutApp( m_MyApp, parent ); break;
    case eAppletAboutUser:                  if( launchAppletAllowed( eAppletAboutUser ) ) appletDialog = new AppletAboutUser( m_MyApp, parent ); break;
    case eAppletApplicationInfo:            if( launchAppletAllowed( eAppletApplicationInfo ) ) appletDialog = new AppletApplicationInfo( m_MyApp, parent ); break;
    case eAppletBrowseFiles:                if( launchAppletAllowed( eAppletBrowseFiles ) ) appletDialog = new AppletBrowseFiles( m_MyApp, parent, launchParam ); break;

    case eAppletCamClient:                  if( launchAppletAllowed( eAppletCamClient ) ) appletDialog = new AppletCamClient( m_MyApp, parent ); break;
    case eAppletChatRoomJoin:               if( launchAppletAllowed( eAppletChatRoomJoin ) ) appletDialog = new AppletChatRoomJoin( m_MyApp, parent ); break;
    case eAppletChatRoomJoinSearch:         if( launchAppletAllowed( eAppletChatRoomJoinSearch ) ) appletDialog = new AppletChatRoomJoinSearch( m_MyApp, parent ); break;
    case eAppletChatRoomListLocalView:      if( launchAppletAllowed( eAppletChatRoomListLocalView ) ) appletDialog = new AppletChatRoomListLocalView( m_MyApp, parent ); break;
    case eAppletChooseThumbnail:            if( launchAppletAllowed( eAppletChooseThumbnail ) ) appletDialog = new AppletChooseThumbnail( m_MyApp, parent ); break;

    case eAppletEditAboutMe:                if( launchAppletAllowed( eAppletEditAboutMe ) ) appletDialog = new AppletEditAboutMe( m_MyApp, parent ); break;
    case eAppletEditAvatarImage:            if( launchAppletAllowed( eAppletEditAvatarImage ) ) appletDialog = new AppletEditAvatarImage( m_MyApp, parent ); break;
    case eAppletEditStoryboard:             if( launchAppletAllowed( eAppletEditStoryboard ) ) appletDialog = new AppletEditStoryboard( m_MyApp, parent ); break;
    case eAppletGalleryEmoticon:            if( launchAppletAllowed( eAppletGalleryEmoticon ) ) appletDialog = new AppletGalleryEmoticon( m_MyApp, parent ); break;
    case eAppletGalleryImage:               if( launchAppletAllowed( eAppletGalleryImage ) ) appletDialog = new AppletGalleryImage( m_MyApp, parent ); break;
    case eAppletGalleryThumb:               if( launchAppletAllowed( eAppletGalleryThumb ) ) appletDialog = new AppletGalleryThumb( m_MyApp, parent ); break;

    case eAppletGetStarted:                 if( launchAppletAllowed( eAppletGetStarted ) ) appletDialog = new AppletGetStarted( m_MyApp, parent ); break;
    case eAppletGroupJoin:                  if( launchAppletAllowed( eAppletGroupJoin ) ) appletDialog = new AppletGroupJoin( m_MyApp, parent ); break;
    case eAppletGroupJoinSearch:            if( launchAppletAllowed( eAppletGroupJoinSearch ) ) appletDialog = new AppletGroupJoinSearch( m_MyApp, parent ); break;
    case eAppletGroupListLocalView:         if( launchAppletAllowed( eAppletGroupListLocalView ) ) appletDialog = new AppletGroupListLocalView( m_MyApp, parent ); break;
    case eAppletHelpNetSignalBars:          if( launchAppletAllowed( eAppletHelpNetSignalBars ) ) appletDialog = new AppletHelpNetSignalBars( m_MyApp, parent ); break;
    case eAppletHomePage:                   m_MyApp.errMessageBox(appletMissingTitle, "Home Page Not Implemented"); return nullptr;

    case eAppletRandomConnectHostAdmin:     if( launchAppletAllowed( eAppletRandomConnectHostAdmin ) ) appletDialog = new AppletRandomConnectHostAdmin( m_MyApp, parent ); break;
    case eAppletRandomConnectJoin:          if( launchAppletAllowed( eAppletRandomConnectJoin ) ) appletDialog = new AppletRandomConnectJoin( m_MyApp, parent ); break;
    case eAppletRandomConnectJoinSearch:    if( launchAppletAllowed( eAppletRandomConnectJoinSearch ) ) appletDialog = new AppletRandomConnectJoinSearch( m_MyApp, parent ); break;
    case eAppletRandomConnectListLocalView: if( launchAppletAllowed( eAppletRandomConnectListLocalView ) ) appletDialog = new AppletRandomConnectListLocalView( m_MyApp, parent ); break;

    case eAppletSoundSettings:              if( launchAppletAllowed( eAppletSoundSettings ) ) appletDialog = new AppletSoundSettings( m_MyApp, parent ); break;
    case eAppletStoryboardClient:           if( launchAppletAllowed( eAppletStoryboardClient ) ) appletDialog = new AppletStoryboardClient( m_MyApp, parent ); break;
    case eAppletLog:                        if( launchAppletAllowed( eAppletLog ) ) appletDialog = new AppletLog( m_MyApp, parent ); break;


    case eAppletLibrary:                    if( launchAppletAllowed( eAppletLibrary ) ) appletDialog = new AppletLibrary( m_MyApp, parent, launchParam ); break;
    case eAppletLogSettings:                if( launchAppletAllowed( eAppletLogSettings ) ) appletDialog = new AppletLogSettings( m_MyApp, parent ); break;

    case eAppletMessengerFrame:             makeMessengerFullSized(); return appletDialog;

    case eAppletNetworkSettings:            if( launchAppletAllowed( eAppletNetworkSettings ) ) appletDialog = new AppletNetworkSettings( m_MyApp, parent ); break;
    case eAppletPersonalRecorder:           if( launchAppletAllowed( eAppletPersonalRecorder ) ) appletDialog = new AppletPersonalRecorder( m_MyApp, parent ); break;
    case eAppletSettingsPage:               if( launchAppletAllowed( eAppletSettingsPage ) ) appletDialog = new AppletSettingsPage( m_MyApp, parent ); break;

    case eAppletSearchPage:	                if( launchAppletAllowed( eAppletSearchPage ) ) appletDialog = new AppletSearchPage( m_MyApp, parent ); break;
 
    case eAppletPlayerCamClip:              if( launchAppletAllowed( eAppletPlayerCamClip ) ) appletDialog = new AppletPlayerCamClip( m_MyApp, parent, assetId ); break;
    case eAppletCamSettings:                if( launchAppletAllowed( eAppletCamSettings ) ) appletDialog = new AppletCamSettings( m_MyApp, parent ); break;
        
        
    case eAppletGroupClient:                if( launchAppletAllowed( eAppletGroupClient ) ) appletDialog = new AppletGroupClient( m_MyApp, parent ); break;
    case eAppletChatRoomClient:             if( launchAppletAllowed( eAppletChatRoomClient ) ) appletDialog = new AppletChatRoomClient( m_MyApp, parent ); break;
    case eAppletRandomConnectClient:        if( launchAppletAllowed( eAppletRandomConnectClient ) ) appletDialog = new AppletRandomConnectClient( m_MyApp, parent ); break;

    case eAppletFriendList:                 if( launchAppletAllowed( eAppletFriendList ) ) appletDialog = new AppletFriendList( m_MyApp, parent ); break;
    case eAppletFriendRequest:              if( launchAppletAllowed( eAppletFriendRequest ) ) appletDialog = new AppletFriendRequest( m_MyApp, parent ); break;
    case eAppletFriendRequestList:          if( launchAppletAllowed( eAppletFriendRequestList ) ) appletDialog = new AppletFriendRequestList( m_MyApp, parent ); break;
       
    case eAppletGroupListClient:            if( launchAppletAllowed( eAppletGroupListClient ) ) appletDialog = new AppletGroupListClient( m_MyApp, parent ); break;

    case eAppletGroupHostAdmin:             if( launchAppletAllowed( eAppletGroupHostAdmin ) ) appletDialog = new AppletGroupHostAdmin( m_MyApp, parent ); break;

    case eAppletChatRoomHostAdmin:          if( launchAppletAllowed( eAppletChatRoomHostAdmin ) ) appletDialog = new AppletChatRoomHostAdmin( m_MyApp, parent ); break;

    case eAppletHostJoinConnect:            if( launchAppletAllowed( eAppletHostJoinConnect ) ) appletDialog = new AppletHostJoinConnect( m_MyApp, parent, launchParam.toUtf8().constData() ); break;
    case eAppletHostJoinChoose:               if( launchAppletAllowed( eAppletHostJoinChoose ) ) appletDialog = new AppletHostJoinChoose( m_MyApp, parent ); break;
    case eAppletHostJoinRequestList:        if( launchAppletAllowed( eAppletHostJoinRequestList ) ) appletDialog = new AppletHostJoinRequestList( m_MyApp, parent ); break;

    case eAppletHostChatRoomStatus:         if( launchAppletAllowed( eAppletHostChatRoomStatus ) ) appletDialog = new AppletHostChatRoomStatus( m_MyApp, parent ); break;
    case eAppletHostGroupStatus:            if( launchAppletAllowed( eAppletHostGroupStatus ) ) appletDialog = new AppletHostGroupStatus( m_MyApp, parent ); break;
    case eAppletHostLeave:                  if( launchAppletAllowed( eAppletHostLeave ) ) appletDialog = new AppletHostLeave( m_MyApp, parent ); break;
    case eAppletHostNetworkStatus:          if( launchAppletAllowed( eAppletHostNetworkStatus ) ) appletDialog = new AppletHostNetworkStatus( m_MyApp, parent ); break;
    case eAppletHostRandomConnectStatus:    if( launchAppletAllowed( eAppletHostRandomConnectStatus ) ) appletDialog = new AppletHostRandomConnectStatus( m_MyApp, parent ); break;

    case eAppletInviteAccept:               if( launchAppletAllowed( eAppletInviteAccept ) ) appletDialog = new AppletInviteAccept( m_MyApp, parent ); break;
    case eAppletInviteCreate:               if( launchAppletAllowed( eAppletInviteCreate ) ) appletDialog = new AppletInviteCreate( m_MyApp, parent ); break;

    case eAppletLanguageSelect:             if( launchAppletAllowed( eAppletLanguageSelect ) ) appletDialog = new AppletLanguageSelect( m_MyApp, parent ); break;

    case eAppletNetHostingPage:             if( launchAppletAllowed( eAppletNetHostingPage ) ) appletDialog = new AppletNetHostingPage( m_MyApp, parent ); break;
    case eAppletPersonOfferList:            if( launchAppletAllowed( eAppletPersonOfferList ) ) appletDialog = new AppletPersonOfferList( m_MyApp, parent ); break;
    case eAppletPopupMenu:                  if( launchAppletAllowed( eAppletPopupMenu ) ) appletDialog = new AppletPopupMenu( m_MyApp, parent ); break;
    case eAppletShareServicesPage:          if( launchAppletAllowed( eAppletShareServicesPage ) ) appletDialog = new AppletShareServicesPage( m_MyApp, parent ); break;

    case eAppletServiceAboutMe:             if( launchAppletAllowed( eAppletServiceAboutMe ) ) appletDialog = new AppletServiceAboutMe( m_MyApp, parent ); break;

    case eAppletServiceConnectionTest:      if( launchAppletAllowed( eAppletServiceConnectionTest ) ) appletDialog = new AppletServiceConnectionTest( m_MyApp, parent ); break;

    case eAppletPermissionList:             if( launchAppletAllowed( eAppletPermissionList ) ) appletDialog = new AppletPermissionList( m_MyApp, parent ); break;
    case eAppletServiceHostNetwork:         if( launchAppletAllowed( eAppletServiceHostNetwork ) ) appletDialog = new AppletServiceHostNetwork( m_MyApp, parent ); break;
    
    case eAppletServiceShareFiles:          if( launchAppletAllowed( eAppletServiceShareFiles ) ) appletDialog = new AppletServiceShareFiles( m_MyApp, parent ); break;
    case eAppletServiceShareWebCam:         if( launchAppletAllowed( eAppletServiceShareWebCam ) ) appletDialog = new AppletServiceShareWebCam( m_MyApp, parent ); break;
    case eAppletServiceStoryboard:          if( launchAppletAllowed( eAppletServiceStoryboard ) ) appletDialog = new AppletServiceStoryboard( m_MyApp, parent ); break;

    case eAppletSettingsAboutMe:            if( launchAppletAllowed( eAppletSettingsAboutMe ) ) appletDialog = new AppletSettingsAboutMe( m_MyApp, parent ); break;
    case eAppletSettingsAvatarImage:        if( launchAppletAllowed( eAppletSettingsAvatarImage ) ) appletDialog = new AppletSettingsAvatarImage( m_MyApp, parent ); break;
    case eAppletSettingsConnectTest:        if( launchAppletAllowed( eAppletSettingsConnectTest ) ) appletDialog = new AppletSettingsConnectionTest( m_MyApp, parent ); break;
    case eAppletSettingsFileXfer:           if( launchAppletAllowed( eAppletSettingsFileXfer ) ) appletDialog = new AppletSettingsFileXfer( m_MyApp, parent ); break;
    case eAppletSettingsFriendRequest:      if( launchAppletAllowed( eAppletSettingsFriendRequest ) ) appletDialog = new AppletSettingsFriendRequest( m_MyApp, parent ); break;

    case eAppletSettingsHostChatRoom:       if( launchAppletAllowed( eAppletSettingsHostChatRoom ) ) appletDialog = new AppletSettingsHostChatRoom( m_MyApp, parent ); break;
    case eAppletSettingsHostGroup:          if( launchAppletAllowed( eAppletSettingsHostGroup ) ) appletDialog = new AppletSettingsHostGroup( m_MyApp, parent ); break;
    case eAppletSettingsHostNetwork:        if( launchAppletAllowed( eAppletSettingsHostNetwork ) ) appletDialog = new AppletSettingsHostNetwork( m_MyApp, parent ); break;
    case eAppletSettingsHostRandomConnect:  if( launchAppletAllowed( eAppletSettingsHostRandomConnect ) ) appletDialog = new AppletSettingsHostRandomConnect( m_MyApp, parent ); break;

    case eAppletSettingsMessenger:          if( launchAppletAllowed( eAppletSettingsMessenger ) ) appletDialog = new AppletSettingsMessenger( m_MyApp, parent ); break;
    case eAppletSettingsPushToTalk:         if( launchAppletAllowed( eAppletSettingsPushToTalk ) ) appletDialog = new AppletSettingsPushToTalk( m_MyApp, parent ); break;
    case eAppletSettingsShareFiles:         if( launchAppletAllowed( eAppletSettingsShareFiles ) ) appletDialog = new AppletSettingsShareFiles( m_MyApp, parent ); break;
    case eAppletSettingsStoryboard:         if( launchAppletAllowed( eAppletSettingsStoryboard ) ) appletDialog = new AppletSettingsStoryboard( m_MyApp, parent ); break;
    case eAppletSettingsTruthOrDare:        if( launchAppletAllowed( eAppletSettingsTruthOrDare ) ) appletDialog = new AppletSettingsTruthOrDare( m_MyApp, parent ); break;
    case eAppletSettingsVideoPhone:         if( launchAppletAllowed( eAppletSettingsVideoPhone ) ) appletDialog = new AppletSettingsVideoPhone( m_MyApp, parent ); break;
    case eAppletSettingsVoicePhone:         if( launchAppletAllowed( eAppletSettingsVoicePhone ) ) appletDialog = new AppletSettingsTruthOrDare( m_MyApp, parent ); break;
    case eAppletSettingsWebCamServer:       if( launchAppletAllowed( eAppletSettingsWebCamServer ) ) appletDialog = new AppletSettingsShareWebCam( m_MyApp, parent ); break;
    case eAppletOfferList:                  if( launchAppletAllowed( eAppletOfferList ) ) appletDialog = new AppletOfferList( m_MyApp, parent ); break;
    case eAppletSnapshot:                   if( launchAppletAllowed( eAppletSnapshot ) ) appletDialog = new AppletSnapshot( m_MyApp, parent ); break;

    case eAppletTestAndDebug:               if( launchAppletAllowed( eAppletTestAndDebug ) ) appletDialog = new AppletTestAndDebug( m_MyApp, parent ); break;
    case eAppletTestUpnp:                   if( launchAppletAllowed( eAppletTestUpnp ) ) appletDialog = new AppletTestUpnp( m_MyApp, parent ); break;
    case eAppletTheme:                      if( launchAppletAllowed( eAppletTheme ) ) appletDialog = new AppletTheme( m_MyApp, parent ); break;

    case eAppletUserIdentity:               if( launchAppletAllowed( eAppletUserIdentity ) ) appletDialog = new AppletUserIdentity( m_MyApp, parent ); break;
    case eAppletUserPreferences:            if( launchAppletAllowed( eAppletUserPreferences ) ) appletDialog = new AppletUserPreferences( m_MyApp, parent ); break;

    case eAppletMultiMessenger:	            if( launchAppletAllowed( eAppletMultiMessenger ) ) appletDialog = new AppletMultiMessenger( m_MyApp, parent ); break;
    case eAppletPeerChangeFriendship:	    if( launchAppletAllowed( eAppletPeerChangeFriendship ) ) appletDialog = new AppletPeerChangeFriendship( m_MyApp, parent ); break;
    case eAppletPeerReplyOfferFile:         if( launchAppletAllowed( eAppletPeerReplyOfferFile ) ) appletDialog = new AppletPeerReplyFileOffer( m_MyApp, parent ); break;
    case eAppletPeerTruthOrDare:            if( launchAppletAllowed( eAppletPeerTruthOrDare ) ) appletDialog = new AppletPeerTodGame( m_MyApp, parent ); break;
    case eAppletPeerVideoPhone:             if( launchAppletAllowed( eAppletPeerVideoPhone ) ) appletDialog = new AppletPeerVideoPhone( m_MyApp, parent ); break;
    case eAppletPeerVoicePhone:             if( launchAppletAllowed( eAppletPeerVoicePhone ) ) appletDialog = new AppletPeerVoicePhone( m_MyApp, parent ); break;

    case eAppletPeerSessionFileOffer:       if( launchAppletAllowed( eAppletPeerSessionFileOffer ) ) appletDialog = new AppletPeerSessionFileOffer( m_MyApp, parent ); break;

    case eAppletIgnoredHosts:               if( launchAppletAllowed( eAppletIgnoredHosts ) ) appletDialog = new AppletIgnoredHosts( m_MyApp, parent ); break;

    case eAppletPlayerPhoto:                if( launchAppletAllowed( eAppletPlayerPhoto ) ) appletDialog = new AppletPlayerPhoto( m_MyApp, parent ); break;
    case eAppletPlayerVideo:                if( launchAppletAllowed( eAppletPlayerVideo ) ) appletDialog = new AppletPlayerVideo( m_MyApp, parent ); break;
    case eAppletPlayerNlc:                  if( launchAppletAllowed( eAppletPlayerNlc ) ) appletDialog = new AppletPlayerNlc( m_MyApp, parent ); break;

    case eAppletIsPortOpenTest:             if( launchAppletAllowed( eAppletIsPortOpenTest ) ) appletDialog = new AppletIsPortOpenTest( m_MyApp, parent ); break;
    case eAppletHackerList:                 if( launchAppletAllowed( eAppletHackerList ) ) appletDialog = new AppletHackerList( m_MyApp, parent ); break;
    case eAppletSocketList:                 if( launchAppletAllowed( eAppletSocketList ) ) appletDialog = new AppletSocketList( m_MyApp, parent ); break;

    case eAppletUserConnections:            if( launchAppletAllowed( eAppletUserConnections ) ) appletDialog = new AppletUserConnections( m_MyApp, parent ); break;

    case eAppletUnknown:                    m_MyApp.errMessageBox( appletMissingTitle, QObject::tr( "Unknown Or Not Implemented" ) ); return nullptr;

	default:
		m_MyApp.errMessageBox2( QObject::tr( "AppCommon::launchApplet").toUtf8().constData(), QObject::tr( "Invalid Applet enum %d" ).toUtf8().constData(), applet );
		return nullptr;
	}

	if( appletDialog )
	{
        if( GuiParams::isLaunchOnStartupApplet( applet ) )
        {
            AppSettings& appSettings = m_MyApp.getAppSettings();
            QString parentFrameName = appletDialog->getParentPageFrameName();
            if( OBJNAME_FRAME_LAUNCH_PAGE == parentFrameName )
            {
                appSettings.setLastAppletLaunched( eLaunchFrameHome, applet );  
            }
            else if( OBJNAME_FRAME_MESSAGER_PAGE == parentFrameName )
            {
                appSettings.setLastAppletLaunched( eLaunchFrameMessenger, applet );  
            }
        }

        appletDialog->aboutToLaunchApplet();
        appletDialog->show();
        if( !findAppletDialog( appletDialog ) )
        {
            m_ActivityList.emplace_back( appletDialog );
            announceAppletState( applet, true );
        }
    }

    return appletDialog;
}

//============================================================================
bool AppletMgr::isAppletLaunched( EApplet applet )
{
    if( eAppletMultiMessenger == applet ||
        eAppletDownloads == applet ||
        eAppletUploads == applet )
    {
        return true;
    }

    ActivityBase* appletDialog = findAppletDialog( applet );
    return appletDialog != nullptr;
}

//============================================================================
void AppletMgr::activityStateChange( ActivityBase* activity, bool isCreated )
{
	if( !isCreated )
	{
        removeApplet( activity );
	}
	else
	{
		addApplet( activity );
	}
}

//============================================================================
void AppletMgr::bringAppletToFront( ActivityBase* appletDialog )
{
    if( appletDialog )
    {
        if( !appletDialog->isVisible() )
        {
            appletDialog->show();
        }

        appletDialog->activateWindow();
        appletDialog->raise();
    }
}

//============================================================================
ActivityBase* AppletMgr::findAppletDialog( EApplet applet )
{
    for( auto activityInList : m_ActivityList )
    {
        if( applet == activityInList->getAppletType() )
        {
            return activityInList;
        }
    }
	
	return nullptr;
}

//============================================================================
ActivityBase* AppletMgr::findAppletDialog( ActivityBase* activity )
{
    if( activity )
    {
        for( auto activityInList : m_ActivityList )
        {
            if( activity == activityInList )
            {
                return activityInList;
            }
        }
    }

    return nullptr;
}

//============================================================================
void AppletMgr::addApplet( ActivityBase* activity )
{
	if( 0 == findAppletDialog( activity ) )
	{
        m_ActivityList.emplace_back( activity );
        announceAppletState( activity->getAppletType(), true );
	}
	else 
	{
		LogMsg( LOG_ERROR, " AppletMgr::addApplet: adding already existing applet" );
        vx_assert( false );
	}
}

//============================================================================
void AppletMgr::removeApplet( EApplet applet )
{
	QVector<ActivityBase*>::iterator iter;
	for( iter = m_ActivityList.begin(); iter != m_ActivityList.end(); ++iter )
	{
		if( applet == ( *iter )->getAppletType() )
		{
            m_ActivityList.erase( iter );
            announceAppletState( applet, false );

            AppSettings& appSettings = m_MyApp.getAppSettings();
            if( appSettings.getLastAppletLaunched( eLaunchFrameHome ) == applet )
            {
                appSettings.setLastAppletLaunched( eLaunchFrameHome, eAppletUnknown );
            }
            else if( appSettings.getLastAppletLaunched( eLaunchFrameMessenger ) == applet )
            {
                appSettings.setLastAppletLaunched( eLaunchFrameMessenger, eAppletUnknown );
            }

			break;
		}
	}
}

//============================================================================
void AppletMgr::removeApplet( ActivityBase* activity )
{
    removeApplet( activity->getAppletType() );
}

//============================================================================
void AppletMgr::makeMessengerFullSized( void )
{
    m_MyApp.getHomeWindow().setIsMaxScreenSize( true, true );
}

//============================================================================
bool AppletMgr::viewMyServerAllowed( EApplet applet )
{
    EPluginType pluginType{ ePluginTypeInvalid };
    switch( applet )
    {
    case eAppletAboutMeServerViewMine:
        pluginType = ePluginTypeAboutMePageServer;
        break;
    case eAppletCamServerViewMine:
        pluginType = ePluginTypeCamServer;
        break;
    case eAppletFileShareServerViewMine:
        pluginType = ePluginTypeFileShareServer;
        break;
    case eAppletStoryboardServerViewMine:
        pluginType = ePluginTypeStoryboardServer;
        break;
    default:
        break;
    }

    if( ePluginTypeInvalid == pluginType )
    {
        QMessageBox::information( this, QObject::tr( "Cannot View Disabled Service" ), 
            QObject::tr( "Unknown Plugin Service" ), QMessageBox::Ok );
        return false;
    }

    bool isEnabled = isServiceEnabled( pluginType ); 
    if( !isEnabled )
    {
        QMessageBox::information( this, QObject::tr( "Cannot View Disabled Service" ),
            GuiParams::describePluginType( pluginType ) + QObject::tr( " Cannot be viewed when permission is disable" ), QMessageBox::Ok );
        return false;
    }

    return true;
}

//============================================================================
bool AppletMgr::isServiceEnabled( EPluginType pluginType )
{
    return m_MyApp.getUserMgr().getMyIdent()->getPluginPermission( pluginType ) != eFriendStateIgnore;
}

//============================================================================
bool AppletMgr::launchAppletAllowed( EApplet applet )
{
    bool launchAllowed{ true };
    if( eAppletInviteCreate == applet )
    {
        if( !m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen() )
        {
            GuiHelpers::showRequiresOpenPort( this );
            return false;
        }
    }

    return launchAllowed;
}

//============================================================================
QFrame* AppletMgr::getLaunchParentFrame( ELaunchFrame launchFrame )
{
    return m_MyApp.getHomeWindow().getLaunchParentFrame( launchFrame );
}

//============================================================================
bool AppletMgr::launchClientApplet( GuiHosted* guiHosted, QWidget* parentFrame )
{
    bool isLaunched{ false };
    if( guiHosted && guiHosted->readyForClientLaunch() )
	{
		if( eHostTypePeerUser == guiHosted->getHostType() )
		{
			AppletMultiMessenger* messenger = m_MyApp.getAppletMultiMessenger();
			if( messenger )
			{
				messenger->userJoinedHost( guiHosted );
				return true;
			}
		}
		else
		{
			AppletClientBase* clientApplet{ nullptr };
            switch( guiHosted->getHostType() )
            {
            case eHostTypeGroup:
                if( !isAppletLaunched( eAppletGroupClient ) )
                {
                    clientApplet = dynamic_cast<AppletClientBase*>(m_MyApp.getAppletMgr().launchApplet( eAppletGroupClient, parentFrame ) );
                }

                break;

            case eHostTypeChatRoom:
                if( !isAppletLaunched( eAppletChatRoomClient ) )
                {
                    clientApplet = dynamic_cast<AppletClientBase*>(m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomClient, parentFrame ) );
                }

                break;

            case eHostTypeRandomConnect:
                if( !isAppletLaunched( eAppletRandomConnectClient ) )
                {
                    clientApplet = dynamic_cast<AppletClientBase*>(m_MyApp.getAppletMgr().launchApplet( eAppletRandomConnectClient, parentFrame ) );
                }

                break;

            default:
                break;
            }

            if( clientApplet )
            {
                clientApplet->userJoinedHost( guiHosted );
                return true;
            }
		}
	}

    return isLaunched;
}

//============================================================================
void AppletMgr::wantAppletMgrCallback( AppletMgrCallback* client, bool wantCallback )
{
    for( auto iter = m_AppletMgrClients.begin(); iter != m_AppletMgrClients.end(); ++iter )
    {
        if( *iter == client )
        {
            m_AppletMgrClients.erase( iter );
            break;
        }
    }

    if( wantCallback )
    {
        m_AppletMgrClients.emplace_back( client );
    }
}

//============================================================================
void AppletMgr::announceAppletState( EApplet applet, bool isOpen )
{
    LogMsg( LOG_VERBOSE, "AppletMgr::%s applet %s isOpen %d", __func__, DescribeApplet( applet ).toUtf8().constData(), isOpen );
    for( auto& client : m_AppletMgrClients )
    {
        client->callbackAppletIsOpen( applet, isOpen );
    }
}
