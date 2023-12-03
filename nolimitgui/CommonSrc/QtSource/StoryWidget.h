#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QMap>
#include <QPointer>
#include "ui_StoryWidget.h"

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QPrinter)

class StoryWidget : public QWidget
{
    Q_OBJECT

public:
    StoryWidget( QWidget* parent = nullptr, QString initialFile = "" );

	void						loadStoryBoardFile( const char* pFileName );
	bool						maybeSave();

signals:
	void						signalStoryBoardSavedModified();

private:
    void						setupFileActions();
    void						setupEditActions();
    void						setupTextActions();
    bool						load( const QString &fileName );
    void						setCurrentFileName( const QString &fileName );

private slots:
	bool						createLink( void );
	void						fileNew( void );
    void						fileOpen( void );
    bool						fileSave( void );
    bool						fileSaveAs( void );
    void						filePrint( void );
    void						filePrintPreview( void );
    void						filePrintPdf( void );

    void						textBold( void );
    void						textUnderline( void );
    void						textItalic( void );
    void						textFamily(const QString &f);
    void						textSize(const QString &p);
    void						textColor( void );

	void						leftJustify( void );
	void						centerJustify( void );
	void						rightJustify( void );
	void						bothJustify( void );

	void						insertPicture( void );
	void						editActionUndo( void );
	void						editActionRedo( void );
	void						editActionCut( void );
	void						editActionCopy( void );
	void						editActionPaste( void );

    void						currentCharFormatChanged(const QTextCharFormat &format);
    void						cursorPositionChanged( void );

    void						clipboardDataChanged( void );
    void						printPreview(QPrinter *);

private:
    void                        mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void                        fontChanged(const QFont &f);
    void                        colorChanged(const QColor &c);

    //=== vars ===//
    Ui::StoryWidgetClass        ui;
    QString                     m_strFileName;
    QTextEdit *                 m_TextEdit;
};


