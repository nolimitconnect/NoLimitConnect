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

#include "ActivityBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class SoundOptionsDialog;
}
QT_END_NAMESPACE

class VxNetIdent;

class ActivitySoundOptions : public ActivityBase
{
	Q_OBJECT
public:

	ActivitySoundOptions(	AppCommon& app, 
							QWidget*		parent = nullptr );
	virtual ~ActivitySoundOptions() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget*     getTitleBarWidget( void ) override;
    virtual BottomBarWidget*    getBottomBarWidget( void ) override;

public slots:
	void						onApplyButClick();
	void						onCancelButClick(); 

protected:
	//=== vars ===//
	Ui::SoundOptionsDialog&		ui;
	EPluginType					m_ePluginType;
	VxNetIdent*					m_Friend{ nullptr };

};
