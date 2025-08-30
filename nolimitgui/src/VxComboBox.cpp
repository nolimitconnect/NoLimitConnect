//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxComboBox.h"
#include "AppCommon.h"
#include "VxAppTheme.h"

#include <QPainter>
#include <QLineEdit>
#include <QPushButton>

//============================================================================
VxComboBox::VxComboBox(QWidget* parent) 
: QComboBox(parent) 
{
    setObjectName( "VxComboBox" );
}

/*
//============================================================================
void VxComboBox::paintEvent( QPaintEvent *ev )
{
    VxAppTheme& appTheme = GetAppInstance().getAppTheme();
    QPalette pal = appTheme.getBasePalette();
    pal.setColor( QPalette::Text, appTheme.getColor( eButtonTextNormal ) );
    setPalette( pal );
    QLineEdit * lineEditor = this->lineEdit();
    if( lineEditor )
    {
        lineEditor->setPalette( pal );
        lineEditor->setReadOnly( true );
   }

    QComboBox::paintEvent( ev );
 }*/

//============================================================================
void VxComboBox::showEvent( QShowEvent*ev )
{
    QComboBox::showEvent( ev );
}
