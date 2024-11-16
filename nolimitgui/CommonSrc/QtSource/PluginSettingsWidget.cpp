//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginSettingsWidget.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIconsDefs.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "ThumbnailViewWidget.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

#include "ui_PluginSettingsWidget.h"

QLabel *            PluginSettingsWidget::getServiceUrlLabel()        { return ui.m_UrlDescriptionLabel; }
QLineEdit *         PluginSettingsWidget::getServiceUrlEdit()         { return ui.m_UrlEdit; }
QLineEdit *         PluginSettingsWidget::getServiceTitleEdit()       { return ui.m_ServiceTitleEdit; }

QPlainTextEdit *    PluginSettingsWidget::getServiceDescriptionEdit() { return ui.m_DescriptionEdit; }
QPlainTextEdit *    PluginSettingsWidget::getGreetingEdit()           { return ui.m_GreetingEdit; }
QPlainTextEdit *    PluginSettingsWidget::getRejectEdit()             { return ui.m_RejectEdit; }

QPushButton *       PluginSettingsWidget::getStartButton()            { return ui.m_StartButton; }
QPushButton *       PluginSettingsWidget::getStopButton()             { return ui.m_StopButton; }
QPushButton *       PluginSettingsWidget::getApplyButton()            { return ui.m_ApplyButton; }

QComboBox *         PluginSettingsWidget::getAgeComboBox()            { return ui.m_AgeComboBox; }
QComboBox *         PluginSettingsWidget::getContentRatingComboBox()  { return ui.m_ContentRatingComboBox; }
QComboBox *         PluginSettingsWidget::getGenderComboBox()         { return ui.m_GenderComboBox; }
QComboBox *         PluginSettingsWidget::getLanguageComboBox()       { return ui.m_LanguageComboBox; }

PermissionWidget *  PluginSettingsWidget::getPermissionWidget()       { return ui.m_PermissionWidget; }
InformationWidget * PluginSettingsWidget::getInformationWidget()      { return ui.m_InfoWidget; }

ThumbnailChooseWidget * PluginSettingsWidget::getThumbnailChooseWidget()  { return ui.m_ThumbnailChooseWidget; }

//============================================================================
PluginSettingsWidget::PluginSettingsWidget( QWidget* parent )
    : QWidget( parent )
    , ui(*(new Ui::PluginSettingsUi))
    , m_MyApp( GetAppInstance() )
{
    m_ParentApplet = GuiHelpers::findParentApplet( parent );

    ui.setupUi( this );
    connect( ui.m_ThumbnailChooseWidget, SIGNAL( signalThumbnailAssetChanged( ThumbInfo* ) ), this, SLOT( slotThumbnailAssetChanged( ThumbInfo* ) ) );
    connect( ui.m_RunOnStartupCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotRunOnStartupCheckBoxChange(int)) );

    ui.m_DescriptionEdit->setFixedHeight( GuiParams::getButtonSize( eButtonSizeSmall ).height() );
    ui.m_GreetingEdit->setFixedHeight( GuiParams::getButtonSize( eButtonSizeSmall ).height() );
    ui.m_RejectEdit->setFixedHeight( GuiParams::getButtonSize( eButtonSizeSmall ).height() );
    ui.m_RunOnStartupCheckBox->setVisible( false );
}

//============================================================================
void PluginSettingsWidget::setupSettingsWidget( EApplet applet, EPluginType pluginType )
{
    m_EAppletType = applet;
    m_PluginType = pluginType;

    m_PluginSetting.setPluginType( pluginType );

    getServiceUrlLabel()->setVisible( false );
    getServiceUrlEdit()->setVisible( false );

    getStartButton()->setVisible( false );
    getStopButton()->setVisible( false );
    getInformationWidget()->setVisible( false );

    GuiHelpers::fillAge( ui.m_AgeComboBox );
    GuiHelpers::fillGender( ui.m_GenderComboBox );
    GuiHelpers::fillLanguage( ui.m_LanguageComboBox );
    GuiHelpers::fillContentRating( ui.m_ContentRatingComboBox );

    if( ePluginTypeInvalid == pluginType )
    {
        pluginType = GuiHelpers::getAppletAssociatedPlugin( m_EAppletType );
    }

    if( ePluginTypeInvalid != pluginType )
    {
        getPermissionWidget()->setPluginType( pluginType );
        ui.m_InfoWidget->setPluginType( pluginType );
    }

    if( ePluginTypeCamServer == m_PluginType )
    {
        ui.m_RunOnStartupCheckBox->setVisible( true );
        ui.m_RunOnStartupCheckBox->setChecked( m_MyApp.getAppSettings().getRunOnStartupCamServer() );
    }

    if( ePluginTypeFileShareServer == m_PluginType )
    {
        ui.m_RunOnStartupCheckBox->setVisible( true );
        ui.m_RunOnStartupCheckBox->setChecked( m_MyApp.getAppSettings().getRunOnStartupFileShareServer() );
    }

    connectServiceWidgets();
}

//============================================================================
void PluginSettingsWidget::connectServiceWidgets()
{
    connect( ui.m_ApplyButton, SIGNAL(clicked()), this, SLOT( slotApplyServiceSettings() ) );
}

//============================================================================
void PluginSettingsWidget::loadPluginSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        m_OrigPermissionLevel = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( getPluginType() );
        getPermissionWidget()->setPermissionLevel( m_OrigPermissionLevel );
        m_MyApp.getEngine().getPluginSettingMgr().getPluginSetting( getPluginType(), m_PluginSetting );
        loadUiFromSetting();
    }
}

//============================================================================
void PluginSettingsWidget::savePluginSetting()
{
    if( ( ePluginTypeInvalid != getPluginType() ) && ( ePluginTypeInvalid != m_PluginSetting.getPluginType() ) )
    {
        if( ePluginTypeCamServer == getPluginType() )
        {
            m_MyApp.getAppSettings().setRunOnStartupCamServer( ui.m_RunOnStartupCheckBox->isChecked() );
        }

        if( ePluginTypeFileShareServer == getPluginType() )
        {
            m_MyApp.getAppSettings().setRunOnStartupFileShareServer( ui.m_RunOnStartupCheckBox->isChecked() );
        }

        saveUiToSetting();
        m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );
    }
}

//============================================================================
void PluginSettingsWidget::loadUiFromSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        ui.m_ContentRatingComboBox->setCurrentIndex( GuiHelpers::contentRatingToIndex( m_PluginSetting.getContentRating() ) );
        ui.m_LanguageComboBox->setCurrentIndex( GuiHelpers::languageToIndex( m_PluginSetting.getLanguage() ) );
        ui.m_UrlEdit->setText( m_PluginSetting.getPluginUrl().c_str() );
        ui.m_ServiceTitleEdit->setText( m_PluginSetting.getTitle().c_str() );
        ui.m_DescriptionEdit->appendPlainText( m_PluginSetting.getDescription().c_str() );
        ui.m_ThumbnailChooseWidget->loadThumbnail( m_PluginSetting.getThumnailId(), m_PluginSetting.getThumbnailIsCircular() );
    }
}

//============================================================================
void PluginSettingsWidget::saveUiToSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        m_PluginSetting.setContentRating( ( EContentRating)ui.m_ContentRatingComboBox->currentIndex() );
        m_PluginSetting.setLanguage( ( ELanguageType )ui.m_LanguageComboBox->currentIndex() );
        m_PluginSetting.setPluginUrl( ui.m_UrlEdit->text().toUtf8().constData() );
        m_PluginSetting.setTitle( ui.m_ServiceTitleEdit->text().toUtf8().constData() );
        m_PluginSetting.setThumnailId( ui.m_ThumbnailChooseWidget->getThumbnailId(), ui.m_ThumbnailChooseWidget->getThumbnailIsCircular() );

        QString description = ui.m_DescriptionEdit->toPlainText().trimmed();
        if( !description.isEmpty() )
        {
            m_PluginSetting.setDescription( description.toUtf8().constData() );
        }
        else
        {
            m_PluginSetting.setDescription( "" );
        }
    }
}

//============================================================================
void PluginSettingsWidget::slotApplyServiceSettings()
{
    saveUiToSetting();
    m_PluginSetting.setUpdateTimestampToNow();
    m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );
    if( getPluginType() == ePluginTypeCamServer )
    {
        if( getPermissionWidget()->getPermissionLevel() != eFriendStateIgnore )
        {
            m_MyApp.getFromGuiInterface().fromGuiStartPluginSession( ePluginTypeCamServer, m_MyApp.getMyNetIdent()->getMyOnlineId(), 0, m_MyApp.getMyNetIdent()->getMyOnlineId() );
        }
        else
        {
            m_MyApp.getFromGuiInterface().fromGuiStopPluginSession( ePluginTypeCamServer, m_MyApp.getMyNetIdent()->getMyOnlineId(), 0, m_MyApp.getMyNetIdent()->getMyOnlineId() );
        }
    }

    emit signalPluginSettingsApplied();
}

//============================================================================
void PluginSettingsWidget::slotThumbnailAssetChanged( ThumbInfo* thumbAsset )
{
    VxGUID thumbGuid = ui.m_ThumbnailChooseWidget->getAssetId();
    LogMsg( LOG_VERBOSE, "slotThumbnailAssetChanged %s", thumbGuid.toGUIDStandardFormatedString().c_str() );
    m_PluginSetting.setThumnailId( thumbGuid, ui.m_ThumbnailChooseWidget->getThumbnailIsCircular() );
    ui.m_ThumbnailChooseWidget->getThumbnailViewWidget()->updateAssetImage( thumbAsset );
    emit signalThumbnailAssetChanged( thumbGuid, thumbAsset->isCircular() );
}

//============================================================================
void PluginSettingsWidget::slotRunOnStartupCheckBoxChange( int runOnStartup )
{

}