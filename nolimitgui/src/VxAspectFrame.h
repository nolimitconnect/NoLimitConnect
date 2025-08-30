#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFrame>

class VxLabel;

class VxAspectFrame : public QFrame
{
    Q_OBJECT

public:
    VxAspectFrame( QWidget* parent = nullptr );

    void                        setPreviewAspectRatio( float aspectRatio );
    float                       getPreviewAspectRatio( void )                       { return m_PreviewAspectRatio; }
    void                        setVideoScreenAspectRatio( float aspectRatio );
    float                       getVideoScreenAspectRatio( void )                   { return m_ScreenAspectRatio; }
   void                         setThumbnailPreview( VxLabel * thumbnailPreview )   { m_ThumbnailPreview = thumbnailPreview; }
    VxLabel *                   getThumbnailPreview( void )                         { return m_ThumbnailPreview; }
    void                        setVideoScreen( VxLabel * videoScreen )             { m_VideoScreen = videoScreen; }
    VxLabel *                   getVideoScreen( void )                              { return m_VideoScreen; }

protected:
    virtual void				resizeEvent( QResizeEvent* ev );
    virtual void                updatePositions( void );
    void						positionWidget( QWidget* parentWidget, float aspectRatio, VxLabel * widget, bool centerWidget, float scale );

    float                       m_PreviewAspectRatio;
    float                       m_ScreenAspectRatio;
    VxLabel *					m_ThumbnailPreview;
    float                       m_ThumbnailScale;
    VxLabel *					m_VideoScreen;
};

