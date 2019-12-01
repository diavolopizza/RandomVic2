#include "Flag.h"


Flag::Flag()
{

	TGA *tga;
	TGAData data;
	data.cmap = nullptr;
	tga = TGAOpen("RandomVic2\\resources\\gfx\\flags\\template.tga", "r+");
	if (!tga || tga->last != TGA_OK) {
		/* error handling goes here */
	}
	/* the TGA_IMAGE_ID tells the TGAReadImage() to read the image
id, the TGA_IMAGE_DATA tells it to read the whole image data
and any color map data if existing. NOTE: the image header is
read always no matter what options were specified.
At last we pass over the TGA_RGB flag so the returned data is
in RGB format and not BGR */
	data.flags = TGA_IMAGE_DATA | TGA_IMAGE_ID | TGA_RGB;
	if (TGAReadImage(tga, &data) != TGA_OK) {
		/* error handling goes here */
		cout << "Reading failed" << endl;
	}
	//tga->fd = fopen("RandomVic2\\resources\\gfx\\flags\\template2.tga", "w");
	TGAWriteImage(tga, &data);

	//TGAClose(tga);

	/* NOTE: you must free the TGAData structure manually because
	   you probably want to use the data it contains after a call to
	   TGAClose()
		*/

}


Flag::~Flag()
{
}
