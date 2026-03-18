//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================



#include "AppTranslate.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QGuiApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>

#include <CoreLib/VxGlobals.h>

#include <memory>

namespace
{
    std::unique_ptr<QTranslator> g_AppTranslator;

    QString languageCode( ELanguageType langType )
    {
        switch( langType )
        {
        case eLangEnglish:      return "en";
        case eLangGerman:       return "de";
        case eLangChinese:      return "zh";
        case eLangSpanish:      return "es";
        case eLangFrench:       return "fr";
        case eLangArabic:       return "ar";
        case eLangHindi:        return "hi";
        case eLangPortuguese:   return "pt";
        case eLangJapanese:     return "ja";
        case eLangKorean:       return "ko";
        case eLangRussian:      return "ru";
        case eLangIndonesian:   return "id";
        default:                return "";
        }
    }

    QString findTranslationFile( ELanguageType langType )
    {
        const QString langCode = languageCode( langType );
        if( langCode.isEmpty() )
        {
            return QString();
        }

        const std::string& translationsDir = VxGetTranslationsDirectory();
        if( translationsDir.empty() )
        {
            return QString();
        }

        const QStringList candidateDirs{ QString::fromUtf8( translationsDir.c_str() ) };
        for( const QString& dirPath : candidateDirs )
        {
            QDir dir( dirPath );
            if( !dir.exists() )
            {
                continue;
            }

            const QStringList fileNames = dir.entryList(
                QStringList() << QString( "nolimitconnect_%1*.qm" ).arg( langCode ),
                QDir::Files,
                QDir::Name );
            if( !fileNames.isEmpty() )
            {
                return dir.filePath( fileNames.first() );
            }
        }

        return QString();
    }
}

// for reference
// QT_TRANSLATE_NOOP( "QObject","Greek" ),      "el", QLocale::Greek );

QString AppTranslate::describeLanguage( ELanguageType langType )
{
    QString lang = "English";
    switch( langType )
    {
    case eLangUnspecified:
        lang = "Any";
        break;
    case eLangEnglish:
        lang = "English";
        break;
    case eLangBulgarian:
        lang = "Bulgarian";
        break;
    case eLangChinese:
        lang = "Chinese";
        break;
    case eLangCroatian:
        lang = "Croatian";
        break;
    case eLangCzech:
        lang = "Czech";
        break;
    case eLangDanish:
        lang = "Danish";
        break;
    case eLangDutch:
        lang = "Dutch";
        break;
    case eLangEstonian:
        lang = "Estonian";
        break;
    case eLangFinnish:
        lang = "Finnish";
        break;
    case eLangFrench:
        lang = "French";
        break;
    case eLangGerman:
        lang = "German";
        break;
    case eLangGreek:
        lang = "Greek";
        break;
    case eLangHindi:
        lang = "Hindi";
        break;
    case eLangHungarian:
        lang = "Hungarian";
        break;
    case eLangItalian:
        lang = "Italian";
        break;
    case eLangJapanese:
        lang = "Japanese";
        break;
    case eLangLatvian:
        lang = "Latvian";
        break;
    case eLangLithuanian:
        lang = "Lithuanian";
        break;
    case eLangPolish:
        lang = "Polish";
        break;
    case eLangPortuguese:
        lang = "Portuguese";
        break;
    case eLangRomanian:
        lang = "Romanian";
        break;
    case eLangRussian:
        lang = "Russian";
        break;
    case eLangSerbian:
        lang = "Serbian";
        break;
    case eLangSlovak:
        lang = "Slovak";
        break;
    case eLangSpanish:
        lang = "Spanish";
        break;
    case eLangSwedish:
        lang = "Swedish";
        break;
    case eLangThai:
        lang = "Thai";
        break;
    case eLangTurkish:
        lang = "Turkish";
        break;
    case eLangUkrainian:
        lang = "Ukrainian";
        break;
    case eLangArabic:
        lang = "Arabic";
        break;
    case eLangKorean:
        lang = "Korean";
        break;
    case eLangIndonesian:
        lang = "Indonesian";
        break;

    default:
        break;
    }

    return lang;
}

//============================================================================
QLocale AppTranslate::getLocale( ELanguageType langType )
{
    QLocale locale = QLocale::English;
    switch( langType )
    {
    case eLangEnglish:
        locale = QLocale::English;
        break;
    case eLangBulgarian:
        locale = QLocale::Bulgarian;
        break;
    case eLangChinese:
        locale = QLocale::Chinese;
        break;
    case eLangCroatian:
        locale = QLocale::Croatian;
        break;
    case eLangCzech:
        locale = QLocale::Czech;
        break;
    case eLangDanish:
        locale = QLocale::Danish;
        break;
    case eLangDutch:
        locale = QLocale::Dutch;
        break;
    case eLangEstonian:
        locale = QLocale::Estonian;
        break;
    case eLangFinnish:
        locale = QLocale::Finnish;
        break;
    case eLangFrench:
        locale = QLocale::French;
        break;
    case eLangGerman:
        locale = QLocale::German;
        break;
    case eLangGreek:
        locale = QLocale::Greek;
        break;
    case eLangHindi:
        locale = QLocale::Hindi;
        break;
    case eLangHungarian:
        locale = QLocale::Hungarian;
        break;
    case eLangItalian:
        locale = QLocale::Italian;
        break;
    case eLangJapanese:
        locale = QLocale::Japanese;
        break;
    case eLangLatvian:
        locale = QLocale::Latvian;
        break;
    case eLangLithuanian:
        locale = QLocale::Lithuanian;
        break;
    case eLangPolish:
        locale = QLocale::Polish;
        break;
    case eLangPortuguese:
        locale = QLocale::Portuguese;
        break;
    case eLangRomanian:
        locale = QLocale::Romanian;
        break;
    case eLangRussian:
        locale = QLocale::Russian;
        break;
    case eLangSerbian:
        locale = QLocale::Serbian;
        break;
    case eLangSlovak:
        locale = QLocale::Slovak;
        break;
    case eLangSpanish:
        locale = QLocale::Spanish;
        break;
    case eLangSwedish:
        locale = QLocale::Swedish;
        break;
    case eLangThai:
        locale = QLocale::Thai;
        break;
    case eLangTurkish:
        locale = QLocale::Turkish;
        break;
    case eLangUkrainian:
        locale = QLocale::Ukrainian;
        break;
    case eLangArabic:
        locale = QLocale::Arabic;
        break;
    case eLangKorean:
        locale = QLocale::Korean;
        break;
    case eLangIndonesian:
        locale = QLocale::Indonesian;
        break;

    default:
        break;
    }

    return locale;
}

//============================================================================
bool AppTranslate::applyLanguage( ELanguageType langType )
{
    QApplication* app = qobject_cast<QApplication*>( QCoreApplication::instance() );
    if( !app )
    {
        return false;
    }

    if( g_AppTranslator )
    {
        app->removeTranslator( g_AppTranslator.get() );
    }

    g_AppTranslator.reset( new QTranslator() );
    QLocale::setDefault( getLocale( langType ) );

    bool translationLoaded = true;
    if( eLangEnglish != langType )
    {
        const QString qmFile = findTranslationFile( langType );
        if( qmFile.isEmpty() || !g_AppTranslator->load( qmFile ) )
        {
            translationLoaded = false;
        }
        else
        {
            app->installTranslator( g_AppTranslator.get() );
        }
    }

    QGuiApplication::setApplicationDisplayName( QObject::tr( "No Limit Connect" ) );

    QEvent langChangeEvent( QEvent::LanguageChange );
    const auto widgets = QApplication::allWidgets();
    for( QWidget* widget : widgets )
    {
        if( widget )
        {
            QApplication::sendEvent( widget, &langChangeEvent );
            widget->update();
        }
    }

    return translationLoaded;
}
