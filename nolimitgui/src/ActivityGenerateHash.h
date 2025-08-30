#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include <CoreLib/Sha1GeneratorCallback.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ActivityGenerateHashUi;
}
QT_END_NAMESPACE

class VxSha1Hash;

class ActivityGenerateHash : public ActivityBase, public Sha1GeneratorCallback
{
	Q_OBJECT
public:
	ActivityGenerateHash( AppCommon& app, QWidget* parent, std::string fileName, std::string fileNameAndPath, VxSha1Hash& hashId );
	virtual ~ActivityGenerateHash() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
	TitleBarWidget*				getTitleBarWidget( void ) override;
	BottomBarWidget*			getBottomBarWidget( void ) override;

	void						setTitle( QString strTitle );
	void						setBodyText( QString strBodyText );
	void						hideCancelButton( void );

signals:
	void						signalGenerateSha1Completed( ESha1GenResult genResult );

private slots:
	void						slotCancelButtonClicked( void );
	void						slotGenerateSha1Completed( ESha1GenResult genResult );

protected:
	virtual void				callbackSha1GenerateResult( ESha1GenResult sha1GenResult, VxGUID& fileId, Sha1Info& sha1Info ) override;

	//=== vars ===//
	Ui::ActivityGenerateHashUi&	ui;

	std::string					m_FileName;
	std::string					m_FileNameAndPath;
	VxSha1Hash&					m_HashId;
	VxGUID						m_HashInstanceId;
};


