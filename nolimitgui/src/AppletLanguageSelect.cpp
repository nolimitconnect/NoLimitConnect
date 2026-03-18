//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLanguageSelect.h"

#include "ActivityMsgBoxYesNo.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "AppSettings.h"
#include "AppTranslate.h"
#include "GuiHelpers.h"

#include <P2PEngine/EngineSettings.h>
#include <P2PEngine/P2PEngine.h>

#include <GuiInterface/IDefs.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

#include "ui_AppletLanguageSelect.h"

namespace
{
    static const ELanguageType kSupportedLanguages[] =
    {
        eLangEnglish,
        eLangGerman,
        eLangChinese,
        eLangSpanish,
        eLangFrench,
        eLangArabic,
        eLangHindi,
        eLangPortuguese,
        eLangJapanese,
        eLangKorean,
        eLangRussian,
        eLangIndonesian,
    };
}

//============================================================================
AppletLanguageSelect::AppletLanguageSelect( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_LANGUAGE_SELECT, app, parent )
, ui(*(new Ui::LanguageSelectUi ))
{
    setAppletType( eAppletLanguageSelect );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    populateLanguageComboBox();

    VxNetIdent* myIdent = m_MyApp.getAppGlobals().getMyNetIdent();
    if( myIdent )
    {
        int selectedIdx = ui.m_LanguageComboBox->findData( (int)myIdent->getPrimaryLanguage() );
        if( 0 <= selectedIdx )
        {
            ui.m_LanguageComboBox->setCurrentIndex( selectedIdx );
        }
    }

    updateLanguagePreview( GuiHelpers::getLanguage( ui.m_LanguageComboBox ) );

    connect( ui.m_LanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLanguageComboIndexChanged(int)) );
    connect( ui.m_ApplyButton, SIGNAL(clicked()), this, SLOT(slotApplyButtonClicked() ) );
}

//============================================================================
void AppletLanguageSelect::slotLanguageComboIndexChanged( int index )
{
    Q_UNUSED( index );
    updateLanguagePreview( GuiHelpers::getLanguage( ui.m_LanguageComboBox ) );
}

//============================================================================
void AppletLanguageSelect::slotApplyButtonClicked( void )
{
    qDebug() << "Language selected: " << ui.m_LanguageComboBox->currentText();
    onLanguageSelected();
}

//============================================================================
void AppletLanguageSelect::onLanguageSelected( void )
{
    ELanguageType selectedLanguage = GuiHelpers::getLanguage( ui.m_LanguageComboBox );
    m_MyApp.getAppSettings().setSelectedLanguage( selectedLanguage );

    VxNetIdent* myIdent = m_MyApp.getAppGlobals().getMyNetIdent();
    if( !myIdent )
    {
        LogMsg( LOG_ERROR, "AppletLanguageSelect::onLanguageSelected null myIdent" );
        return;
    }

    if( myIdent->getPrimaryLanguage() == selectedLanguage )
    {
        updateLanguagePreview( selectedLanguage );
        return;
    }

    bool translationLoaded = AppTranslate::applyLanguage( selectedLanguage );
    myIdent->setPrimaryLanguage( selectedLanguage );
    m_MyApp.getAccountMgr().updateAccount( *myIdent );
    m_MyApp.getUserMgr().updateMyIdent( myIdent );
    getFromGuiInterface().fromGuiIdentPersonalInfoChanged(
        myIdent->getAgeType(),
        myIdent->getGender(),
        myIdent->getPrimaryLanguage(),
        myIdent->getPreferredContent() );

    populateLanguageComboBox();
    GuiHelpers::setLanguage( ui.m_LanguageComboBox, selectedLanguage );
    updateLanguagePreview( selectedLanguage );

    if( translationLoaded )
    {
        QMessageBox::information( this, QObject::tr( "Language" ), QObject::tr( "Language updated." ) );
    }
    else
    {
        QMessageBox::information(
            this,
            QObject::tr( "Language" ),
            QObject::tr( "Language updated, but no translation file was found in the translations folder." ) );
    }
}

//============================================================================
void AppletLanguageSelect::retranslateUi( void )
{
    const ELanguageType selectedLanguage = GuiHelpers::getLanguage( ui.m_LanguageComboBox );
    ui.retranslateUi( getContentItemsFrame() );
    populateLanguageComboBox();
    GuiHelpers::setLanguage( ui.m_LanguageComboBox, selectedLanguage );
    updateLanguagePreview( selectedLanguage );
    AppletBase::retranslateUi();
}

//============================================================================
QString AppletLanguageSelect::getTranslatedTitleBarText( void ) const
{
    return previewSelectLanguageText( GuiHelpers::getLanguage( ui.m_LanguageComboBox ) );
}

//============================================================================
void AppletLanguageSelect::populateLanguageComboBox( void )
{
    ui.m_LanguageComboBox->clear();
    for( ELanguageType language : kSupportedLanguages )
    {
        QString comboText = QString( "%1 (%2)" )
            .arg( nativeLanguageName( language ) )
            .arg( translatedLanguageName( language ) );
        ui.m_LanguageComboBox->addItem( comboText, (int)language );
    }
}

//============================================================================
QString AppletLanguageSelect::nativeLanguageName( ELanguageType language ) const
{
    switch( language )
    {
    case eLangEnglish:      return QString::fromUtf8( "English" );
    case eLangGerman:       return QString::fromUtf8( "Deutsch" );
    case eLangChinese:      return QString::fromUtf8( "简体中文" );
    case eLangSpanish:      return QString::fromUtf8( "Español" );
    case eLangFrench:       return QString::fromUtf8( "Français" );
    case eLangArabic:       return QString::fromUtf8( "العربية" );
    case eLangHindi:        return QString::fromUtf8( "हिन्दी" );
    case eLangPortuguese:   return QString::fromUtf8( "Português" );
    case eLangJapanese:     return QString::fromUtf8( "日本語" );
    case eLangKorean:       return QString::fromUtf8( "한국어" );
    case eLangRussian:      return QString::fromUtf8( "Русский" );
    case eLangIndonesian:   return QString::fromUtf8( "Bahasa Indonesia" );
    default:                return QString::fromUtf8( "English" );
    }
}

//============================================================================
QString AppletLanguageSelect::translatedLanguageName( ELanguageType language ) const
{
    switch( language )
    {
    case eLangEnglish:      return QObject::tr( "English" );
    case eLangGerman:       return QObject::tr( "German" );
    case eLangChinese:      return QObject::tr( "Chinese" );
    case eLangSpanish:      return QObject::tr( "Spanish" );
    case eLangFrench:       return QObject::tr( "French" );
    case eLangArabic:       return QObject::tr( "Arabic" );
    case eLangHindi:        return QObject::tr( "Hindi" );
    case eLangPortuguese:   return QObject::tr( "Portuguese" );
    case eLangJapanese:     return QObject::tr( "Japanese" );
    case eLangKorean:       return QObject::tr( "Korean" );
    case eLangRussian:      return QObject::tr( "Russian" );
    case eLangIndonesian:   return QObject::tr( "Indonesian" );
    default:                return QObject::tr( "English" );
    }
}

//============================================================================
void AppletLanguageSelect::updateLanguagePreview( ELanguageType language )
{
    const QString selectLanguageText = previewSelectLanguageText( language );
    ui.m_SelectLanguageLabel->setText( selectLanguageText );
    ui.m_ApplyButton->setText( previewApplyText( language ) );
    setTitleBarText( selectLanguageText );
}

//============================================================================
QString AppletLanguageSelect::previewSelectLanguageText( ELanguageType language ) const
{
    switch( language )
    {
    case eLangGerman:       return QString::fromUtf8( "Sprache auswählen" );
    case eLangChinese:      return QString::fromUtf8( "选择语言" );
    case eLangSpanish:      return QString::fromUtf8( "Seleccionar idioma" );
    case eLangFrench:       return QString::fromUtf8( "Choisir la langue" );
    case eLangArabic:       return QString::fromUtf8( "اختر اللغة" );
    case eLangHindi:        return QString::fromUtf8( "भाषा चुनें" );
    case eLangPortuguese:   return QString::fromUtf8( "Selecionar idioma" );
    case eLangJapanese:     return QString::fromUtf8( "言語を選択" );
    case eLangKorean:       return QString::fromUtf8( "언어 선택" );
    case eLangRussian:      return QString::fromUtf8( "Выбрать язык" );
    case eLangIndonesian:   return QString::fromUtf8( "Pilih Bahasa" );
    case eLangEnglish:
    default:                return QString::fromUtf8( "Select Language" );
    }
}

//============================================================================
QString AppletLanguageSelect::previewApplyText( ELanguageType language ) const
{
    switch( language )
    {
    case eLangGerman:       return QString::fromUtf8( "Anwenden" );
    case eLangChinese:      return QString::fromUtf8( "应用" );
    case eLangSpanish:      return QString::fromUtf8( "Aplicar" );
    case eLangFrench:       return QString::fromUtf8( "Appliquer" );
    case eLangArabic:       return QString::fromUtf8( "تطبيق" );
    case eLangHindi:        return QString::fromUtf8( "लागू करें" );
    case eLangPortuguese:   return QString::fromUtf8( "Aplicar" );
    case eLangJapanese:     return QString::fromUtf8( "適用" );
    case eLangKorean:       return QString::fromUtf8( "적용하기" );
    case eLangRussian:      return QString::fromUtf8( "Применить" );
    case eLangIndonesian:   return QString::fromUtf8( "Terapkan" );
    case eLangEnglish:
    default:                return QString::fromUtf8( "Apply" );
    }
}
