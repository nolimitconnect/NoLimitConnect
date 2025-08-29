#include "VxJpgLib.h"

/*
#include <stdio.h>
extern "C" { 
#include "jpeglib.h" 
}
#include <setjmp.h>
#include "VxJpgLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "memory.h"

struct jpg_datastore{
  unsigned char *data;
  size_t len;

  jpg_datastore () { data = 0; len = 0; }
  ~jpg_datastore () { free (data); }
};
*/

#include <turbojpeg.h>

#include "memory.h"

tjhandle GetJpegCompressorInstance( void )
{
    static tjhandle jpegCompressor = nullptr;
    if(!jpegCompressor)
    {
        jpegCompressor = tjInitCompress();
    }

    return jpegCompressor;
}

int32_t VxBmp2Jpg(	int				iBitsPerPixel,	//number of bits each pixel..(For now must be 24)
                      unsigned char *   pu8Bits,		//bits of bmp to convert
                      int				width,			//width of image in pixels
                      int				height,		//height of image in pixels
                      int				iQuality,		//quality of image
                      int				iJpgBufLen,		//maximum length of pu8RetJpg
                      unsigned char *   pu8RetJpg,		//buffer to return Jpeg image
                      long *			ps32RetJpgLen ) //return length of jpeg image
{
	
	* ps32RetJpgLen = 0;


    const int JPEG_QUALITY = 75;

    long unsigned int _jpegSize = iJpgBufLen;
    unsigned char* _compressedImage = NULL; //!< Memory is allocated by tjCompress2 if _jpegSize == 0

    tjhandle _jpegCompressor = GetJpegCompressorInstance();

    tjCompress2(_jpegCompressor, pu8Bits, width, 0, height, TJPF_RGB,
                &pu8RetJpg, &_jpegSize, TJSAMP_420, JPEG_QUALITY,
                TJFLAG_FASTDCT);

    if(_jpegSize)
    {
        *ps32RetJpgLen = _jpegSize;
//        memcpy( pu8RetJpg, _compressedImage, _jpegSize );
    }


    //tjDestroy(_jpegCompressor);

    //to free the memory allocated by TurboJPEG (either by tjAlloc(),
    //or by the Compress/Decompress) after you are done working on it:
    //tjFree(_compressedImage);


	/* And we're done! */
	return 0;
}
