#include <snes.h>

#define SCREEN_W 32
#define SCREEN_H 22
#define GROUND_Y 18
#define WORLD_W 64
#define MAX_ITEMS 5

static unsigned short pad, oldPad;
static int area = 0;
static int px = 3, py = 16, vy = 0, onGround = 1;
static int cameraX = 0, frame = 0, itemsFound = 0;
static int itemX[MAX_ITEMS] = {9, 18, 29, 42, 55};
static int itemY[MAX_ITEMS] = {16, 14, 15, 14, 15};
static int itemArea[MAX_ITEMS] = {0, 0, 0, 1, 1};
static int itemTaken[MAX_ITEMS] = {0, 0, 0, 0, 0};
static char message[33] = "THE STREETLIGHTS ARE HUMMING.";
static char grid[SCREEN_H][SCREEN_W + 1];

static void say(const char *s) {
    int i = 0;
    while (i < 32 && s[i]) { message[i] = s[i]; i++; }
    while (i < 32) { message[i++] = ' '; }
    message[32] = 0;
}

static void clearGrid(void) {
    int y, x;
    for (y = 0; y < SCREEN_H; y++) {
        for (x = 0; x < SCREEN_W; x++) grid[y][x] = ' ';
        grid[y][SCREEN_W] = 0;
    }
}

static void put(int x, int y, char c) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) grid[y][x] = c;
}

static void drawGame(void) {
    int x, y, wx, i, left;
    clearGrid();
    left = cameraX;
    for (x = 0; x < SCREEN_W; x++) {
        wx = left + x;
        if (area == 0) {
            if (wx % 12 == 4) { put(x, 11, '|'); put(x, 12, '|'); put(x, 13, '|'); }
            if (wx % 16 == 7) { put(x, 10, 'o'); put(x, 11, '|'); }
            if (wx % 20 == 2) { put(x, 14, 'H'); put(x, 15, 'H'); put(x, 16, 'H'); }
        } else {
            if (wx % 8 == 2) { put(x, 7, '^'); put(x, 8, '|'); put(x, 9, '|'); }
            if (wx % 11 == 5) put(x, 12, '*');
        }
        put(x, GROUND_Y, '#');
        put(x, GROUND_Y + 1, '#');
    }

    /* Platforms are defined in world coordinates. */
    if (area == 0) {
        for (x = 12; x < 16; x++) put(x-left, 14, '=');
        for (x = 23; x < 27; x++) put(x-left, 12, '=');
        for (x = 36; x < 40; x++) put(x-left, 14, '=');
        for (x = 47; x < 51; x++) put(x-left, 12, '=');
        put(54-left, 16, 'M'); put(55-left, 16, '_');
        put(61-left, 16, '>');
    } else {
        for (x = 8; x < 12; x++) put(x-left, 14, '=');
        for (x = 19; x < 24; x++) put(x-left, 12, '=');
        for (x = 31; x < 35; x++) put(x-left, 14, '=');
        for (x = 43; x < 48; x++) put(x-left, 11, '=');
        put(61-left, 16, '<');
    }

    for (i = 0; i < MAX_ITEMS; i++) {
        if (!itemTaken[i] && itemArea[i] == area) put(itemX[i]-left, itemY[i], '*');
    }

    put(px-left, py, '@');
    for (y = 0; y < SCREEN_W; y++) { /* keep compiler happy on small C toolchains */ }
    consoleDrawText(0, 0, "STRANGE THINGS AT STARLING HILL");
    consoleDrawText(0, 1, area == 0 ? "MAPLE STREET" : "INSIDE STARLING HILL");
    consoleDrawText(0, 2, "D-PAD MOVE  A JUMP  B LOOK");
    for (y = 4; y < SCREEN_H; y++) consoleDrawText(0, y, grid[y]);
    {
        char hud[33] = "ODDITIES: 0/5";
        hud[10] = (char)('0' + itemsFound);
        consoleDrawText(0, 20, hud);
    }
    consoleDrawText(0, 21, message);
}

static void interact(void) {
    if (area == 0 && px >= 52 && px <= 57) {
        say("MAIL: RETURN TO SENDER: THE MOON.");
    } else if (area == 0 && px >= 60) {
        area = 1; px = 2; py = 16; vy = 0; cameraX = 0;
        say("THE HILL IS BIGGER ON THE INSIDE.");
    } else if (area == 1 && px >= 60) {
        area = 0; px = 58; py = 16; vy = 0; cameraX = 32;
        say("HOME LOOKS ALMOST NORMAL.");
    } else if (area == 1 && px >= 39 && px <= 46) {
        say("THE WALLS ARE WHISPERING YOUR NAME.");
    } else {
        say("A GNOME SAYS: THE MOON IS SUSPICIOUS.");
    }
}

static void updateGame(void) {
    int oldY, i;
    int left, right;
    pad = padsCurrent(0);
    left = (pad & KEY_LEFT) != 0;
    right = (pad & KEY_RIGHT) != 0;

    if ((frame % 4) == 0) {
        if (left && px > 0) px--;
        if (right && px < WORLD_W-1) px++;
    }

    if ((pad & KEY_A) && !(oldPad & KEY_A) && onGround) {
        vy = -3; onGround = 0;
    }
    if ((pad & KEY_B) && !(oldPad & KEY_B)) interact();

    oldY = py;
    if (!onGround) {
        py += vy;
        vy++;
        if (vy > 3) vy = 3;
    }

    if (py >= GROUND_Y-1) { py = GROUND_Y-1; vy = 0; onGround = 1; }

    /* Land on platforms while falling. */
    if (vy >= 0 && py > oldY) {
        if (area == 0) {
            if (px >= 12 && px < 16 && oldY < 14 && py >= 14) { py=13; vy=0; onGround=1; }
            if (px >= 23 && px < 27 && oldY < 12 && py >= 12) { py=11; vy=0; onGround=1; }
            if (px >= 36 && px < 40 && oldY < 14 && py >= 14) { py=13; vy=0; onGround=1; }
            if (px >= 47 && px < 51 && oldY < 12 && py >= 12) { py=11; vy=0; onGround=1; }
        } else {
            if (px >= 8 && px < 12 && oldY < 14 && py >= 14) { py=13; vy=0; onGround=1; }
            if (px >= 19 && px < 24 && oldY < 12 && py >= 12) { py=11; vy=0; onGround=1; }
            if (px >= 31 && px < 35 && oldY < 14 && py >= 14) { py=13; vy=0; onGround=1; }
            if (px >= 43 && px < 48 && oldY < 11 && py >= 11) { py=10; vy=0; onGround=1; }
        }
    }

    for (i = 0; i < MAX_ITEMS; i++) {
        if (!itemTaken[i] && itemArea[i] == area &&
            px >= itemX[i]-1 && px <= itemX[i]+1 &&
            py >= itemY[i]-1 && py <= itemY[i]+1) {
            itemTaken[i] = 1;
            itemsFound++;
            say(itemsFound == 5 ? "ALL FIVE FOUND. THE HILL IS NOT A HILL." : "ODDITY FOUND. IT HUMS IN YOUR POCKET.");
        }
    }

    cameraX = px - 15;
    if (cameraX < 0) cameraX = 0;
    if (cameraX > WORLD_W - SCREEN_W) cameraX = WORLD_W - SCREEN_W;
    if (area == 1 && px < 2) cameraX = 0;
    oldPad = pad;
    frame++;
}

int main(void) {
    consoleInitDefaultText(0);
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    say("THE STREETLIGHTS ARE HUMMING.");
    setScreenOn();

    while (1) {
        updateGame();
        if ((frame % 3) == 0) drawGame();
        WaitForVBlank();
    }
    return 0;
}
