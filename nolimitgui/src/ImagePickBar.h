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

#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ImagePickBarClass;
}
QT_END_NAMESPACE

class GuiUser;
class GuiOfferSession;
class QLabel;
class AppCommon;
class MyIcons;

class ImagePickBar : public QWidget
{
    Q_OBJECT

public:
    ImagePickBar( QWidget* parent = nullptr );
    virtual ~ImagePickBar(){};


    Ui::ImagePickBarClass&      ui;
    AppCommon &					m_MyApp;

};



