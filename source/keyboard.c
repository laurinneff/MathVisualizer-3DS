#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "consts.h"

char *keyboard()
{
    bool entering = true;
    char *text;
    int selx = 0;
    int sely = 0;
#define KBWIDTH 10
#define KBHEIGHT 5
    char characters[KBHEIGHT][KBWIDTH] = {
        {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
        {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'},
        {'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'},
        {'u', 'v', 'w', 'x', 'y', 'z', '=', '+', '-', '*'},
        {'/', '.', '^', '(', ')'}};

    text = malloc(1);
    text[0] = 0;

    for (int i = 0; i < 29; i++)
        printf("\x1b[%d;1H                                        ", i + 1);
    printf("\x1b[28;1Hv%s (%s %s)", VERSION_NUM, __DATE__, __TIME__);
    printf("\x1b[29;5HPress Select to close keyboard.");

    while (entering)
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
        {
            text = "§";
            break;
        }

        if (kDown & KEY_SELECT)
        {
            if (!text)
                text = "";
            entering = false;
        }

        if (kDown & KEY_A)
        {
            size_t len = strlen(text);
            if (len < 38)
            {
                char *str2 = malloc(len + 1 + 1); /* one for extra char, one for trailing zero */
                strcpy(str2, text);
                str2[len] = characters[sely][selx];
                str2[len + 1] = '\0';

                free(text);
                text = str2;
            }
            //text += characters[sely][selx];
        }

        if (kDown & KEY_B)
        {
            text[strlen(text) - 1] = 0;
        }

        if (kDown & KEY_UP)
            sely--;
        if (kDown & KEY_DOWN)
            sely++;
        if (kDown & KEY_LEFT)
            selx--;
        if (kDown & KEY_RIGHT)
            selx++;
        if (selx < 0)
            selx = 0;
        if (selx >= KBWIDTH)
            selx = KBWIDTH - 1;
        if (sely < 0)
            sely = 0;
        if (sely >= KBHEIGHT)
            sely = KBHEIGHT - 1;
        if ((sely == 4) && (selx > 4))
            selx = 4;

        printf("\x1b[%d;1H                                        ", 2);
        printf("\x1b[2;2H%s", text);
        //23*13
        for (int x = 0; x < KBWIDTH; x++)
        {
            for (int y = 0; y < KBHEIGHT; y++)
            {
                if ((x == selx) && (y == sely))
                    printf("\x1b[47;30m\x1b[%d;%dH%c\x1b[0m", ((y + 1) * 2) + 3, (x + 1) * 2, characters[y][x]); // inverted
                else
                    printf("\x1b[%d;%dH%c", ((y + 1) * 2) + 3, (x + 1) * 2, characters[y][x]); // normal
            }
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();

        //Wait for VBlank
        gspWaitForVBlank();
    }
    for (int i = 0; i < 29; i++)
        printf("\x1b[%d;1H                                        ", i + 1);
    printf("\x1b[28;1Hv%s (%s %s)", VERSION_NUM, __DATE__, __TIME__);
    return text;
}