#include <3ds.h>
#include <stdio.h>
#include <string.h>

//This include a header containing definitions of our image
#include "Scale_bgr.h"

#include "tinyexpr.h"

bool StartsWith(const char *a, const char *b) {
	if(strncmp(a, b, strlen(b)) == 0) return 1;
	return 0;
}

char * stringReplace(char*, char*, char*);

void drawPixel(u16, u16, u8, u8, u8, u8*);

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

	//Copy our image in the top screen's frame buffer
	memcpy(fbTop, Scale_bgr, Scale_bgr_size);

	int selectedline = 1;
	#define maxline 7
	char *formulas[maxline];
	formulas[0] = "y=x";
	formulas[1] = "y=2*x^3";
	formulas[2] = "y=sin(x)";
	formulas[3] = "y=cos(x)";
	formulas[4] = "x=y^2";
	formulas[5] = "y=sin(x)+cos(x)";
	formulas[6] = "y=2*sin(x/2)";
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
			printf("\x1b[%d;1H                                        ", maxline+3);
			printf("\x1b[%d;1HLoaded %s!", maxline+1, formula);

			// Calculate
			if(StartsWith(formula, "x=")) {
				memcpy(fbTop, Scale_bgr, Scale_bgr_size);
				formula += 2;
				char newformula[128];
				char replace[128];
				for(float y = -12; y <= 12.1; y+=.02) {
					strcpy(&newformula, formula);
					sprintf(&replace, "%.2f", y);
					stringReplace("y", replace, newformula);
					stringReplace("y", replace, newformula);
					double x;
					x = te_interp(newformula, 0);
					
					int sx, sy;
					sx = (x + 20) * 10 + 1.5;
					sy = 240-((y + 12) * 10 - 1.5);
					
					if(sx < 1) sx = 1;
					if(sx > 400) sx = 400;
					if(sy < 1) sy = 1;
					if(sy > 240) sy = 240;
					//if((0 < sx && sx <= 400) && (0 < sy && sx <= 240))
					drawPixel(sx, sy, 0xFF, 0xFF, 0xFF, fbTop);

					
				}
			} else if(StartsWith(formula, "y=")) {
				memcpy(fbTop, Scale_bgr, Scale_bgr_size);
				formula += 2;
				char newformula[128];
				char replace[128];
				for(float x = -20; x <= 20.1; x+=.02) {
					strcpy(&newformula, formula);
					sprintf(&replace, "%.2f", x);
					stringReplace("x", replace, newformula);
					stringReplace("x", replace, newformula);
					double y;
					y = te_interp(newformula, 0);
					
					int sx, sy;
					sx = (x + 20) * 10 + 1.5;
					sy = 240-((y + 12) * 10 - 1.5);
					
					if(sx < 1) sx = 1;
					if(sx > 400) sx = 400;
					if(sy < 1) sy = 1;
					if(sy > 240) sy = 240;
					
					//if((0 < sx && sx <= 400) && (0 < sy && sy <= 240))
					drawPixel(sx, sy, 0xFF, 0xFF, 0xFF, fbTop);
					
					printf("\x1b[%d;1H %s!", maxline+2, newformula);
				}
			} else
				printf("\x1b[%d;1HInvalid formula, make sure it starts with 'x=' or 'y='", maxline+1);
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

char * stringReplace(char *search, char *replace, char *string) {
	char *tempString, *searchStart;
	int len=0;


	// preuefe ob Such-String vorhanden ist
	searchStart = strstr(string, search);
	if(searchStart == NULL) {
		return string;
	}

	// Speicher reservieren
	tempString = (char*) malloc(strlen(string) * sizeof(char));
	if(tempString == NULL) {
		return NULL;
	}

	// temporaere Kopie anlegen
	strcpy(tempString, string);

	// ersten Abschnitt in String setzen
	len = searchStart - string;
	string[len] = '\0';

	// zweiten Abschnitt anhaengen
	strcat(string, replace);

	// dritten Abschnitt anhaengen
	len += strlen(search);
	strcat(string, (char*)tempString+len);

	// Speicher freigeben
	free(tempString);

	return string;
}

void drawPixel(u16 x, u16 y, u8 red, u8 green, u8 blue, u8* fb) {
	fb[3 * (240 - y + (x - 1) * 240) + 0] = blue;
	fb[3 * (240 - y + (x - 1) * 240) + 1] = green;
	fb[3 * (240 - y + (x - 1) * 240) + 2] = red;
}
