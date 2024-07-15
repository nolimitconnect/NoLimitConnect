#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include <CoreLib/AssetDefs.h>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ConfirmRemoveMessageClass;
}
QT_END_NAMESPACE

class AssetBaseInfo;
class AppCommon;
class MyIcons;

class DialogConfirmRemoveMessage : public QDialog
{
	Q_OBJECT

public:
    DialogConfirmRemoveMessage( AssetBaseInfo& assetInfo, QWidget* parent = nullptr );
	virtual ~DialogConfirmRemoveMessage() = default;
	DialogConfirmRemoveMessage( QWidget* parent = nullptr ) = delete;// dont allow without asset info construct

	AppCommon&					getMyApp( void )					{ return m_MyApp; }
	MyIcons&					getMyIcons( void );

	EAssetAction				getAssetActionResult( void )		{ return m_AssetAction; }

protected slots:
	void						slotRemoveAssetButtonClicked( void );
	void						slotShredFileButtonClicked( void );

protected:
	Ui::ConfirmRemoveMessageClass&	ui;
	AppCommon&					m_MyApp;
    AssetBaseInfo&				m_AssetInfo;
	EAssetAction				m_AssetAction;
};
