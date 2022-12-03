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
#define LESSON_NAME_LENGTH 20
#define MARGIN_X 8

// ------------------------- //
// User defined types
// ------------------------- //
typedef struct
{
    int is_correct;
    int is_wrong;
} letter_state;

// ------------------------- //
// Function Prototypes
// ------------------------- //
int get_wpm(int keystrokes, int number_of_seconds);
float get_accuracy(int correct, int total);
void get_lesson_names(char (*lessons)[LESSON_NAME_LENGTH]);
void sort_lessons(char (*lessons)[LESSON_NAME_LENGTH]);
int extract_lesson_number(char *lesson);
void reset_validations(letter_state *v, int length);

// ------------------------- //
// Local Variables
// ------------------------- //
const int width = 864;
const int height = 576;
Texture2D keyboard;

int main()
{

    char lessons[TOTAL_LESSONS][LESSON_NAME_LENGTH];
    get_lesson_names(lessons);
    sort_lessons(lessons);

    InitWindow(width, height, "Typing Tutor - by Hamza");
    SetTargetFPS(60);

    Font ibm_font_title = LoadFontEx("resources/IBM-font.ttf", 36, 0, 256);
    Font ibm_font_text = LoadFontEx("resources/IBM-font.ttf", 32, 0, 256);
    float single_character_width = MeasureTextEx(ibm_font_text, "D", 34.5, 1).x;
    float single_character_height = MeasureTextEx(ibm_font_text, "O", 32, 1).y;
    char *title = "Learn Touch Typing";
    Vector2 text_size = MeasureTextEx(ibm_font_title, title, 36, 1);
    Vector2 title_position = {width / 2 - text_size.x / 2, 4};

    Rectangle text_box;
    text_box.x = 16;
    text_box.y = 64;
    text_box.width = 832;
    text_box.height = 136;
    int text_box_mid = text_box.y + text_box.height / 2 - single_character_height / 2;

    keyboard = LoadTexture("resources/keyboard.png");

    // int current_lesson = 50;
    char lesson_name[40] = "lessons/";
    strcat(lesson_name, lessons[0]);
    FILE *lesson;
    lesson = fopen(lesson_name, "r");

    char lesson_text[LESSON_LINES_COUNT][LESSON_LINE_LENGTH] = {0};
    int i = 0;
    do
    {
        fgets(lesson_text[i], LESSON_LINE_LENGTH, lesson);
        i += 1;
    } while (!feof(lesson));
    fclose(lesson);
    // int end_time = time(NULL);

    // printf("\nCorrect: %d\nIncorrect: %d\nTotal keystrokes: %d\n", correct_keystrokes, incorrect_keystrokes, total_keystrokes);
    // printf("Accuracy: %.2f\n", get_accuracy(correct_keystrokes, total_keystrokes));
    // printf("WPM: %d\n", get_wpm(correct_keystrokes + incorrect_keystrokes, end_time - start_time));

    // char current_character;
    int correct_keystrokes = 0;
    int incorrect_keystrokes = 0;
    int total_keystrokes = 0;
    // int start_time = time(NULL);
    char key;
    int current_letter_number = 0;
    int current_line_number = 0;
    int lesson_text_length = strlen(lesson_text[current_line_number]);
    letter_state letters_state[lesson_text_length];
    reset_validations(letters_state, lesson_text_length);
    Color letter_color;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawTextEx(ibm_font_title, title, title_position, 36, 1, BLACK);

        DrawRectangleRounded(text_box, 0.05, 1000, WHITE);

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

        DrawRectangle(text_box.x + 8 + current_letter_number * single_character_width, text_box.y + text_box.height / 2 + single_character_height / 2, single_character_width, 4, DARKGREEN);

        if ((key = GetCharPressed()) != 0)
        {
            total_keystrokes += 1;
            if (key == lesson_text[current_line_number][current_letter_number])
            {
                letters_state[current_letter_number].is_correct = 1;
                correct_keystrokes += 1;
            }
            else
            {
                letters_state[current_letter_number].is_wrong = 1;
                incorrect_keystrokes += 1;
            }

            current_letter_number += 1;

            if (current_letter_number >= LESSON_LINE_LENGTH - 1)
            {
                current_letter_number = 0;
                current_line_number += 1;

                reset_validations(letters_state, strlen(lesson_text[current_line_number]));
            }
        }

        DrawTexture(keyboard, 16, 240, RAYWHITE);
        EndDrawing();
    }
    UnloadTexture(keyboard);
    CloseWindow();
    return 0;
}
int get_wpm(int keystrokes, int number_of_seconds)
{
    float words_count = (float)keystrokes / STANDARD_WORD_LENGTH;
    float minutes_count = (float)number_of_seconds / 60;
    return words_count / minutes_count;
}
float get_accuracy(int correct, int total)
{
    return (float)correct / total * 100;
}
void get_lesson_names(char (*lessons)[LESSON_NAME_LENGTH])
{
    DIR *lessons_folder;
    struct dirent *file;
    lessons_folder = opendir("lessons");
    int current_lesson = 0;
    if (lessons_folder)
    {
        while ((file = readdir(lessons_folder)) != NULL)
        {
            if (strcmp(".", file->d_name) == 0 || strcmp("..", file->d_name) == 0)
            {
                continue;
            }
            strcpy(lessons[current_lesson], file->d_name);
            current_lesson += 1;
        }
        closedir(lessons_folder);
    }
}
void sort_lessons(char (*lessons)[LESSON_NAME_LENGTH])
{
    size_t i, j;
    char temp[LESSON_NAME_LENGTH];
    for (i = 0; i < TOTAL_LESSONS - 1; i++)
    {
        for (j = 0; j < TOTAL_LESSONS - 1 - i; j++)
        {
            if (extract_lesson_number(lessons[j]) > extract_lesson_number(lessons[j + 1]))
            {
                strcpy(temp, lessons[j]);
                strcpy(lessons[j], lessons[j + 1]);
                strcpy(lessons[j + 1], temp);
            }
        }
    }
}
int extract_lesson_number(char *lesson)
{
    int index = 7;
    char digits[5] = {0};

    while (lesson[index] != '.')
    {
        digits[index - 7] = lesson[index];
        index += 1;
    }
    return atoi(digits);
}
void reset_validations(letter_state *v, int length)
{
    for (int i = 0; i < length; i++)
    {
        v[i].is_wrong = 0;
        v[i].is_correct = 0;
    }
}
