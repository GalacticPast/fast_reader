#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MB(num) ((int)num * 1024 * 1024ULL)
#define MAX(a, b) (((int)a >= (int)b) ? a : b)
#define DEBUG_BREAK __asm__ volatile("int $3")

typedef struct gui_elem
{
    // is the cursor on the element
    int       is_focused;
    Rectangle size;
} gui_elem;

typedef enum state
{
    STATE_MENU   = 0,
    STATE_READER = 1
} game_state;

typedef struct global_state
{
    bool       pause;
    game_state state;
    gui_elem   text_box;
    gui_elem   start_box;
    int        text_length;
    int        wpm;
    Font       font;

    char line[256];

    // index of the current string
    int ind;
    // if in reader mode display this string
    char       *string;
    const char *text;
} global_state;

void update(global_state *gm_state);
void draw(global_state *gm_state);
void display_line(const char *line, Font archivo, Vector2 pos, int font_size, int spacing);

int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Calculation Connect Five");

    global_state gm_state = {};

    gm_state.pause  = false;
    gm_state.text   = NULL;
    gm_state.state  = STATE_MENU;
    gm_state.ind    = 0;
    gm_state.string = NULL;
    gm_state.font   = LoadFont("../src/Archivo-Regular.ttf");
    // 400 words per minute
    gm_state.wpm    = 400;

    Rectangle text_box      = (Rectangle){50, 50, 700, 400};
    gm_state.text_box.size  = text_box;
    gm_state.start_box.size = (Rectangle){(float)SCREEN_WIDTH / 2 - 60, text_box.height + text_box.y + 20, 120, 50};

    float delta_time     = 0;
    float difference     = 0;
    float epsilon        = 0.001;
    float time_for_words = 0;
    bool  pause          = false;

    while (!WindowShouldClose())
    {
        update(&gm_state);

        draw(&gm_state);
        time_for_words = ((float)60000 / gm_state.wpm) / 1000.0f;
        if (gm_state.state == STATE_READER)
        {
            WaitTime(time_for_words);
        }
    }

    CloseWindow();
}

void update(global_state *gm_state)
{

    Vector2   mouse_pos   = GetMousePosition();
    gui_elem *text_box    = &gm_state->text_box;
    text_box->is_focused  = false;
    gui_elem *start_box   = &gm_state->start_box;
    start_box->is_focused = false;

    if (CheckCollisionPointRec(mouse_pos, text_box->size))
    {
        text_box->is_focused = true;
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_V))
        {
            const char *clipboard_text = GetClipboardText();
            if (clipboard_text == NULL)
            {
                return;
            }
            gm_state->text = clipboard_text;
            int   count    = 0;
            char *ptr      = (char *)gm_state->text;
            while (*ptr != '\0')
            {
                count++;
                ptr++;
            }
            gm_state->text_length = count;
            printf("%s\n", gm_state->text);
        }
    }
    if (CheckCollisionPointRec(mouse_pos, start_box->size))
    {
        start_box->is_focused = true;
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && gm_state->text != NULL)
        {
            gm_state->state = STATE_READER;
        }
    }

    if (gm_state->state == STATE_READER)
    {

        // speed
        int key = GetKeyPressed();

        switch (key)
        {
        case 'K': {
            gm_state->wpm += 50;
            if (gm_state->wpm > 2000)
            {
                gm_state->wpm = 2000;
            }
        }
        break;
        case 'J': {
            gm_state->wpm -= 50;
            if (gm_state->wpm < 50)
            {
                gm_state->wpm = 50;
            }
        }
        break;
        case 32: { // space
            gm_state->pause = gm_state->pause == true ? false : true;
        }
        break;
        }

        if (gm_state->pause == false)
        {

            char *ptr = (char *)gm_state->text;
            int   ind = gm_state->ind;

            if (ind >= gm_state->text_length)
            {
                printf("finished reading\n");
                gm_state->state  = STATE_MENU;
                gm_state->ind    = 0;
                gm_state->string = NULL;
                return;
            }

            ptr += ind;
            if (*ptr == '\0')
            {
                *ptr++ = ' ';
                ind++;
            }
            gm_state->string = ptr;

            while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0')
            {
                ptr++;
                ind++;
            }
            if ((ptr - gm_state->text) != ind)
            {
                printf("There is an error.\n");
                DEBUG_BREAK;
            }
            *ptr          = '\0';
            gm_state->ind = ind;
        }
    }
}

void draw(global_state *gm_state)
{
    gui_elem *text_box  = &gm_state->text_box;
    gui_elem *start_box = &gm_state->start_box;

    int offset_pos  = 5;
    int offset_size = 10;

    BeginDrawing();
    ClearBackground(BLACK);

    float   font_size;
    float   spacing;
    Vector2 pos;
    int     padding;

    if (gm_state->state == STATE_MENU)
    {
        // draw gui elems
        DrawRectangleRec(text_box->size, GRAY);
        if (text_box->is_focused)
        {
            DrawRectangleLines(text_box->size.x - offset_pos, text_box->size.y - offset_pos,
                               text_box->size.width + offset_size, text_box->size.height + offset_size, WHITE);
        }

        DrawRectangleRec(start_box->size, GREEN);
        if (start_box->is_focused)
        {
            DrawRectangleLines(start_box->size.x - offset_pos, start_box->size.y - offset_pos,
                               start_box->size.width + offset_size, start_box->size.height + offset_size, WHITE);
        }

        // text box
        const char *text = gm_state->text;
        char       *line = gm_state->line;

        font_size = 20;
        spacing   = 2;
        pos       = (Vector2){text_box->size.x + 10, text_box->size.y + 10};
        padding   = 10;

        if (text != NULL)
        {
            int     ind = 0;
            char   *ptr = (char *)text;
            Vector2 text_size;

            while (*ptr == ' ')
            {
                ptr++;
            }

            while (*ptr != '\0' && pos.y <= text_box->size.height)
            {
                line[ind] = *ptr;
                text_size = MeasureTextEx(gm_state->font, line, font_size, spacing);

                if (*ptr == '\n' || (text_size.x >= text_box->size.width - (padding * 2)))
                {
                    line[ind] = '\0';
                    display_line(line, gm_state->font, pos, font_size, spacing);

                    pos.y     += text_size.y;
                    pos.x      = text_box->size.x + padding;
                    text_size  = (Vector2){0, 0};
                    ind        = 0;
                    if (*ptr == '\n')
                        ptr++;
                    continue;
                }

                ptr++;
                ind++;
            }
            if (line[0] != '\0')
            {
                line[ind] = '\0';
                DrawTextEx(gm_state->font, line, pos, font_size, spacing, WHITE);
                memset(line, '\0', sizeof(char) * 256);
            }
        }
        else
        {
            DrawTextEx(gm_state->font, "Paste text here\0", pos, font_size, spacing, WHITE);
        }
        // draw the start text
        pos       = (Vector2){start_box->size.x + 10, start_box->size.y + 10};
        font_size = 30;
        DrawTextEx(gm_state->font, "START\0", pos, font_size, spacing, WHITE);
    }
    else if (gm_state->state == STATE_READER)
    {
        DrawLineEx((Vector2){0, 10},
                   (Vector2){
                       (SCREEN_WIDTH),
                       10,
                   },
                   5, GRAY);
        DrawLineEx((Vector2){SCREEN_WIDTH / 2, 0},
                   (Vector2){
                       (SCREEN_WIDTH / 2),
                       (SCREEN_HEIGHT / 2 - 50),
                   },
                   5, GRAY);
        DrawLineEx((Vector2){(SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2 + 50)},
                   (Vector2){
                       (SCREEN_WIDTH / 2),
                       (SCREEN_HEIGHT),
                   },
                   5, GRAY);
        DrawLineEx((Vector2){0, SCREEN_HEIGHT - 10},
                   (Vector2){
                       SCREEN_WIDTH,
                       SCREEN_HEIGHT - 10,
                   },
                   5, GRAY);

        char wpm[5] = {};
        sprintf(wpm, "%d\n", gm_state->wpm);
        DrawText(wpm, SCREEN_WIDTH - 30, SCREEN_HEIGHT - 20, 15, WHITE);

        font_size          = 80;
        spacing            = 10;
        float scale_factor = font_size / gm_state->font.baseSize;
        int   length       = strlen(gm_state->string);
        int   mid          = length / 2;

        Vector2 text_size = MeasureTextEx(gm_state->font, gm_state->string, font_size, spacing);
        pos   = (Vector2){(float)SCREEN_WIDTH / 2 - (text_size.x / 2), (float)SCREEN_HEIGHT / 2 - (text_size.y / 2)};
        pos.x = round(pos.x);
        pos.y = round(pos.y);

        float text_offset_x = 0.0f;
        float text_offset_y = 0.0f;

        float diff = 0;

        for (int i = 0; i < length;)
        {
            int codepointByteCount = 0;
            int codepoint          = GetCodepointNext(&gm_state->string[i], &codepointByteCount);
            int index              = GetGlyphIndex(gm_state->font, codepoint);

            if (i == mid)
            {
                // WARNING:  wtf figure this out
                diff = ((float)SCREEN_WIDTH / 2) - (pos.x + text_offset_x) -
                       (((gm_state->font.glyphs[index].advanceX * scale_factor) / 2));
                break;
            }

            text_offset_x += ((float)gm_state->font.glyphs[index].advanceX * scale_factor + spacing);

            i += codepointByteCount;
        }

        text_offset_x  = 0;
        text_offset_y  = 0;
        pos.x         += diff;

        for (int i = 0; i < length;)
        {
            int codepointByteCount = 0;
            int codepoint          = GetCodepointNext(&gm_state->string[i], &codepointByteCount);
            int index              = GetGlyphIndex(gm_state->font, codepoint);

            if (i == mid)
            {
                DrawTextCodepoint(gm_state->font, codepoint, (Vector2){pos.x + text_offset_x, pos.y + text_offset_y},
                                  font_size, RED);
            }
            else
            {
                DrawTextCodepoint(gm_state->font, codepoint, (Vector2){pos.x + text_offset_x, pos.y + text_offset_y},
                                  font_size, WHITE);
            }

            text_offset_x += ((float)gm_state->font.glyphs[index].advanceX * scale_factor + spacing);

            i += codepointByteCount;
        }
    }

    EndDrawing();
}

void display_line(const char *line, Font archivo, Vector2 pos, int font_size, int spacing)
{
    DrawTextEx(archivo, line, pos, font_size, spacing, WHITE);
    memset((void *)line, '\0', sizeof(char) * 256);
}
