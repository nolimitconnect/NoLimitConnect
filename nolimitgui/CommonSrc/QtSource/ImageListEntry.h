//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include "ThumbnailViewWidget.h"

class AssetInfo;  

class ImageListEntry : public ThumbnailViewWidget
{
    Q_OBJECT
public:
    ImageListEntry( QWidget* parent );
    ~ImageListEntry() override = default;

    bool                        loadFromAsset( AssetInfo * thumbAsset );

protected slots:
    void                        slotThumbGalleryClick( void );

protected:

};
