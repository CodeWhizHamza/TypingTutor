
#pragma once
int get_wpm(int keystrokes, int number_of_seconds);
float get_accuracy(int correct, int total);
void get_files_from(char (*list)[NAME_LENGTH], char *folder_name);
void sort_lessons(char (*lessons)[NAME_LENGTH]);
int extract_lesson_number(char *lesson);
void reset_validations(letter_state *v, int length);
void load_lesson(char *lesson_name, char lesson_text[][LESSON_LINE_LENGTH]);
void reset_lesson_name(char *lesson_name);
char *strremove(char *str, const char *sub);
void save_user_data(char *current_user, int lesson_number, float accuracy, int wpm);
void load_current_lesson(char *current_user, int *current_lesson);
void get_current_user_data(UserHistory *holder, char *user_name);
int get_selected_index(int arr[]);

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
void get_files_from(char (*list)[NAME_LENGTH], char *folder_name)
{
    DIR *folder;
    struct dirent *file;
    folder = opendir(folder_name);
    int current_file = 0;
    if (folder)
    {
        while ((file = readdir(folder)) != NULL)
        {
            if (strcmp(".", file->d_name) == 0 || strcmp("..", file->d_name) == 0)
            {
                continue;
            }
            strcpy(list[current_file], file->d_name);
            current_file += 1;
        }
        closedir(folder);
    }
}
void sort_lessons(char (*lessons)[NAME_LENGTH])
{
    size_t i, j;
    char temp[NAME_LENGTH];
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
void load_lesson(char *lesson_name, char lesson_text[][LESSON_LINE_LENGTH])
{
    FILE *lesson;
    lesson = fopen(lesson_name, "r");

    int i = 0;
    do
    {
        fgets(lesson_text[i], LESSON_LINE_LENGTH, lesson);
        i += 1;
    } while (!feof(lesson));

    fclose(lesson);
}
void reset_lesson_name(char *lesson_name)
{
    char *prefix = "lessons/";
    for (int i = 0; i < NAME_LENGTH * 2; i++)
    {
        if (i < strlen(prefix))
            lesson_name[i] = prefix[i];
        else
            lesson_name[i] = 0;
    }
}
char *strremove(char *str, const char *sub)
{
    size_t len = strlen(sub);
    if (len > 0)
    {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL)
        {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}
void save_user_data(char *current_user, int lesson_number, float accuracy, int wpm)
{
    char folder_name[] = "users/";
    char filename[40];
    strcpy(filename, strcat(folder_name, current_user));
    FILE *file;
    file = fopen(filename, "w");
    fprintf(file, "%d %f %d", lesson_number, accuracy, wpm);
    fclose(file);
}
void load_current_lesson(char *current_user, int *current_lesson)
{
    char folder_name[] = "users/";
    char filename[40];
    strcpy(filename, strcat(folder_name, current_user));
    FILE *file;
    file = fopen(filename, "r");
    fscanf(file, "%d", current_lesson);
    fclose(file);
}
void get_current_user_data(UserHistory *holder, char *user_name)
{
    char folder_name[] = "users/";
    char filename[40];
    strcpy(filename, strcat(folder_name, user_name));
    FILE *user_file = fopen(filename, "r");

    fscanf(user_file, "%d %f %d", &holder->lesson_number, &holder->accuracy, &holder->time);

    fclose(user_file);
}
int get_selected_index(int arr[])
{
    int selected_user_index = 0;
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (arr[i])
        {
            selected_user_index = i;
            break;
        }
    }
    return selected_user_index;
}
