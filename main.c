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
#define TOTAL_WORDS 1000
#define MAX_WORD_LENGTH 20

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
    PRACTICE_LAYER = 2,
    GAME_MENU = 3,
    TYPING_STACK = 4,
    TYPING_FLAPPY = 5,
    TYPING_POPUP = 6
};
typedef struct
{
    int lesson_number;
    float accuracy;
    int time;
} UserHistory;
typedef struct
{
    bool is_done;
} flappy_word_state;

// included here so that it can access macros and user-defined variables
#include "helper_functions.h"

// ------------------------- //
// Functions Prototypes
// ------------------------- //
void print_games_menu(Font, Font, Texture2D, Texture2D, Rectangle, Texture2D, Rectangle, int *);

// ------------------------- //
// Global Variables
// ------------------------- //
const int width = 864;
const int height = 576;
Texture2D keyboard;

int main()
{
    srand(time(NULL));

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
    Font font_big = LoadFontEx("resources/IBM-font.ttf", 48, 0, 256);
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
    Rectangle btn_reset_user_data;
    btn_reset_user_data.x = current_user_info_box.x + 16;
    btn_reset_user_data.y = current_user_info_box.y + 16 + 7 * single_character_height + 8;
    btn_reset_user_data.width = 108;
    btn_reset_user_data.height = 48;

    // Start practice button
    Rectangle btn_start_lesson;
    btn_start_lesson.width = single_character_width * strlen("Start Lesson") + 16;
    btn_start_lesson.height = 48;
    btn_start_lesson.x = menu_box.x + 40 + MARGIN_X * 2;
    btn_start_lesson.y = menu_box.y + menu_box.height + 36;

    // Start practice button
    Rectangle btn_start_practice;
    btn_start_practice.width = single_character_width * strlen("Start practice") + 16;
    btn_start_practice.height = 48;
    btn_start_practice.x = btn_start_lesson.x + btn_start_lesson.width + MARGIN_X * 2;
    btn_start_practice.y = menu_box.y + menu_box.height + 36;

    // Play games button
    Rectangle btn_play_games;
    btn_play_games.width = single_character_width * strlen("Play Games") + 16;
    btn_play_games.height = 48;
    btn_play_games.x = btn_start_practice.x + btn_start_practice.width + MARGIN_X * 2;
    btn_play_games.y = menu_box.y + menu_box.height + 36;

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

    // * GAMES DATA
    Texture2D duck_sprite = LoadTexture("resources/duck-sprites.png");
    Texture2D stack_png = LoadTexture("resources/stack.png");
    Texture2D clouds_sprite = LoadTexture("resources/clouds.png");
    Rectangle duck_frame = {0.0f, 0.0f, (float)duck_sprite.width / 3, (float)duck_sprite.height};
    Rectangle clouds_frame = {0.0f, 0.0f, (float)clouds_sprite.width / 3, (float)clouds_sprite.height};

    // Flappy states
    int duck_current_frame = 0;
    float duck_y_position = 80;
    float game_speed = 0.2f;
    int frame_speed = 10;
    int frame_flag = 0;
    bool game_running = false;
    bool game_over = false;
    Texture2D background = LoadTexture("resources/background.png");
    Rectangle background_frame = {0.0f, 0.0f, width, height};
    char flappy_words[TOTAL_WORDS][MAX_WORD_LENGTH] = {0};
    load_flappy_words(flappy_words);
    char current_word[MAX_WORD_LENGTH];
    int current_word_index;
    int current_letter_index;
    bool current_word_states[MAX_WORD_LENGTH];
    int flappy_score;
    size_t i;
    Sound game_over_sound = LoadSound("resources/game-over.mp3");
    Music music = LoadMusicStream("resources/game-music.mp3");
    bool sound_not_played;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (current_layer == MENU_LAYER)
        {
            // Play welcome sound
            if (GetMusicTimePlayed(intro_music) < GetMusicTimeLength(intro_music) - 0.1 && current_layer != LESSONS_LAYER)
                UpdateMusicStream(intro_music);
            else
                StopMusicStream(intro_music);

            // App menu layer
            DrawTextEx(ibm_font_title, "WELCOME", (Vector2){text_box.x + text_box.width / 2 - single_character_width * 8 / 2, 48}, 36, 1, DARKGRAY);
            DrawRectangleRounded(menu_box, 0.05, 1000, WHITE);

            /**
             * Draw The history box in right of our menu box.
             */
            DrawRectangleRounded(current_user_info_box, 0.05, 1000, WHITE);
            DrawTextEx(ibm_font_title, "History", (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16}, 36, 1, DARKGRAY);
            sprintf(history_line_container, "Lesson: %d", history_holder.lesson_number + 1);
            DrawTextEx(ibm_font_text, history_line_container, (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16 + 2 * single_character_height + 8}, 32, 1, DARKGRAY);
            sprintf(history_line_container, "Accuracy: %.2f", history_holder.accuracy);
            DrawTextEx(ibm_font_text, history_line_container, (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16 + 3.5 * single_character_height + 8}, 32, 1, DARKGRAY);
            sprintf(history_line_container, "Time: %ds", history_holder.time);
            DrawTextEx(ibm_font_text, history_line_container, (Vector2){current_user_info_box.x + 16, current_user_info_box.y + 16 + 5 * single_character_height + 8}, 32, 1, DARKGRAY);

            // Draw reset button
            DrawRectangleRounded(btn_reset_user_data, 0.1, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Reset", (Vector2){btn_reset_user_data.x + 16, btn_reset_user_data.y + 8}, 32, 1, WHITE);

            // Reset user progress
            if (CheckCollisionPointRec(GetMousePosition(), btn_reset_user_data) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                strcpy(current_user, strcat(strcpy(temp, users[get_selected_index(selected_user)]), ".txt"));
                save_user_data(current_user, 0, 0.0, 0);
                get_current_user_data(&history_holder, current_user);
            }

            /**
             * Draw user selection boxes.
             */
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
            DrawRectangleRounded(btn_start_lesson, 0.15, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Start Lesson", (Vector2){btn_start_lesson.x + 8, btn_start_lesson.y + 8}, 32, 1, WHITE);

            // Draw start practice button
            DrawRectangleRounded(btn_start_practice, 0.15, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Start Practice", (Vector2){btn_start_practice.x + 8, btn_start_practice.y + 8}, 32, 1, WHITE);

            DrawRectangleRounded(btn_play_games, 0.15, 1000, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Play Games", (Vector2){btn_play_games.x + 8, btn_play_games.y + 8}, 32, 1, WHITE);

            // When start lesson button is pressed
            if ((CheckCollisionPointRec(GetMousePosition(), btn_start_lesson) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
            {
                int selected_user_index = get_selected_index(selected_user);
                current_layer = LESSONS_LAYER;
                strcpy(current_user, strcat(users[selected_user_index], ".txt"));
                load_current_lesson(current_user, &current_lesson);
                reset_lesson_name(lesson_name);
                strcat(lesson_name, lessons[current_lesson]);
                load_lesson(lesson_name, lesson_text);
            }

            // When Start practice button is pressed
            if (CheckCollisionPointRec(GetMousePosition(), btn_start_practice) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                current_layer = PRACTICE_LAYER;
                load_words(lesson_text);
                StopMusicStream(intro_music);
            }

            // When play games button is clicked
            if (CheckCollisionPointRec(GetMousePosition(), btn_play_games) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                current_layer = GAME_MENU;
                StopMusicStream(intro_music);
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
                {
                    reset_lesson_text(lesson_text);
                    load_words(lesson_text);
                }
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

        if (current_layer == GAME_MENU)
        {
            print_games_menu(font_big, ibm_font_text, stack_png, duck_sprite, duck_frame, clouds_sprite, clouds_frame, &current_layer);
        }

        // if (current_layer == TYPING_STACK)
        // {
        //     DrawTextEx(ibm_font_title, "HEHE STACK", (Vector2){10, 10}, ibm_font_title.baseSize, 1, DARKGRAY);
        // }

        if (current_layer == TYPING_FLAPPY)
        {
            if (!game_running && !game_over)
            {
                flappy_score = 0;
                current_word_index = 0;
                current_letter_index = 0;
                strcpy(current_word, flappy_words[current_word_index]);
                game_running = true;
                duck_y_position = 80;
                game_speed = 0.2f;
                sound_not_played = true;
                for (i = 0; i < strlen(current_word); i++)
                {
                    current_word_states[i] = false;
                }
            }

            // Draw scrolling background
            DrawTextureRec(background, background_frame, (Vector2){0, 0}, WHITE);
            if (!game_over && game_running)
            {
                background_frame.x += 0.5f;
                if (background_frame.x > background.width - background_frame.width - 10)
                {
                    background_frame.x = 0.0f;
                }
            }

            // Draw Score
            char score_string[40];
            sprintf(score_string, "Score: %d", flappy_score);
            DrawTextEx(ibm_font_text, score_string, (Vector2){16, 58}, 32, 1, BLACK);

            // Draw back button
            DrawRectangle(8, 8, 80, 40, DARKPURPLE);
            DrawTextEx(ibm_font_text, "Back", (Vector2){16, 12}, 32, 1, WHITE);
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){8, 8, 80, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                current_layer = GAME_MENU;
                game_running = false;
            }

            // Draw bird with changing frames
            DrawTextureRec(duck_sprite, duck_frame, (Vector2){40, duck_y_position}, WHITE);
            if (game_running)
            {
                frame_flag += 1;
                if (frame_flag >= 60 / frame_speed)
                {
                    frame_flag = 0;
                    duck_current_frame += 1;
                    if (duck_current_frame > 3)
                        duck_current_frame = 0;

                    duck_frame.x = duck_current_frame * duck_frame.width;
                }

                duck_y_position += game_speed;
            }

            if (game_running)
            {
                int x = width / 2 - strlen(current_word) * (single_character_width)-16;
                for (i = 0; i < strlen(current_word); i++)
                {
                    char letter = current_word[i];
                    char *ptr = malloc(2 * sizeof(char));
                    ptr[0] = letter;
                    ptr[1] = '\0';

                    DrawRectangle(x + i * single_character_width + 16 + i * 16, 40, single_character_width + 8, single_character_height + 8, (Color){250, 250, 250, current_word_states[i] ? 150 : 255});
                    DrawTextEx(ibm_font_text, ptr, (Vector2){x + i * single_character_width + 16 + i * 16 + 4, 40 + 4}, 32, 1, BLACK);
                }

                if ((key = GetCharPressed()) != 0)
                {
                    if (key == current_word[current_letter_index])
                    {
                        if (duck_y_position > 80 + 20)
                        {
                            duck_y_position -= 20;
                        }

                        current_word_states[current_letter_index] = true;
                        current_letter_index += 1;

                        flappy_score += 1;

                        if (current_word[current_letter_index] == '\0')
                        {
                            current_letter_index = 0;
                            current_word_index += 1;
                            strcpy(current_word, flappy_words[current_word_index]);
                            for (i = 0; i < strlen(current_word); i++)
                            {
                                current_word_states[i] = false;
                                game_speed += 0.005;
                            }
                        }
                    }
                    else
                    {
                        if (flappy_score > 0)
                            flappy_score -= 1;
                    }
                }
            }

            // Check if game is over or not
            if (duck_y_position > height - duck_frame.height - 100)
            {
                game_over = true;
                game_running = false;
            }

            // Draw game over display
            if (game_over)
            {
                DrawRectangleRounded((Rectangle){width / 2 - 150, height / 2 - 100, 300, 200}, 0.1f, 1000, WHITE);
                DrawTextEx(ibm_font_title, "GAME OVER", (Vector2){width / 2 - 150 + 70, height / 2 - 100 + 16}, 36, 1, BLACK);

                DrawRectangleRounded((Rectangle){width / 2 - 70, height / 2 - 20, 140, 40}, 0.1f, 1000, DARKPURPLE);
                DrawTextEx(ibm_font_text, "Restart", (Vector2){width / 2 - 70 + 16, height / 2 - 20 + 4}, 32, 1, WHITE);

                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){width / 2 - 70, height / 2 - 20, 140, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    game_over = false;
                    game_running = false;
                }
            }

            // Play game music
            if (game_running)
            {
                PlayMusicStream(music);
                UpdateMusicStream(music);
            }

            // Play game over music
            if (game_over && sound_not_played)
            {
                sound_not_played = false;
                PlaySound(game_over_sound);
            }
        }

        // if (current_layer == TYPING_POPUP)
        // {
        //     DrawTextEx(ibm_font_title, "HEHE popup", (Vector2){10, 10}, ibm_font_title.baseSize, 1, DARKGRAY);
        // }

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

void print_games_menu(Font title_font, Font text_font, Texture2D stack_png, Texture2D duck_sprite, Rectangle duck_frame, Texture2D clouds_sprite, Rectangle clouds_frame, int *current_layer)
{
    ClearBackground(WHITE);
    DrawTextEx(title_font, "Games", (Vector2){width / 2 - 40, 20}, title_font.baseSize, 1, DARKGRAY);

    DrawRectangle(8, 8, 80, 40, DARKPURPLE);
    DrawTextEx(text_font, "Back", (Vector2){16, 12}, 32, 1, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){8, 8, 80, 40}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        *current_layer = MENU_LAYER;
    }

    // First game tile
    Rectangle box;
    // box.height = 200;
    // box.width = 200;
    // box.x = 56;
    // box.y = (float)height / 2 - (float)box.height / 2 + 30;
    // DrawRectangleRounded(box, 0.05, 1000, (Color){240, 240, 240, 255});
    // DrawTextEx(text_font, "Typing stack", (Vector2){box.x + 8, box.y + 10}, text_font.baseSize, 1, DARKGRAY);
    // DrawTexture(stack_png, box.x + 40, box.y + 50, WHITE);

    // if (CheckCollisionPointRec(GetMousePosition(), box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    // {
    //     *current_layer = TYPING_STACK;
    // }

    // Second game tile
    box.width = 240;
    box.height = 240;
    box.x = 200 + 56 * 2;
    box.y = (float)height / 2 - (float)box.height / 2 + 30;
    DrawRectangleRounded(box, 0.05, 1000, (Color){240, 240, 240, 255});
    DrawTextEx(text_font, "Typing bird", (Vector2){box.x + 36, box.y + 10}, text_font.baseSize, 1, DARKGRAY);
    DrawTextureRec(duck_sprite, duck_frame, (Vector2){box.x + 50, box.y + 80}, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        *current_layer = TYPING_FLAPPY;
    }

    // Popup game tile
    // box.height = 200;
    // box.width = 200;
    // box.x = 200 + 240 + 56 * 3;
    // box.y = (float)height / 2 - (float)box.height / 2 + 30;
    // DrawRectangleRounded(box, 0.05, 1000, (Color){240, 240, 240, 255});
    // DrawTextEx(text_font, "Typing Popup", (Vector2){box.x + 8, box.y + 10}, text_font.baseSize, 1, DARKGRAY);
    // DrawTextureRec(clouds_sprite, clouds_frame, (Vector2){box.x + clouds_frame.width / 4 - 8, box.y + 60}, WHITE);
    // DrawTextEx(text_font, "type", (Vector2){box.x + 74, box.y + 120}, text_font.baseSize, 1, DARKGRAY);

    // if (CheckCollisionPointRec(GetMousePosition(), box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    // {
    //     *current_layer = TYPING_POPUP;
    // }
}
