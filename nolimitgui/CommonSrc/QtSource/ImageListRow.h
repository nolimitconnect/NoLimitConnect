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

#include "ui_ImageListRow.h"

#include <QListWidgetItem>
#include <QObject>
#include <QPushButton>

class ThumbnailViewWidget;
class QHBoxLayout;
class VxGUID;

class ImageListRow : public QWidget, public QListWidgetItem
{
	Q_OBJECT;
public:
	ImageListRow( QListWidget * parent = 0 );
	virtual ~ImageListRow() = default;

    void                        addThumbnail( ThumbnailViewWidget * thumbnail );
    bool                        hasRoomForThumbnail( int idx = 0 );
    void                        setRowNum( int rowNum );

    void                        clearImages( void );
    void                        recalculateSizeHint( int listWidth, float displayScale );
    bool                        thumbExistsInList( VxGUID& assetId );

signals:
    void                        signalImageClicked( ThumbnailViewWidget * thumbnail );

protected slots:
    void                        slotImageClicked( ThumbnailViewWidget * thumbnail );

protected:
    virtual QSize				sizeHint() const override;

    //=== vars ===//
    Ui::ImageListRowUi	        ui;
    QSize                       m_SizeHint;
    bool                        m_LayoutInited = false;
    int                         m_ThumbnailCnt = 0;
};




