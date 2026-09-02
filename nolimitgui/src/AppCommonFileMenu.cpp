//============================================================================
// Copyright (C) 2013 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "AppletPopupMenu.h"
#include "MyIcons.h"
#include "AppGlobals.h"

#include "AppletUploads.h"
#include "AppletDownloads.h"
#include "ActivityBrowseFiles.h"

#include "AppletPersonalRecorder.h"

#include <P2PEngine/P2PEngine.h>
#include <P2PEngine/FileShareSettings.h>
#include <P2PEngine/EngineSettings.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
void AppCommon::onMenuFileSelected( int iMenuId, PopupMenu * senderPopupMenu, ActivityBase* activityBase )
{
	Q_UNUSED( senderPopupMenu );
	switch( iMenuId )
	{
	case 1:
		{
			AppletPersonalRecorder * dlg = new AppletPersonalRecorder(	*this, nullptr );
			dlg->show();
			break;
		}

	case 2: // 
		{
			ActivityBrowseFiles dlg( *this, eFileFilterAll, 0 );
			dlg.exec();
			break;
		}

	case 5: // View Downloading Files
        if( getAppletDownloads() )
        {
            getAppletDownloads()->show();
        }
		break;

	case 6: // View Uploading Files
        if( getAppletUploads() )
        {
            getAppletUploads()->show();
        }
		break;

	case 7: // Open Download Folder
		{
			FileShareSettings fileShareSettings;
			getEngine().fromGuiGetFileShareSettings( fileShareSettings );

			std::string strExploreCmd = "explorer ";
			strExploreCmd += VxGetDownloadsDirectory();
			system( strExploreCmd.c_str() );
			break;
		}

	case 8: // File Share Options
		{
			//ActivityShareFilesSetup dlg( *this, this );
			//dlg.exec();
			break;
		}

	default: 
		LogMsg( LOG_ERROR, "onMenuFileSelected: ERROR unknown menu id %d", iMenuId );
		break;
	}
}
