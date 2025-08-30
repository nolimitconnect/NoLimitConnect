#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QMap>
#include <QPointer>
#include <QTextEdit>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QPrinter)

class RichTextWidget : public QTextEdit
{
    Q_OBJECT

public:
    RichTextWidget( QWidget* parent = nullptr, QString initialFile = "" );

	void						loadRichTextFile( const char* pFileName );
	bool						maybeSave();

private:
    bool						load( const QString &fileName );
    void						setCurrentFileName( const QString &fileName );

private slots:
    void						printPreview(QPrinter *);

private:
    void                        fileOpen( void );
    bool                        fileSave( void );
    bool                        fileSaveAs( void );

    void                        filePrint( void );
    void                        filePrintPreview( void );
    void                        filePrintPdf( void );

    //=== vars ===//
    QString                     m_strFileName;
};


