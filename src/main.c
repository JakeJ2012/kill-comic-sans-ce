#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <stdlib.h>
#include <time.h>
#include <fileioc.h>

#include "gfx/gfx.h"

bool tick();
void initPalette();
void refreshKeyData(uint8_t keys[8], uint8_t lastKeys[8]);
bool isPressed(uint8_t keys[8], kb_lkey_t key);
bool isNewPress(uint8_t keys[8], uint8_t lastKeys[8], kb_lkey_t key);
void setVarToHighscore(int highscore);
void setHighscoreToVar(int * highscore);

typedef enum gameScreen
{
    SCREEN_TITLE, 
    SCREEN_GAME
} gameScreen;

int main(void)
{

    srand(time(NULL));

    gfx_Begin();

    gfx_SetDrawBuffer();

    initPalette();

    while (tick());

    gfx_End();
}

void initPalette() 
{
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    gfx_SetTextBGColor(0);
    gfx_SetTextTransparentColor(0);
    gfx_SetTextFGColor(2);
    gfx_SetTextScale(1, 1);
}

bool tick()
{
    static uint8_t keys[8];
    static uint8_t lastKeys[8];
    static gameScreen screen = SCREEN_TITLE;
    static int targetX = 0;
    static int targetY = 0;
    static int crossX = GFX_LCD_WIDTH/2;
    static int crossY = GFX_LCD_HEIGHT/2;
    static int frame = 0;
    static int score = 0;
    static int bulletHoleX = -1;
    static int bulletHoleY = -1;
    static bool hasLoadedHighscore = false;
    static int highScore = 0;

    refreshKeyData(keys, lastKeys);

    int retCode = 1;

    if (screen == SCREEN_TITLE)
    {
        if (isNewPress(keys, lastKeys, kb_KeyClear)) {
            retCode = 0;
        }

        if (isNewPress(keys, lastKeys, kb_KeyEnter) || isNewPress(keys, lastKeys, kb_Key2nd)) {
            bulletHoleX = -1;
            bulletHoleY = -1;
            screen = SCREEN_GAME;
            score = 0;
        }

        if (!hasLoadedHighscore)
        {
            setHighscoreToVar(&highScore);
            hasLoadedHighscore = true;
        }

        char buf[20];

        snprintf(buf, sizeof(buf), "Highscore: %d", highScore);

        gfx_FillScreen(1);

        gfx_SetTextScale(1, 1);
        int w = gfx_GetStringWidth("[Enter] or [2nd] to play");
        gfx_PrintStringXY("[Enter] or [2nd] to play", GFX_LCD_WIDTH/2-w/2, 120);

        gfx_PrintStringXY(buf, 0, GFX_LCD_HEIGHT-8);

        gfx_SetTextScale(2, 2);
        w = gfx_GetStringWidth("Kill Comic Sans");
        gfx_PrintStringXY("Kill Comic Sans", GFX_LCD_WIDTH/2-w/2, 20);
        gfx_SwapDraw();
    }
    else if (screen == SCREEN_GAME)
    {
        if (isNewPress(keys, lastKeys, kb_KeyClear)) {
            screen = SCREEN_TITLE;
            if (score > highScore)
            {
                highScore = score;
                setVarToHighscore(highScore);
            }
        }

        if ((isNewPress(keys, lastKeys, kb_KeyEnter) || isNewPress(keys, lastKeys, kb_Key2nd)) && (crossX >= targetX) && (crossX <= targetX + comic_sans_width) && (crossY >= targetY) && (crossY <= targetY + comic_sans_width)) 
        {
            frame = -1;
            score++;
            bulletHoleX = crossX;
            bulletHoleY = crossY;
        }

        frame = (frame + 1) % 60;

        if (frame == 0) 
        {
            targetX = rand() % (GFX_LCD_WIDTH - comic_sans_width);
            targetY = rand() % (GFX_LCD_HEIGHT - comic_sans_height);
        }

        crossX += 5 * ((int)isPressed(keys, kb_KeyRight) - (int)isPressed(keys, kb_KeyLeft));
        crossY += 5 * ((int)isPressed(keys, kb_KeyDown) - (int)isPressed(keys, kb_KeyUp));

        if (crossX < 0)
        {
            crossX = 0;
        } else if (crossX > (GFX_LCD_WIDTH - cross_width)) 
        {
            crossX = GFX_LCD_WIDTH - cross_width;
        }

        if (crossY < 0)
        {
            crossY = 0;
        } else if (crossY > (GFX_LCD_HEIGHT - cross_height)) 
        {
            crossY = GFX_LCD_HEIGHT - cross_height;
        }




        gfx_FillScreen(1);

        gfx_Sprite(comic_sans, targetX, targetY);
        gfx_TransparentSprite(cross, crossX, crossY);
        gfx_SetTextScale(1, 1);
        gfx_SetTextXY(0, GFX_LCD_HEIGHT-8);
        gfx_PrintInt(score, 1);
        if (bulletHoleX > 0 && bulletHoleY > 0) 
        {
            gfx_SetColor(2);
            gfx_FillCircle(bulletHoleX, bulletHoleY, 5);
        }
        gfx_SwapDraw();
    }

    return retCode;

}

void refreshKeyData(uint8_t keys[8], uint8_t lastKeys[8])
{
    kb_Scan();

    for (int i = 0; i < 8; i++)
    {
        lastKeys[i] = keys[i];
        keys[i] = kb_Data[i];
    }
}

bool isPressed(uint8_t keys[8], kb_lkey_t key)
{
    return (bool)(keys[(key) >> 8] & (key));
}

bool isNewPress(uint8_t keys[8], uint8_t lastKeys[8], kb_lkey_t key)
{
    return (bool)(keys[(key) >> 8] & (key)) && !(bool)(lastKeys[(key) >> 8] & (key));
}

void setVarToHighscore(int highscore) 
{
    uint8_t handle = ti_Open("CSANSHI", "w");
    if (!handle)
    {
        return;
    }

    ti_Write(&highscore, sizeof(int), 1, handle);
    ti_Close(handle);
}

void setHighscoreToVar(int * highscore)
{
    uint8_t handle = ti_Open("CSANSHI", "r");
    if (!handle)
    {
        *highscore = 0;
        return;
    }

    size_t count = ti_Read(highscore, sizeof(int), 1, handle);
    ti_Close(handle);

    if (!count)
    {
        *highscore = 0;
    }
}