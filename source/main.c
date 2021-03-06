#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//This include a header containing definitions of our image
#include "Scale_bgr.h"
#include "tinyexpr.h"
#include "main.h"

int maxline = 0;

bool StartsWith(const char *a, const char *b)
{
	if (strncmp(a, b, strlen(b)) == 0)
		return 1;
	return 0;
}

char *stringReplace(char *, char *, char *);
void drawPixel(u16, u16, u8, u8, u8, u8 *);
void drawFormula(char *, u8 *, bool);

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on bottom screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_BOTTOM, NULL);

	printf("\x1b[28;1Hv%s (%s %s)", VERSION_NUM, __DATE__, __TIME__);
	printf("\x1b[30;10HPress Start to exit.");

	//We don't need double buffering in this example. In this way we can draw our image only once on screen.
	gfxSetDoubleBuffering(GFX_TOP, false);

	//Get the top screen's frame buffer
	u8 *fbTop = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	//Get the bottom screen's frame buffer
	u8 *fbBottom = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

	//Copy our image in the top screen's frame buffer
	memcpy(fbTop, Scale_bgr, Scale_bgr_size);

	int selectedline = 1;
	FILE *lastFormulas;
	char formulas[20][39];
	lastFormulas = fopen("lastFormulas.txt", "r");
	if (lastFormulas != NULL)
	{

		int i;
		for (i = 0; i < 20; i++)
		{
			formulas[i][0] = 0;
		}
		i = 0;
		while (i < 20 && fgets(&formulas[i], 38, lastFormulas))
		{
			i++;
		}

		maxline = i;
		for (i = 0; i < maxline; i++)
		{
			if (formulas[i][strlen(formulas[i]) - 1] == '\n')
				formulas[i][strlen(formulas[i]) - 1] = 0;
		}
		//printf("\x1b[%d;1H%d", maxline + 5, i);

		fclose(lastFormulas);
	}
	bool fast = true;

	printf("\x1b[%d;1H[Y]Disable fast mode (more pixels, slow)", maxline + 3);
	printf("\x1b[%d;1H[X]Open Keyboard", maxline + 4);
	printf("\x1b[%d;1H[B]Clear List", maxline + 5);
	printf("\x1b[%d;1HReady!", maxline + 1);

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_UP && selectedline > 1)
			selectedline--;
		else if (kDown & KEY_UP && selectedline == 1)
			selectedline = maxline;
		if (kDown & KEY_DOWN && selectedline < maxline)
			selectedline++;
		else if (kDown & KEY_DOWN && selectedline == maxline)
			selectedline = 1;

		for (int currentline = 1; currentline <= maxline; currentline++)
		{
			int maxline = 0;
			printf("\x1b[%d;1H                                        ", currentline);
			if (currentline == selectedline)
				printf("\x1b[%d;1H\x1b[47;30m%s\x1b[0m", currentline, formulas[currentline - 1]);
			else
				printf("\x1b[%d;1H%s", currentline, formulas[currentline - 1]);
		}

		if (kDown & KEY_X)
		{
			char *res = keyboard();
			if (*res && (StartsWith(res, "x=") || StartsWith(res, "y=")))
			{
				if (maxline < 20)
				{
					maxline++;
				}
				for (int i = 18; i >= 0; i--)
				{
					strcpy(formulas[i + 1], formulas[i]);
				}
				strcpy(formulas[0], res);

				if (res == "§")
					break;
			}
			else
				printf("\x1b[%d;1HInvalid formula, make sure it starts with 'x=' or 'y='", maxline + 1);

			if (fast)
				printf("\x1b[%d;1H[Y]Disable fast mode (more pixels, slow)", maxline + 3);
			else
				printf("\x1b[%d;1H[Y]Enable fast mode (less pixels, fast)", maxline + 3);
			printf("\x1b[%d;1H[X]Open Keyboard", maxline + 4);
			printf("\x1b[%d;1H[B]Clear List", maxline + 5);
			for (int currentline = 1; currentline <= maxline; currentline++)
			{
				printf("\x1b[%d;1H                                        ", currentline);
				if (currentline == selectedline)
					printf("\x1b[%d;1H\x1b[47;30m%s\x1b[0m", currentline, formulas[currentline - 1]);
				else
					printf("\x1b[%d;1H%s", currentline, formulas[currentline - 1]);
			}

			drawFormula(res, fbTop, fast);
		}

		if (kDown & KEY_Y)
		{
			fast = !fast;
			printf("\x1b[%d;1H                                        ", maxline + 3);
			if (fast)
				printf("\x1b[%d;1H[Y]Disable fast mode (more pixels, slow)", maxline + 3);
			else
				printf("\x1b[%d;1H[Y]Enable fast mode (less pixels, fast)", maxline + 3);
		}

		if (kDown & KEY_A)
		{
			drawFormula(formulas[selectedline - 1], fbTop, fast);
		}

		if (kDown & KEY_B)
		{
			maxline = 0;
			memcpy(fbTop, Scale_bgr, Scale_bgr_size);
			for (int i = 0; i < 29; i++)
				printf("\x1b[%d;1H                                        ", i + 1);
			if (fast)
				printf("\x1b[%d;1H[Y]Disable fast mode (more pixels, slow)", maxline + 3);
			else
				printf("\x1b[%d;1H[Y]Enable fast mode (less pixels, fast)", maxline + 3);
			printf("\x1b[%d;1H[X]Open Keyboard", maxline + 4);
			printf("\x1b[%d;1H[B]Clear List", maxline + 5);
			for (int currentline = 1; currentline <= maxline; currentline++)
			{
				printf("\x1b[%d;1H                                        ", currentline);
				if (currentline == selectedline)
					printf("\x1b[%d;1H\x1b[47;30m%s\x1b[0m", currentline, formulas[currentline - 1]);
				else
					printf("\x1b[%d;1H%s", currentline, formulas[currentline - 1]);
			}
			printf("\x1b[%d;1HReady!", maxline + 1);
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}
	lastFormulas = fopen("lastFormulas.txt", "w");
	for (int i = 0; i < maxline; i++)
	{
		fprintf(lastFormulas, "%s", formulas[i]);
		if (i < maxline - 1)
			fprintf(lastFormulas, "\n");
	}
	fclose(lastFormulas);

	// Exit services
	gfxExit();
	return 0;
}

char *stringReplace(char *search, char *replace, char *string)
{
	char *tempString, *searchStart;
	int len = 0;

	// preuefe ob Such-String vorhanden ist
	searchStart = strstr(string, search);
	if (searchStart == NULL)
	{
		return string;
	}

	// Speicher reservieren
	tempString = (char *)malloc(strlen(string) * sizeof(char));
	if (tempString == NULL)
	{
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
	strcat(string, (char *)tempString + len);

	// Speicher freigeben
	free(tempString);

	return string;
}

void drawPixel(u16 x, u16 y, u8 red, u8 green, u8 blue, u8 *fb)
{
	fb[3 * (240 - y + (x - 1) * 240) + 0] = blue;
	fb[3 * (240 - y + (x - 1) * 240) + 1] = green;
	fb[3 * (240 - y + (x - 1) * 240) + 2] = red;
}

void drawFormula(char *formula, u8 *fbTop, bool fast)
{
	memcpy(fbTop, Scale_bgr, Scale_bgr_size);

	u8 *tempFb = malloc(Scale_bgr_size);
	memcpy(tempFb, Scale_bgr, Scale_bgr_size);

	printf("\x1b[%d;1H                                        ", maxline + 1);
	printf("\x1b[%d;1H                                        ", maxline + 2);
	printf("\x1b[%d;1HLoaded %s!", maxline + 1, formula);

	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();

	float increment;
	if (fast)
		increment = .02;
	else
		increment = .0001;

	formula += 2;

	double x, y;

	te_variable vars[] = {{"x", &x}, {"y", &y}};

	int err;
	te_expr *expr = te_compile(formula, vars, 2, &err);

	// Calculate
	if (StartsWith(formula - 2, "x="))
	{
		for (y = -12; y <= 12.1; y += increment)
		{
			x = te_eval(expr);

			int sx, sy;
			sx = (x + 20) * 10 + 1.5;
			sy = 240 - ((y + 12) * 10 - 1.5);

			if (sx < 1)
				sx = 1;
			if (sx > 400)
				sx = 400;
			if (sy < 1)
				sy = 1;
			if (sy > 240)
				sy = 240;
			drawPixel(sx, sy, 0xFF, 0xFF, 0xFF, tempFb);
		}
	}
	else if (StartsWith(formula - 2, "y="))
	{
		for (x = -20; x <= 20.1; x += increment)
		{
			y = te_eval(expr);

			int sx, sy;
			sx = (x + 20) * 10 + 1.5;
			sy = 240 - ((y + 12) * 10 - 1.5);

			if (sx < 1)
				sx = 1;
			if (sx > 400)
				sx = 400;
			if (sy < 1)
				sy = 1;
			if (sy > 240)
				sy = 240;

			drawPixel(sx, sy, 0xFF, 0xFF, 0xFF, tempFb);
		}
	}
	else
		printf("\x1b[%d;1HInvalid formula, make sure it starts with 'x=' or 'y='", maxline + 1);

	memcpy(fbTop, tempFb, Scale_bgr_size);
	free(tempFb);
}