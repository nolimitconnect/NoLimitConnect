#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include <CoreLib/Sha1GeneratorCallback.h>

#include "ui_ActivityGenerateHash.h"

class ActivityGenerateHash : public ActivityBase, public Sha1GeneratorCallback
{
	Q_OBJECT
public:
	ActivityGenerateHash( AppCommon& app, QWidget* parent, std::string fileName, VxSha1Hash& hashId );
	virtual ~ActivityGenerateHash() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	void						setTitle( QString strTitle );
	void						setBodyText( QString strBodyText );
	void						hideCancelButton( void );

signals:
	void						signalGenerateSha1Completed( ESha1GenResult genResult );

private slots:
    void						slotHomeButtonClicked( void ) override;
	void						slotCancelButtonClicked( void );
	void						slotGenerateSha1Completed( ESha1GenResult genResult );

protected:
	virtual void				callbackSha1GenerateResult( ESha1GenResult sha1GenResult, VxGUID& fileId, Sha1Info& sha1Info ) override;

	//=== vars ===//
	Ui::ActivityGenerateHashUi	ui;

	std::string					m_FileName;
	VxSha1Hash&					m_HashId;
	VxGUID						m_HashInstanceId;
};


