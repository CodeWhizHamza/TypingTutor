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
#define LESSON_TEXT 100
#define STANDARD_WORD_LENGTH 5
#define TOTAL_LESSONS 144
#define LESSON_NAME_LENGTH 20

// ------------------------- //
// Function Prototypes
// ------------------------- //
int get_wpm(int keystrokes, int number_of_seconds);
float get_accuracy(int correct, int total);
void get_lesson_names(char (*lessons)[LESSON_NAME_LENGTH]);
void sort_lessons(char (*lessons)[LESSON_NAME_LENGTH]);
int extract_lesson_number(char *lesson);

#include "headers/draw_keyboard.h"

// ------------------------- //
// Local Variables
// ------------------------- //
const int width = 864;
const int height = 576;

int main()
{

    char lessons[TOTAL_LESSONS][LESSON_NAME_LENGTH];
    get_lesson_names(lessons);
    sort_lessons(lessons);

    InitWindow(width, height, "Typing Tutor - by Hamza");
    SetTargetFPS(10);

    Font ibm_font = LoadFontEx("resources/IBM-font.ttf", 36, 0, 256);
    char *title = "Learn Touch Typing";
    Vector2 text_size = MeasureTextEx(ibm_font, title, 36, 1);
    Vector2 title_position;
    title_position.x = width / 2 - text_size.x / 2;
    title_position.y = 4;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawTextEx(ibm_font, title, title_position, 36, 1, BLACK);

        Rectangle box;
        box.height = 144;
        box.width = 816;
        box.x = 24;
        box.y = 48;
        DrawRectangleRounded(box, 0.03, 1000, WHITE);

        DrawKeyboard();

        EndDrawing();
    }

    CloseWindow();
    return 0;

    // int current_lesson = 50;
    // printf("Enter lesson number(1-144): ");
    // scanf("%d", &current_lesson);
    // string lesson_name = get_lesson_name(current_lesson);
    // lesson_name = "lessons/" + lesson_name;
    // FILE *lesson;
    // lesson = fopen(lesson_name.c_str(), "r");

    // char current_character;
    // int correct_keystrokes = 0;
    // int incorrect_keystrokes = 0;
    // int total_keystrokes = 0;
    // int start_time = time(NULL);
    // char lesson_text[LESSON_TEXT];
    // do
    // {
    //     fgets(lesson_text, LESSON_TEXT, lesson);
    //     printf("\n  %s\n", lesson_text);

    //     fflush(stdin);
    //     printf("> ");
    //     for (size_t i = 0; lesson_text[i] != '\0'; i++)
    //     {
    //         current_character = getchar();
    //         total_keystrokes += 1;

    //         if (current_character == lesson_text[i])
    //             correct_keystrokes += 1;
    //         else
    //             incorrect_keystrokes += 1;
    //     }
    // } while (!feof(lesson));
    // int end_time = time(NULL);

    // printf("\nCorrect: %d\nIncorrect: %d\nTotal keystrokes: %d\n", correct_keystrokes, incorrect_keystrokes, total_keystrokes);
    // printf("Accuracy: %.2f\n", get_accuracy(correct_keystrokes, total_keystrokes));
    // printf("WPM: %d\n", get_wpm(correct_keystrokes + incorrect_keystrokes, end_time - start_time));

    // fclose(lesson);
    // return 0;
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
