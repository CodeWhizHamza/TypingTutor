#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <dirent.h>
#include <raylib.h>

// ------------------------- //
// Macros
// ------------------------- //
#define LESSON_LINE_LENGTH 55
#define LESSON_LINES_COUNT 10
#define STANDARD_WORD_LENGTH 5
#define TOTAL_LESSONS 144
#define NAME_LENGTH 20
#define MARGIN_X 8
#define MAX_USERS 3

// ------------------------- //
// User defined types
// ------------------------- //
typedef struct
{
    int is_correct;
    int is_wrong;
} letter_state;
enum APP_STATUS
{
    MENU_LAYER = 0,
    TYPING_LAYER = 1,
};

#include "helper_functions.h"

// ------------------------- //
// Global Variables
// ------------------------- //
const int width = 864;
const int height = 576;
Texture2D keyboard;

int main()
{
    // Load lessons
    char lessons[TOTAL_LESSONS][NAME_LENGTH];
    get_files_from(lessons, "lessons");
    sort_lessons(lessons);

    // Load users data
    char users[MAX_USERS][NAME_LENGTH] = {0};
    int selected_user[MAX_USERS] = {1};
    get_files_from(users, "users");

    // Remove .txt from every user
    for (int i = 0; i < MAX_USERS; i++)
    {
        strcpy(users[i], strremove(users[i], ".txt"));
    }

    // Setup windows size and FPS
    InitWindow(width, height, "Typing Tutor - by Hamza");
    SetTargetFPS(60);

    // Load sounds
    InitAudioDevice();
    Music intro_music = LoadMusicStream("resources/welcome.wav");
    PlayMusicStream(intro_music);
    Sound error_effect = LoadSound("resources/error.mp3");
    Sound congrats = LoadSound("resources/congrats.mp3");

    int current_layer = MENU_LAYER;

    // Useful values
    Font ibm_font_title = LoadFontEx("resources/IBM-font.ttf", 36, 0, 256);
    Font ibm_font_text = LoadFontEx("resources/IBM-font.ttf", 32, 0, 256);
    float single_character_width = MeasureTextEx(ibm_font_text, "D", 34.5, 1).x;
    float single_character_height = MeasureTextEx(ibm_font_text, "O", 32, 1).y;

    // Setting up the title
    char title[] = "Learn Touch Typing";
    Vector2 title_position = {width / 2 - strlen(title) * single_character_width / 2, 4};

    // Menu box
    Rectangle menu_box;
    menu_box.width = 480;
    menu_box.height = 360;
    menu_box.x = width / 2 - menu_box.width / 2;
    menu_box.y = height / 2 - menu_box.height / 2;

    // User boxes
    int margin_x = 16;
    Rectangle user_box1;
    user_box1.width = menu_box.width - margin_x * 2;
    user_box1.height = 60;
    user_box1.x = menu_box.x + margin_x;
    user_box1.y = menu_box.y + single_character_height + 32;
    Rectangle user_box2;
    user_box2.width = menu_box.width - margin_x * 2;
    user_box2.height = 60;
    user_box2.x = user_box1.x;
    user_box2.y = user_box1.y + user_box2.height + 8;
    Rectangle user_box3;
    user_box3.width = menu_box.width - margin_x * 2;
    user_box3.height = 60;
    user_box3.x = user_box1.x;
    user_box3.y = user_box2.y + user_box3.height + 8;

    // Start Button
    Rectangle start_btn;
    start_btn.width = 108;
    start_btn.height = 48;
    start_btn.x = menu_box.x + margin_x;
    start_btn.y = menu_box.y + menu_box.height - start_btn.height - margin_x;

    // Text box
    Rectangle text_box;
    text_box.x = 16;
    text_box.y = 64;
    text_box.width = 832;
    text_box.height = 136;
    int text_box_mid = text_box.y + text_box.height / 2 - single_character_height / 2;

    // Next button
    Rectangle next_btn;
    next_btn.x = 16;
    next_btn.y = 216;
    next_btn.width = 196;
    next_btn.height = 48;

    // Stats
    Rectangle stats_box;
    stats_box.x = 536;
    stats_box.y = 216;
    stats_box.width = 312;
    stats_box.height = 316;

    keyboard = LoadTexture("resources/keyboard.png");

    int current_lesson = 0;
    char lesson_name[NAME_LENGTH * 2];
    char lesson_text[LESSON_LINES_COUNT][LESSON_LINE_LENGTH] = {0};
    reset_lesson_name(lesson_name);
    strcat(lesson_name, lessons[current_lesson]);
    load_lesson(lesson_name, lesson_text);

    int correct_keystrokes = 0;
    int incorrect_keystrokes = 0;
    int total_keystrokes = 0;

    char key;
    int current_letter_number = 0;
    int current_line_number = 0;
    int lesson_text_length = strlen(lesson_text[current_line_number]);

    letter_state letters_state[lesson_text_length];
    reset_validations(letters_state, lesson_text_length);

    Color letter_color;
    int is_lesson_completed = 0;
    int is_trainer_completed = 0;
    int start_time = 0, end_time = 0;
    int is_time_started = 0;
    int has_enough_accuracy = 0;

    char words_per_minute[50] = {0};
    char accuracy[50] = {0};
    char time_string[50] = {0};

    char current_user[NAME_LENGTH];

    while (!WindowShouldClose())
    {
        if (GetMusicTimePlayed(intro_music) < GetMusicTimeLength(intro_music) - 0.1 && current_layer != TYPING_LAYER)
            UpdateMusicStream(intro_music);
        else
            StopMusicStream(intro_music);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (current_layer == MENU_LAYER)
        {
            DrawRectangleRounded(menu_box, 0.05, 1000, WHITE);
            DrawTextEx(ibm_font_title, "WELCOME", (Vector2){text_box.x + text_box.width / 2 - single_character_width * 8 / 2, menu_box.y + 8}, 36, 1, DARKGRAY);

            if (CheckCollisionPointRec(GetMousePosition(), user_box1) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                selected_user[0] = 1;
                selected_user[1] = 0;
                selected_user[2] = 0;
            }
            if (CheckCollisionPointRec(GetMousePosition(), user_box2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                selected_user[0] = 0;
                selected_user[1] = 1;
                selected_user[2] = 0;
            }
            if (CheckCollisionPointRec(GetMousePosition(), user_box3) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                selected_user[0] = 0;
                selected_user[1] = 0;
                selected_user[2] = 1;
            }

            if (*users[0] != NULL)
            {
                DrawRectangleRounded(user_box1, 0.1, 1000, selected_user[0] ? PURPLE : RAYWHITE);
                DrawTextEx(ibm_font_text, users[0], (Vector2){user_box1.x + 8, user_box1.y + 12}, 32, 1, DARKGRAY);
            }
            if (*users[1] != NULL)
            {
                DrawRectangleRounded(user_box2, 0.1, 1000, selected_user[1] ? PURPLE : RAYWHITE);
                DrawTextEx(ibm_font_text, users[1], (Vector2){user_box2.x + 8, user_box2.y + 12}, 32, 1, DARKGRAY);
            }
            if (*users[2] != NULL)
            {
                DrawRectangleRounded(user_box3, 0.1, 1000, selected_user[2] ? PURPLE : RAYWHITE);
                DrawTextEx(ibm_font_text, users[2], (Vector2){user_box3.x + 8, user_box3.y + 12}, 32, 1, DARKGRAY);
            }

            // Draw start button
            DrawRectangleRounded(start_btn, 0.1, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Start", (Vector2){start_btn.x + 8, start_btn.y + 8}, 32, 1, WHITE);

            if ((CheckCollisionPointRec(GetMousePosition(), start_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
            {
                int selected_user_index = 0;
                for (int i = 0; i < MAX_USERS; i++)
                    if (selected_user[i])
                    {
                        selected_user_index = i;
                        break;
                    }
                current_layer = TYPING_LAYER;
                strcpy(current_user, strcat(users[selected_user_index], ".txt"));
                load_current_lesson(current_user, &current_lesson);
                reset_lesson_name(lesson_name);
                strcat(lesson_name, lessons[current_lesson]);
                load_lesson(lesson_name, lesson_text);
            }
        }

        if (current_layer == TYPING_LAYER)
        { // Print the title
            DrawTextEx(ibm_font_title, title, title_position, 36, 1, BLACK);

            DrawRectangleRounded(text_box, 0.05, 1000, WHITE);

            // printing training text
            for (int letter = 0; letter < strlen(lesson_text[current_line_number]); letter++)
            {
                if (letters_state[letter].is_correct)
                    letter_color = GREEN;
                else if (letters_state[letter].is_wrong)
                    letter_color = RED;
                else
                    letter_color = DARKGRAY;

                char temp_letter[2];
                temp_letter[0] = lesson_text[current_line_number][letter];
                temp_letter[1] = '\0';
                DrawTextEx(ibm_font_text, temp_letter, (Vector2){text_box.x + MARGIN_X + letter * single_character_width, text_box_mid}, 32, 1, letter_color);
            }

            // The cursor
            DrawRectangle(text_box.x + 8 + current_letter_number * single_character_width, text_box.y + text_box.height / 2 + single_character_height / 2, single_character_width, 4, DARKGREEN);

            // Waiting for user inputs
            if ((key = GetCharPressed()) != 0 && !is_lesson_completed && !is_trainer_completed)
            {
                // Start the time
                if (!is_time_started)
                {
                    start_time = time(NULL);
                    is_time_started = 1;
                }
                total_keystrokes += 1;

                // Check if correct key is pressed or not
                if (key == lesson_text[current_line_number][current_letter_number])
                {
                    letters_state[current_letter_number].is_correct = 1;
                    correct_keystrokes += 1;
                }
                else
                {
                    PlaySound(error_effect);
                    letters_state[current_letter_number].is_wrong = 1;
                    incorrect_keystrokes += 1;
                }

                current_letter_number += 1;

                // If lesson is completed
                if (lesson_text[current_line_number][current_letter_number] == '\0' && current_letter_number < LESSON_LINE_LENGTH - 1)
                {
                    PlaySound(congrats);
                    is_lesson_completed = 1;
                    is_time_started = 0;
                    end_time = time(NULL);
                }

                // If one line is completed
                if (current_letter_number >= LESSON_LINE_LENGTH - 1)
                {
                    current_letter_number = 0;
                    current_line_number += 1;

                    reset_validations(letters_state, strlen(lesson_text[current_line_number]));
                }
            }

            // When next button is clicked
            if ((CheckCollisionPointRec(GetMousePosition(), next_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (is_lesson_completed && !is_trainer_completed && IsKeyPressed(KEY_ENTER)))
            {
                correct_keystrokes = 0;
                total_keystrokes = 0;
                incorrect_keystrokes = 0;
                is_lesson_completed = 0;
                start_time = time(NULL);
                current_letter_number = 0;
                current_line_number = 0;

                // If accuracy is not enough, then don't let him go to next lesson
                if (has_enough_accuracy)
                    current_lesson += 1;
                // if (current_lesson >= TOTAL_LESSONS - 1)
                //     is_trainer_completed = 1;
                // if (is_trainer_completed)
                // {
                //     DrawRectangle(0, 0, width, height, PURPLE);
                //     DrawText("Congratulations! You completed your training.", width / 2 - single_character_width * 23, height / 2 - single_character_height / 2, 32, WHITE);

                //     current_lesson = 0;
                //     reset_lesson_name(lesson_name);
                //     strcat(lesson_name, lessons[current_lesson]);
                //     load_lesson(lesson_name, lesson_text);
                //     reset_validations(letters_state, strlen(lesson_text[current_line_number]));
                // }
                // else
                // {
                reset_lesson_name(lesson_name);
                strcat(lesson_name, lessons[current_lesson]);
                load_lesson(lesson_name, lesson_text);
                reset_validations(letters_state, strlen(lesson_text[current_line_number]));
                // }
            }

            if (!is_lesson_completed)
                DrawTexture(keyboard, 16, 240, RAYWHITE);

            if (is_lesson_completed)
            {

                has_enough_accuracy = get_accuracy(correct_keystrokes, total_keystrokes) >= 85;

                // Draw next button
                DrawRectangleRounded(next_btn, 0.15, 1000, DARKPURPLE);
                char *button_text = has_enough_accuracy ? "Next button" : "Retry";
                DrawTextEx(ibm_font_text, button_text, (Vector2){next_btn.x + 16, next_btn.y + next_btn.height / 2 - single_character_height / 2}, 32, 1, WHITE);

                // Draw status Box
                DrawRectangleRounded(stats_box, 0.05, 1000, WHITE);
                DrawTextEx(ibm_font_title, "Stats", (Vector2){stats_box.x + 16, stats_box.y + 16}, 36, 1, DARKGRAY);
                int line_position_y = stats_box.y + 16 + (single_character_height + 8) + 4;
                DrawLine(stats_box.x + 16, line_position_y, stats_box.x + stats_box.width - 24, line_position_y, LIGHTGRAY);

                // print words per minute
                sprintf(words_per_minute, "WPM: %d", get_wpm(correct_keystrokes + incorrect_keystrokes, end_time - start_time));
                DrawTextEx(ibm_font_text, words_per_minute, (Vector2){stats_box.x + 16, line_position_y + 8}, 32, 1, DARKGRAY);

                // print accuracy
                sprintf(accuracy, "Accuracy: %.2f", get_accuracy(correct_keystrokes, total_keystrokes));
                DrawTextEx(ibm_font_text, accuracy, (Vector2){stats_box.x + 16, line_position_y + 16 + single_character_height}, 32, 1, DARKGRAY);

                // print time
                sprintf(time_string, "Time: %ds", end_time - start_time);
                DrawTextEx(ibm_font_text, time_string, (Vector2){stats_box.x + 16, line_position_y + 32 + single_character_height * 2}, 32, 1, DARKGRAY);

                // Save user data to a file
                char lesson_number_string[10];

                sprintf(lesson_number_string, "%d", is_trainer_completed ? 0 : current_lesson + 1);

                save_user_data(current_user, lesson_number_string, accuracy, words_per_minute);
            }
        }
        EndDrawing();
    }
    UnloadMusicStream(intro_music);
    UnloadSound(error_effect);
    UnloadSound(congrats);
    CloseAudioDevice();
    UnloadTexture(keyboard);
    CloseWindow();
    return 0;
}
