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

#include <QLabel>

class VxLabel : public QLabel 
{
	Q_OBJECT
public:
    VxLabel( QWidget* parent=nullptr, Qt::WindowFlags f=Qt::Widget );
    VxLabel( const QString &text, QWidget* parent=nullptr, Qt::WindowFlags f=Qt::Widget );
    virtual ~VxLabel() = default;

	virtual void				setTextBreakAnywhere( QString text, int maxLines = 2 );

	void						setResourceImage( QString resourceUrl, bool scaleToLabelSize = true );
	bool						setImageFromFile( QString fileName );

	void						playMotionVideoFrame( QImage& vidFrame, int motion0To100000 );
	void                        playVideoFrame( QImage& vidFrame );
	void						playRotatedVideoFrame( QImage& vidFrame, int iRotate );

	void						setVidImageRotation( int imageRotation )		{ m_VidImageRotation = imageRotation; }
	int							getVidImageRotation( void )						{ return m_VidImageRotation; }

	QImage						getLastVideoImage( void )						{ return m_PreResizeImage; }
	void						showScaledImage( QImage& picBitmap );

signals:
	void						clicked();

protected:
	void						resizeBitmapToFitScreen( QImage& picBitmap );
	bool						loadImageFromFile( QString fileName );

	virtual void				mousePressEvent( QMouseEvent * ev ) override;
	virtual void				resizeEvent( QResizeEvent* ev ) override;

	//=== vars ===//
	QString						m_ImageFileName;
	QString						m_OrigText;
	int							m_MaxLines{ 1 };
    int							m_VidImageRotation{ 0 };
	QImage						m_PreResizeImage;
};

