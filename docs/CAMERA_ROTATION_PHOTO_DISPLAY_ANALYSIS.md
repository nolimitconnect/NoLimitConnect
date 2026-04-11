# Camera Rotation Settings Applied to Photo Display - Code Analysis

## Summary
Camera rotation settings are **NOT** automatically applied when loading photos via `AppletPlayerPhoto` or `AssetPhotoWidget`. The rotation is only applied to camera capture streams and manually in photo viewers.

---

## File Locations & Functions

### 1. **AppletPlayerPhoto.cpp** - Photo Loading Entry Point
**File:** [nolimitgui/src/AppletPlayerPhoto.cpp](nolimitgui/src/AppletPlayerPhoto.cpp#L91-L110)

**Function:** `bool AppletPlayerPhoto::playMedia( AssetPlaySession& assetPlaySession, bool useExternalPlayer)`
```cpp
bool AppletPlayerPhoto::playMedia( AssetPlaySession& assetPlaySession, bool useExternalPlayer)
{
	std::string fullFileName = assetPlaySession.getFileNameAndPath();

	QPixmap pixmap;
	pixmap.load( fullFileName.c_str() );
	if( pixmap.isNull() )
	{
		close();
		return false;
	}

	if( ui.m_VidWidget->setImageFromFile( fullFileName.c_str() ) )
	{
		return true;
	}
	
	close();
	return false;
}
```
- Simply loads the photo file and calls `setImageFromFile()`
- **NO camera rotation applied here**

---

### 2. **AssetPhotoWidget.cpp** - Photo Asset Display
**File:** [nolimitgui/src/AssetPhotoWidget.cpp](nolimitgui/src/AssetPhotoWidget.cpp#L55-L75)

**Function:** `void AssetPhotoWidget::setAssetInfo( AssetBaseInfo& assetInfo )`
```cpp
void AssetPhotoWidget::setAssetInfo( AssetBaseInfo& assetInfo )
{
	AssetBaseWidget::setAssetInfo( assetInfo );
	this->setSizeHint( QSize( 100, 224 - 16 ) );
	ui.m_VidWidget->setImageFromFile( assetInfo.getAssetNameAndPath().c_str() );
	// ... rest of function
}
```
- Calls `VidWidget::setImageFromFile()` with the photo path
- **NO camera rotation is applied**

---

### 3. **VidWidget.cpp** - Video Display Widget

#### 3a. `setImageFromFile()` Function
**File:** [nolimitgui/src/VidWidget.cpp](nolimitgui/src/VidWidget.cpp#L508-L517)

```cpp
bool VidWidget::setImageFromFile( QString fileName )
{
	bool result = ui.m_VideoScreen->setImageFromFile( fileName );
	if( result )
	{
		m_StillImageFileName = fileName;
		m_StillImage.load( fileName );
	}

	return result;
}
```
- Delegates to `VxLabel::setImageFromFile()`
- **NO rotation applied**

#### 3b. Rotation Control Functions
**File:** [nolimitgui/src/VidWidget.cpp](nolimitgui/src/VidWidget.cpp#L347-L351)

```cpp
void VidWidget::setVidImageRotation( int imageRotation )
{
	ui.m_VideoScreen->setVidImageRotation( imageRotation );
}
```

#### 3c. Manual Photo Rotation Handler
**File:** [nolimitgui/src/VidWidget.cpp](nolimitgui/src/VidWidget.cpp#L420-L447)

```cpp
void VidWidget::slotFeedRotateButtonClicked( void )
{
	if( eVideoUiModePhoto == m_VideoUiMode )
	{
		int imageRotation = ui.m_VideoScreen->getVidImageRotation();
		imageRotation += 90;
		if( imageRotation >= 360 )
		{
			imageRotation = 0;
		}

		setVidImageRotation( imageRotation );
		QImage curImage = ui.m_VideoScreen->getLastVideoImage();
		if( curImage.isNull() )
		{
			curImage = m_StillImage;
		}
		if( curImage.isNull() && !m_StillImageFileName.isEmpty() )
		{
			curImage.load( m_StillImageFileName );
		}
		if( !curImage.isNull() )
		{
			ui.m_VideoScreen->playVideoFrame( curImage );
		}

		m_MyApp.toGuiUserMessage( "Photo Rotation %d", imageRotation );
		return;
	}
	// ... Video feed rotation code
}
```
- This is **manual user-initiated rotation** for photo viewing
- Sets `m_VidImageRotation` and calls `playVideoFrame()` to apply rotation

#### 3d. Camera Rotation Button Handler
**File:** [nolimitgui/src/VidWidget.cpp](nolimitgui/src/VidWidget.cpp#L489-L495)

```cpp
void VidWidget::slotCamRotateButtonClicked( void )
{
    std::string camId = m_AppSettings.getCamSourceId();
	int camRotation = m_AppSettings.getCamRotation( camId );
	camRotation += 90;
	if( camRotation >= 360 )
	{
		camRotation = 0;
	}

	m_AppSettings.setCamRotation( camId, camRotation );
	m_MyApp.setCamCaptureRotation( camRotation );
	m_MyApp.toGuiUserMessage( "My Cam Rotation %d", camRotation );
	emit signalCamRotationChanged( camRotation );
}
```
- Updates camera rotation setting in `AppSettings`
- Sets `m_CamCaptureRotation` in `AppCommon`
- **Does NOT apply to existing photos**

---

### 4. **VxLabel.cpp** - Actual Image Rendering

#### 4a. Load Image from File
**File:** [nolimitgui/src/VxLabel.cpp](nolimitgui/src/VxLabel.cpp#L181-L195)

```cpp
bool VxLabel::setImageFromFile( QString fileName )
{
	m_ImageFileName = fileName;
	return loadImageFromFile( m_ImageFileName );
}

bool VxLabel::loadImageFromFile( QString fileName )
{
	QImage	picBitmap;
	if( picBitmap.load( fileName ) && !picBitmap.isNull() )
	{
		playMotionVideoFrame( picBitmap, 0 );
		return true;
	}
    else
    {
        LogMsg( LOG_DEBUG, "ERROR VxLabel::loadImageFromFile failed %s", fileName.toUtf8().constData() );
		return false;
    }
}
```
- Loads the image and calls `playMotionVideoFrame()` with rotation 0

#### 4b. Motion Frame Playing (With Rotation)
**File:** [nolimitgui/src/VxLabel.cpp](nolimitgui/src/VxLabel.cpp#L205-LC213)

```cpp
void VxLabel::playMotionVideoFrame( QImage& vidFrame, int motion0To100000 )
{
	playRotatedVideoFrame( vidFrame, m_VidImageRotation );
}

void VxLabel::playVideoFrame( QImage& vidFrame )
{
	playRotatedVideoFrame( vidFrame, m_VidImageRotation );
}
```
- Uses `m_VidImageRotation` member variable (set via `setVidImageRotation()`)

#### 4c. Rotated Frame Display
**File:** [nolimitgui/src/VxLabel.cpp](nolimitgui/src/VxLabel.cpp#L215-L254)

```cpp
void VxLabel::playRotatedVideoFrame( QImage& vidFrame, int iRotate )
{
    if( !vidFrame.isNull() && isVisible() )
    {
		m_PreResizeImage = vidFrame;  // for camera snapshot

        QSize screenSize( this->width(), this->height() );
        QImage resizedPicmap = m_PreResizeImage.scaled( screenSize, Qt::KeepAspectRatio );
        if( iRotate )
        {
            #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                QTransform rm;
            #else
                QMatrix rm; 
            #endif
            rm.rotate( iRotate );
            QPixmap pixmap = QPixmap::fromImage( resizedPicmap ).transformed( rm );
            if( !pixmap.isNull() )
            {
                setPixmap( pixmap );
                update();
            }
            else
            {
                LogMsg( LOG_DEBUG, " VxLabel::slotPlayVideoFrame NULL pixmap" );
            }
        }
        else
        {
            QPixmap pixmap = QPixmap::fromImage( resizedPicmap );
            if( !pixmap.isNull() )
            {
                setPixmap( pixmap );
                update();
            }
            else
            {
                LogMsg( LOG_DEBUG, " VxLabel::slotPlayVideoFrame NULL pixmap" );
            }
        }
    }
    else if( vidFrame.isNull() )
    {
        LogMsg( LOG_ERROR, " VxLabel::slotPlayVideoFrame NULL picBitmap" );
    }
}
```
- Applies rotation using `QTransform::rotate()` or `QMatrix::rotate()`
- Only applies rotation if `iRotate != 0`
- **Rotation value comes from `m_VidImageRotation`, NOT from camera settings**

---

### 5. **InputPhotoWidget.cpp** - Camera Photo Capture

**File:** [nolimitgui/src/InputPhotoWidget.cpp](nolimitgui/src/InputPhotoWidget.cpp#L64-L103)

**Function:** `void InputPhotoWidget::slotSnapShotButtonClicked( void )`
```cpp
void InputPhotoWidget::slotSnapShotButtonClicked( void )
{
	m_PicImage = ui.m_VidWidget->getVideoScreen()->getLastVideoImage();
	if( m_PicImage.isNull() )
	{
		QMessageBox::warning(this, QObject::tr("Photo Snapshot Error"), QObject::tr("Photo Unavailable" ) );
	}
	else
	{
		// ... saves image to file
		if( false == m_PicImage.save( m_FileName.c_str() ) )
		{
			// ... error handling
		}
	}
}
```
- Takes snapshot from camera feed
- **The image is from the live camera feed (which should have rotation applied by backend)**
- Saves the raw image without applying rotation

**File:** [nolimitgui/src/InputPhotoWidget.cpp](nolimitgui/src/InputPhotoWidget.cpp#L109-L122)

**Function:** `void InputPhotoWidget::slotRotateCamButtonClicked( void )`
```cpp
void InputPhotoWidget::slotRotateCamButtonClicked( void )
{
    std::string camId = m_MyApp.getAppSettings().getCamSourceId();
	int camRotation = m_MyApp.getAppSettings().getCamRotation( camId );
	camRotation += 90;
	if( camRotation >= 360 )
	{
		camRotation = 0;
	}

	m_MyApp.getAppSettings().setCamRotation( camId, camRotation );
	m_MyApp.setCamCaptureRotation( camRotation );
}
```
- Updates camera rotation setting
- **Does NOT apply to already-captured photos**

---

### 6. **AppCommon.h** - Camera Rotation Member

**File:** [nolimitgui/src/AppCommon.h](nolimitgui/src/AppCommon.h#L206-L207)

```cpp
void						setCamCaptureRotation( uint32_t rot )  { m_CamCaptureRotation = rot; }
int							getCamCaptureRotation( void ) { return m_CamCaptureRotation; }
```

**File:** [nolimitgui/src/AppCommon.h](nolimitgui/src/AppCommon.h#L942)

```cpp
uint32_t					m_CamCaptureRotation;
```

**File:** [nolimitgui/src/AppCommon.cpp](nolimitgui/src/AppCommon.cpp#L234)

```cpp
, m_CamCaptureRotation( 0 )
```
- Initialized to 0 (no rotation)

---

### 7. **CamLogic.cpp** - Camera Source Selection

**File:** [nolimitgui/src/CamLogic.cpp](nolimitgui/src/CamLogic.cpp#L278)

```cpp
m_MyApp.setCamCaptureRotation( m_MyApp.getAppSettings().getCamRotation( camId ) );
```
- Sets `m_CamCaptureRotation` when camera source is selected
- Used for live camera feed rotation

---

### 8. **AppSettings.cpp** - Camera Rotation Settings

**File:** [nolimitgui/src/AppSettings.cpp](nolimitgui/src/AppSettings.cpp#L382-L397)

```cpp
void AppSettings::setCamRotation( std::string camId, uint32_t camRotation )
{
	// Implementation stores camera rotation by camera ID
}

uint32_t AppSettings::getCamRotation( std::string camId )
{
	// Implementation retrieves camera rotation by camera ID
}
```
- Stores per-camera rotation settings
- Retrieved when camera is changed or rotation button is clicked

---

## Data Flow Analysis

### Flow 1: Loading Photo via AppletPlayerPhoto
```
AppletPlayerPhoto::playMedia()
  └─> VidWidget::setImageFromFile()
       └─> VxLabel::setImageFromFile()
            └─> VxLabel::loadImageFromFile()
                 └─> QImage::load()
                 └─> VxLabel::playMotionVideoFrame(... rotation=0)
                      └─> VxLabel::playRotatedVideoFrame(... iRotate=m_VidImageRotation)
                           └─> QTransform::rotate(iRotate) [only if iRotate != 0]
```
**Result:** No camera rotation applied; uses `m_VidImageRotation` (which is 0 initially)

### Flow 2: Loading Photo via AssetPhotoWidget
```
AssetPhotoWidget::setAssetInfo()
  └─> VidWidget::setImageFromFile()
       └─> VxLabel::setImageFromFile()
            └─> VxLabel::loadImageFromFile()
                 └─> VxLabel::playMotionVideoFrame(... rotation=0)
                      └─> VxLabel::playRotatedVideoFrame(... iRotate=m_VidImageRotation)
```
**Result:** Same as Flow 1; no camera rotation applied

### Flow 3: User Rotates Photo (Manual)
```
VidWidget::slotFeedRotateButtonClicked()
  └─> VidWidget::setVidImageRotation(newRotation)
       └─> VxLabel::setVidImageRotation()
  └─> VidWidget::playVideoFrame()
       └─> VxLabel::playRotatedVideoFrame(... iRotate=m_VidImageRotation)
            └─> QTransform::rotate(iRotate)
```
**Result:** Photo is rotated manually by user action

### Flow 4: User Changes Camera Rotation
```
InputPhotoWidget::slotRotateCamButtonClicked() OR VidWidget::slotCamRotateButtonClicked()
  └─> AppSettings::setCamRotation(camId, newRotation)
  └─> AppCommon::setCamCaptureRotation(newRotation)
```
**Result:** Only affects FUTURE photos captured from camera; doesn't affect already-loaded photos

---

## Key Findings

### What Gets Applied Camera Rotation:
1. **Live Camera Preview** - Camera feed from capture device (backend handles rotation)
2. **Camera Snapshots** - When saved, contain the live feed (which may have rotation applied by camera hardware/driver)
3. **New Photos Taken** - Future captures will use the updated camera rotation setting

### What Does NOT Apply Camera Rotation:
1. **Photos loaded via AppletPlayerPhoto** - No automatic camera rotation
2. **Photos in AssetPhotoWidget** - No automatic camera rotation
3. **Already-saved photo files** - Camera rotation is only for NEW captures
4. **Historical photos** - No retroactive rotation

### Where Rotation IS Applied:
- **Manual rotation button in photo viewer** - User clicks rotate button to turn the displayed photo
- **VxLabel::playRotatedVideoFrame()** - Uses `m_VidImageRotation` which is set manually, not from camera settings
- **Camera preview during photo capture** - Before user clicks snapshot

---

## Important Classes and Members

| Class | Member | Purpose |
|-------|--------|---------|
| `AppCommon` | `m_CamCaptureRotation` | Currently set camera rotation for new captures |
| `VxLabel` | `m_VidImageRotation` | Current rotation applied to displayed image (manual) |
| `AppSettings` | Camera rotation per camId | Persistent settings for each camera |
| `VidWidget` | Rotation handling | Delegates to VxLabel |

---

## Conclusion

The `m_CamCaptureRotation` and `getCamRotation()` settings are **NOT** automatically applied when displaying loaded photos. They are:
1. Only applied to FUTURE camera captures
2. Used by the camera capture hardware/driver to rotate live preview
3. NOT used anywhere in the photo display pipeline (AppletPlayerPhoto → VidWidget → VxLabel)

If you want to apply camera rotation to loaded photos, you would need to either:
- Apply the rotation manually when loading the photo
- Modify `setImageFromFile()` to check for and apply camera rotation settings
- Store rotation metadata with the photo at capture time
