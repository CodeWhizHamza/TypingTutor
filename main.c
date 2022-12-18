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
#define LESSON_LINES_COUNT 100
#define STANDARD_WORD_LENGTH 5
#define TOTAL_LESSONS 144
#define NAME_LENGTH 20
#define MARGIN_X 8
#define MAX_USERS 3
#define TEXT_LENGTH 80

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
    LESSONS_LAYER = 1,
    PRACTICE_LAYER = 2
};
typedef struct
{
    int lesson_number;
    float accuracy;
    int time;
} UserHistory;

// included here so that it can access macros and user-defined variables
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

    // Changing Icon
    Image logo = LoadImage("resources/icon.png");
    ImageFormat(&logo, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(logo);

    // Load sounds
    InitAudioDevice();
    Music intro_music = LoadMusicStream("resources/welcome.wav");
    PlayMusicStream(intro_music);
    Sound error_effect = LoadSound("resources/error.mp3");
    Sound congrats = LoadSound("resources/congrats.mp3");

    int current_layer = MENU_LAYER;

    // Useful constant values
    Font ibm_font_title = LoadFontEx("resources/IBM-font.ttf", 36, 0, 256);
    Font ibm_font_text = LoadFontEx("resources/IBM-font.ttf", 32, 0, 256);
    float single_character_width = MeasureTextEx(ibm_font_text, "D", 34.5, 1).x;
    float single_character_height = MeasureTextEx(ibm_font_text, "O", 32, 1).y;

    // Setting up the title
    char title[] = "Learn Touch Typing";
    Vector2 title_position = {width / 2 - strlen(title) * single_character_width / 2, 4};

    // Menu box
    Rectangle menu_box;
    menu_box.width = 360;
    menu_box.height = 320;
    menu_box.x = 48;
    menu_box.y = height / 2 - menu_box.height / 2;

    // Current User info box
    Rectangle current_user_info_box;
    current_user_info_box.width = 360;
    current_user_info_box.height = 320;
    current_user_info_box.x = menu_box.x + 48 + /* margin */ menu_box.width;
    current_user_info_box.y = height / 2 - current_user_info_box.height / 2;

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

    Rectangle user_boxes[MAX_USERS] = {user_box1, user_box2, user_box3};

    // Reset user button
    Rectangle reset_user_button;
    reset_user_button.x = current_user_info_box.x + 16;
    reset_user_button.y = current_user_info_box.y + 16 + 7 * single_character_height + 8;
    reset_user_button.width = 108;
    reset_user_button.height = 48;

    // Start Button
    Rectangle start_lesson_button;
    start_lesson_button.width = single_character_width * strlen("Start Lesson") + 16;
    start_lesson_button.height = 48;
    start_lesson_button.x = menu_box.x + 160 + MARGIN_X * 2;
    start_lesson_button.y = menu_box.y + menu_box.height + 36;

    Rectangle start_practice_button;
    start_practice_button.width = single_character_width * strlen("Start practice") + 16;
    start_practice_button.height = 48;
    start_practice_button.x = start_lesson_button.x + start_lesson_button.width + MARGIN_X * 2;
    start_practice_button.y = menu_box.y + menu_box.height + 36;

    // Text box
    Rectangle text_box;
    text_box.x = 16;
    text_box.y = 64;
    text_box.width = 832;
    text_box.height = 136;
    int text_box_mid = text_box.y + text_box.height / 2 - single_character_height / 2;

    // Next button
    Rectangle next_lesson_button;
    next_lesson_button.x = 16;
    next_lesson_button.y = 216;
    next_lesson_button.width = 196;
    next_lesson_button.height = 48;

    // Stats
    Rectangle stats_box;
    stats_box.x = 536;
    stats_box.y = 216;
    stats_box.width = 312;
    stats_box.height = 316;

    // Loading keyboard image
    keyboard = LoadTexture("resources/keyboard.png");

    // Current lesson data
    int current_lesson = 0;
    char lesson_name[NAME_LENGTH * 2];
    char lesson_text[LESSON_LINES_COUNT][LESSON_LINE_LENGTH] = {0};
    reset_lesson_name(lesson_name);
    strcat(lesson_name, lessons[current_lesson]);
    load_lesson(lesson_name, lesson_text);

    // Current lesson variables
    int correct_keystrokes = 0;
    int incorrect_keystrokes = 0;
    int total_keystrokes = 0;

    // Current key data
    char key;
    int current_letter_number = 0;
    int current_line_number = 0;

    //
    int current_line_text_length = strlen(lesson_text[current_line_number]);

    // Keys validation store
    letter_state letters_state[current_line_text_length];
    reset_validations(letters_state, current_line_text_length);

    Color letter_color;
    int is_lesson_completed = 0;
    int is_trainer_completed = 0;
    int start_time = 0, end_time = 0;
    int is_time_started = 0;
    int has_enough_accuracy = 0;

    char words_per_minute[50] = {0};
    char accuracy[50] = {0};
    char time_string[50] = {0};

    // Current user previous history
    char current_user[NAME_LENGTH];
    char temp[NAME_LENGTH];
    strcpy(current_user, strcat(strcpy(temp, users[get_selected_index(selected_user)]), ".txt"));
    UserHistory history_holder;
    get_current_user_data(&history_holder, current_user);
    char history_line_container[TEXT_LENGTH];

    while (!WindowShouldClose())
    {
        // Play welcome sound
        if (GetMusicTimePlayed(intro_music) < GetMusicTimeLength(intro_music) - 0.1 && current_layer != LESSONS_LAYER)
            UpdateMusicStream(intro_music);
        else
            StopMusicStream(intro_music);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (current_layer == MENU_LAYER)
        {
            DrawTextEx(ibm_font_title, "WELCOME", (Vector2){text_box.x + text_box.width / 2 - single_character_width * 8 / 2, 48}, 36, 1, DARKGRAY);
            DrawRectangleRounded(menu_box, 0.05, 1000, WHITE);

            DrawRectangleRounded(current_user_info_box, 0.05, 1000, WHITE);

            DrawTextEx(ibm_font_title, "History", (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16}, 36, 1, DARKGRAY);

            sprintf(history_line_container, "Lesson: %d", history_holder.lesson_number + 1);
            DrawTextEx(ibm_font_text, history_line_container, (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16 + 2 * single_character_height + 8}, 32, 1, DARKGRAY);
            sprintf(history_line_container, "Accuracy: %.2f", history_holder.accuracy);
            DrawTextEx(ibm_font_text, history_line_container, (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16 + 3.5 * single_character_height + 8}, 32, 1, DARKGRAY);
            sprintf(history_line_container, "Time: %ds", history_holder.time);
            DrawTextEx(ibm_font_text, history_line_container, (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16 + 5 * single_character_height + 8}, 32, 1, DARKGRAY);

            // Draw reset button
            DrawRectangleRounded(reset_user_button, 0.1, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Reset", (Vector2){reset_user_button.x + 16, reset_user_button.y + 8}, 32, 1, WHITE);

            // Reset user progress
            if (CheckCollisionPointRec(GetMousePosition(), reset_user_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                strcpy(current_user, strcat(strcpy(temp, users[get_selected_index(selected_user)]), ".txt"));
                save_user_data(current_user, 0, 0.0, 0);
                get_current_user_data(&history_holder, current_user);
            }

            // Draw user boxes
            for (int i = 0; i < MAX_USERS; i++)
            {
                DrawRectangleRounded(user_boxes[i], 0.1, 1000, selected_user[i] ? PURPLE : RAYWHITE);
                DrawTextEx(ibm_font_text, users[i], (Vector2){user_boxes[i].x + 8, user_boxes[i].y + 12}, 32, 1, DARKGRAY);

                // Mark clicked user as active
                if (CheckCollisionPointRec(GetMousePosition(), user_boxes[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    for (int j = 0; j < MAX_USERS; j++)
                    {
                        selected_user[j] = i == j ? 1 : 0;
                    }
                    strcpy(current_user, strcat(strcpy(temp, users[get_selected_index(selected_user)]), ".txt"));
                    get_current_user_data(&history_holder, current_user);
                }
            }

            // Draw start lesson button
            DrawRectangleRounded(start_lesson_button, 0.15, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Start Lesson", (Vector2){start_lesson_button.x + 8, start_lesson_button.y + 8}, 32, 1, WHITE);

            // Draw start practice button
            DrawRectangleRounded(start_practice_button, 0.15, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Start Practice", (Vector2){start_practice_button.x + 8, start_practice_button.y + 8}, 32, 1, WHITE);

            if ((CheckCollisionPointRec(GetMousePosition(), start_lesson_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
            {
                int selected_user_index = get_selected_index(selected_user);
                current_layer = LESSONS_LAYER;
                strcpy(current_user, strcat(users[selected_user_index], ".txt"));
                load_current_lesson(current_user, &current_lesson);
                reset_lesson_name(lesson_name);
                strcat(lesson_name, lessons[current_lesson]);
                load_lesson(lesson_name, lesson_text);
            }
            if (CheckCollisionPointRec(GetMousePosition(), start_practice_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                current_layer = PRACTICE_LAYER;
                load_words(lesson_text);
            }
        }

        if (current_layer == LESSONS_LAYER || current_layer == PRACTICE_LAYER)
        {
            // Print the title
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
            if ((key = GetCharPressed()) != 0 && !is_lesson_completed)
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
            if ((CheckCollisionPointRec(GetMousePosition(), next_lesson_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (is_lesson_completed && IsKeyPressed(KEY_ENTER)))
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

                reset_lesson_name(lesson_name);
                strcat(lesson_name, lessons[current_lesson]);

                if (current_layer == LESSONS_LAYER)
                    load_lesson(lesson_name, lesson_text);
                else
                    load_words(lesson_text);

                reset_validations(letters_state, strlen(lesson_text[current_line_number]));
            }

            if (!is_lesson_completed)
                DrawTexture(keyboard, 16, 240, RAYWHITE);

            if (is_lesson_completed)
            {
                has_enough_accuracy = get_accuracy(correct_keystrokes, total_keystrokes) >= 85;

                // Draw next button
                if (current_layer == LESSONS_LAYER)
                {
                    DrawRectangleRounded(next_lesson_button, 0.15, 1000, DARKPURPLE);
                    char *button_text = has_enough_accuracy ? "Next lesson" : "Retry";
                    DrawTextEx(ibm_font_text, button_text, (Vector2){next_lesson_button.x + 16, next_lesson_button.y + next_lesson_button.height / 2 - single_character_height / 2}, 32, 1, WHITE);
                }
                else
                {
                    DrawRectangleRounded(next_lesson_button, 0.15, 1000, DARKPURPLE);
                    char *button_text = "Start Again";
                    DrawTextEx(ibm_font_text, button_text, (Vector2){next_lesson_button.x + 16, next_lesson_button.y + next_lesson_button.height / 2 - single_character_height / 2}, 32, 1, WHITE);
                }

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

                // save user data in file
                if (current_layer == LESSONS_LAYER)
                    save_user_data(current_user, is_trainer_completed ? 0 : current_lesson + 1, get_accuracy(correct_keystrokes, total_keystrokes), get_wpm(correct_keystrokes + incorrect_keystrokes, end_time - start_time));
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
