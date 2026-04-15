#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputBaseWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class InputFaceWidgetClass;
}
QT_END_NAMESPACE

class VxLabel;

class InputFaceWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputFaceWidget( QWidget* parent = nullptr );

    void                        setCanSend( bool canSend ) {}

protected slots:
    void                        slotCancelFaceButtonClicked( void );

protected:
    void                        resizeEvent( QResizeEvent* ev ) override;
    void                        showEvent( QShowEvent* ev ) override;
    void                        faceLabelClicked( int faceNum );
    void                        repositionFaceTiles( bool force = false );
    void                        refreshScaledPixmaps( int tileSize );

    Ui::InputFaceWidgetClass&   ui;
    QWidget*                    m_FaceContentWidget{ nullptr };
    QVector<VxLabel*>           m_FaceList;
    QVector<QString>            m_FaceResourcePaths;
    QVector<QPixmap>            m_FacePixmaps;
    QVector<QPixmap>            m_ScaledFacePixmaps;
    int                         m_LastTileSize{ 0 };
    QSize                       m_LastFaceAreaSize;
};
