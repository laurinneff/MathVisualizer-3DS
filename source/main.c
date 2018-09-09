#include <3ds.h>
#include <stdio.h>
#include <string.h>

//This include a header containing definitions of our image
#include "Scale_bgr.h"
#include "../expression_parser/code/expression_parser.h"

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on bottom screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_BOTTOM, NULL);

	printf("y=2x^3\n");
	printf("y=sin(2x^5+9)");

	printf("\x1b[30;10HPress Start to exit.");

	//We don't need double buffering in this example. In this way we can draw our image only once on screen.
	gfxSetDoubleBuffering(GFX_TOP, false);

	//Get the top screen's frame buffer
	u8* fbTop = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	//Get the bottom screen's frame buffer
	u8* fbBottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

	//Copy our image in the toü screen's frame buffer
	memcpy(fbTop, Scale_bgr, Scale_bgr_size);

	int selectedline = 1;
	#define maxline 4
	char *formulas[maxline];
	formulas[0] = "y=x";
	formulas[1] = "y=2x^3";
	formulas[2] = "y=sin(2x^5+9)";
	formulas[3] = "x=y^2";
	printf("\x1b[%d;1HReady!", maxline+1);
	char *formula = "y=x";


	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		if (kDown & KEY_UP   && selectedline > 1       ) selectedline--;
		else if (kDown & KEY_UP   && selectedline == 1      ) selectedline = maxline;
		if (kDown & KEY_DOWN && selectedline < maxline ) selectedline++;
		else if (kDown & KEY_DOWN && selectedline == maxline) selectedline = 1;

		for (int currentline = 1; currentline <= maxline; currentline++) {
			printf("\x1b[%d;1H                                        ", currentline);
			if(currentline == selectedline)
				printf("\x1b[%d;1H\x1b[47;30m%s\x1b[0m",currentline,formulas[currentline-1]);
			else
				printf("\x1b[%d;1H%s",currentline,formulas[currentline-1]);
		}

		if (kDown & KEY_A) {
			formula = formulas[selectedline-1];
			printf("\x1b[%d;1H                                        ", maxline+1);
			printf("\x1b[%d;1HLoaded %s!", maxline+1, formula);
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit services
	gfxExit();
	return 0;
}
