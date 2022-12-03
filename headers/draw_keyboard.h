// void DrawButton(int x, int y, char *text, int width, int height)
// {
//     int font_size = 28;
//     Font ibm_font = LoadFontEx("resources/IBM-font.ttf", font_size, 0, 256);
//     Rectangle box2;
//     box2.height = height;
//     box2.width = width;
//     box2.x = x;
//     box2.y = y;
//     Vector2 button_text_position;
//     Vector2 text_dimensions = MeasureTextEx(ibm_font, text, font_size, 1);
//     button_text_position.x = box2.x + width / 2 - text_dimensions.x / 2;
//     button_text_position.y = box2.y + height / 2 - text_dimensions.y / 2;
//     DrawRectangleRounded(box2, 0.1, 1000, LIGHTGRAY);
//     DrawTextEx(ibm_font, text, button_text_position, font_size, 1, DARKGRAY);
// }

// void DrawKeyboard()
// {
//     Rectangle box;
//     box.height = 288;
//     box.width = 816;
//     box.x = 24;
//     box.y = 240;
//     DrawRectangleRounded(box, 0.03, 1000, WHITE);

//     char row1[15][5] = {
//         "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "«"};
//     // Row 1
//     for (size_t i = 0; i < 13; i++)
//     {
//         DrawButton(32 + 56 * i, 248, row1[i], 48, 48);
//     }
//     DrawButton(32 + 56 * 13, 248, row1[13], 72, 48);

//     // Row 2
//     char row2[13][5] = {
//         "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "\\"};
//     DrawButton(32, 304, "Tab", 72, 48);
//     for (size_t i = 0; i < 13; i++)
//     {
//         DrawButton(112 + 56 * i, 304, row2[i], 48, 48);
//     }

//     // Row 3
//     char row3[11][5] = {"A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'"};
//     DrawButton(32, 360, "Caps", 96, 48);
//     for (size_t i = 0; i < 11; i++)
//     {
//         DrawButton(136 + 56 * i, 360, row3[i], 48, 48);
//     }
//     DrawButton(752, 360, "Enter", 80, 48);

//     // Row 4
//     char row4[10][5] = {"Z", "X", "C", "V", "B", "N", "M", ",", ".", "/"};
//     DrawButton(32, 416, "Shift", 120, 48);
//     for (size_t i = 0; i < 10; i++)
//     {
//         DrawButton(160 + 56 * i, 416, row4[i], 48, 48);
//     }
//     DrawButton(720, 416, "Shift", 112, 48);

//     // Row 5
//     DrawButton(32, 472, "ctl", 48, 48);
//     DrawButton(32 + 56 * 2, 472, "fn", 48, 48);
//     DrawButton(32 + 56 * 3, 472, "■", 48, 48);
//     DrawButton(32 + 56 * 1, 472, "alt", 48, 48);
//     DrawButton(256, 472, "Space", 296, 48);
//     DrawButton(560, 472, "alt", 48, 48);
//     DrawButton(560 + 56 * 1, 472, "ctl", 48, 48);
//     DrawButton(560 + 56 * 2, 472, "<", 48, 48);
//     DrawButton(560 + 56 * 3, 472, "up", 48, 24);
//     DrawButton(560 + 56 * 3, 496, "dn", 48, 24);
//     DrawButton(560 + 56 * 4, 472, ">", 48, 48);
// }

// void DrawKeyboard()
// {
//     Texture2D keyboard = LoadImage("resources/keyboard.png");
//     ImageDraw
// }