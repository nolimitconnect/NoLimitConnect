#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include <GuiInterface/IToGui.h>
#include <PktLib/SearchParams.h>

#include "ThumbnailChooseWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class SearchParamsUi;
}
QT_END_NAMESPACE

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

class SearchParamsWidget : public QWidget
{
	Q_OBJECT
public:
    SearchParamsWidget( QWidget* parent );
	virtual ~SearchParamsWidget() = default;

    void						setupSearchParamsWidget( EApplet applet, EPluginType pluginType );

    virtual QLineEdit *         getSearchTextEdit();

    virtual QLabel *            getSearchDescriptionLabel();

    virtual QPushButton *       getStartButton();
    virtual QPushButton *       getStopButton();

    virtual QComboBox *         getSearchTypeComboBox();
    virtual QComboBox *         getAgeComboBox();
    virtual QComboBox *         getContentRatingComboBox();
    virtual QComboBox *         getGenderComboBox();
    virtual QComboBox *         getLanguageComboBox();

    virtual bool                toSearchParams( SearchParams& params );
    virtual bool                fromSearchParams( SearchParams& params );

    virtual void                setSearchListAll( bool listAll );
    virtual bool                getSearchListAll( void );

signals:
    void                        signalSearchState(bool searchStarted);

public slots:
    virtual void                slotSearchCancel();
    virtual void                slotSearchComplete();

protected slots:
    virtual void                slotStartSearch();
    virtual void                slotStopSearch();

protected:
    EPluginType                 getPluginType() { return m_PluginType; }

    //=== vars ===//
    Ui::SearchParamsUi&         ui;
    AppCommon&                  m_MyApp;
    EApplet                     m_EAppletType{ eAppletUnknown };
    EPluginType                 m_PluginType{ ePluginTypeInvalid };
    AppletBase*                 m_ParentApplet{ nullptr };
    int                         m_SubPluginType{ 0 };
    bool                        m_OrigPermissionIsSet{ false };
    EFriendState                m_OrigPermissionLevel{ eFriendStateIgnore };
    bool                        m_PermissionsConnected{ false };
    SearchParams                m_SearchParams;
    bool                        m_SearchStarted{ false };
};


