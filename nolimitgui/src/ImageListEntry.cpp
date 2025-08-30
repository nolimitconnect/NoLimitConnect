//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "ImageListEntry.h"
#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>
#include <AssetMgr/AssetInfo.h>
#include <AssetMgr/AssetMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <VxVideoLib/VxVideoLib.h>

#include <QMessageBox>
#include <QPainter>

//============================================================================
ImageListEntry::ImageListEntry( QWidget* parent )
    : ThumbnailViewWidget( parent )
{
}

//============================================================================
bool ImageListEntry::loadFromAsset( AssetInfo * thumbAsset )
{
    bool loadOk = false;
    if( thumbAsset )
    {
        loadOk = loadFromFile( thumbAsset->getAssetNameAndPath().c_str() );
    }

    return loadOk;
}

//============================================================================
void ImageListEntry::slotThumbGalleryClick( void )
{

}
