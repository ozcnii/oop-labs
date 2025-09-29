#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SENTENCE_INPUT_LENGTH 80
#define INITIAL_WORDS_ARRAY_CAPACITY 10

// my_strrev, так как это нестандартная функция
char *my_strrev(char *str)
{
    if (!str || !*str)
    {
        return str;
    }
    char *p1 = str, *p2 = str + strlen(str) - 1;
    while (p2 > p1)
    {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }
    return str;
}

// const void* требует стандартная функция qsort
int compareWordStringsByLength(const void *a, const void *b)
{
    const char *strA = *(const char **)a;
    const char *strB = *(const char **)b;
    return strlen(strA) - strlen(strB);
}

class Sentence
{
private:
    char *string;
    char **words;
    int wordCount;
    int wordCapacity;

    void AddWordString(char *token)
    {
        if (wordCount == wordCapacity)
        {
            wordCapacity *= 2;
            char **new_words_array = (char **)realloc(words, wordCapacity * sizeof(char *));
            if (new_words_array == NULL)
            {
                perror("Ошибка перевыделения памяти для массива указателей на строки-слова");
                exit(EXIT_FAILURE);
            }
            words = new_words_array;
        }

        words[wordCount] = (char *)malloc(strlen(token) + 1);
        if (words[wordCount] == NULL)
        {
            perror("Ошибка выделения памяти для строки-слова");
            exit(EXIT_FAILURE);
        }
        strcpy(words[wordCount], token);
        wordCount++;
    }

    void InputRawString()
    {
        printf("Введите предложение (не более %d символов, слова разделены пробелами):\n", MAX_SENTENCE_INPUT_LENGTH);

        char buffer[MAX_SENTENCE_INPUT_LENGTH + 1];
        int i = 0;
        int c;

        printf(">>> ");

        while ((c = getchar()) != '\n' && c != EOF && i < MAX_SENTENCE_INPUT_LENGTH)
        {
            buffer[i++] = (char)c;
        }
        buffer[i] = '\0';

        string = (char *)malloc(i + 1);
        if (string == NULL)
        {
            perror("Ошибка выделения памяти для исходной строки предложения");
            exit(EXIT_FAILURE);
        }
        strcpy(string, buffer);
    }

public:
    Sentence()
    {
        string = NULL;
        wordCount = 0;
        wordCapacity = INITIAL_WORDS_ARRAY_CAPACITY;

        words = (char **)malloc(wordCapacity * sizeof(char *));
        if (words == NULL)
        {
            perror("Ошибка выделения памяти для Sentence::words в конструкторе по умолчанию");
            exit(EXIT_FAILURE);
        }

        InputRawString();
        FormSetOfWords();
    }

    Sentence(char **sourceWordStrings, int count)
    {
        string = NULL;
        wordCount = 0;
        wordCapacity = INITIAL_WORDS_ARRAY_CAPACITY;

        if (count > wordCapacity)
        {
            wordCapacity = count;
        }

        words = (char **)malloc(wordCapacity * sizeof(char *));
        if (words == NULL)
        {
            perror("Ошибка выделения памяти для Sentence::words в параметризованном конструкторе");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < count; ++i)
        {
            AddWordString(sourceWordStrings[i]);
        }
    }

    ~Sentence()
    {
        if (string)
        {
            free(string);
            string = NULL;
        }
        for (int i = 0; i < wordCount; ++i)
        {
            if (words[i])
            {
                free(words[i]);
                words[i] = NULL;
            }
        }
        if (words)
        {
            free(words);
            words = NULL;
        }
    }

    void FormSetOfWords()
    {
        if (!string)
        {
            printf("Поле 'строка' объекта Sentence пусто. Нечего разбирать.\n");
            return;
        }

        // strtok изменяет исходную строку, делаем копию поля string
        char *temp_sentence_copy = (char *)malloc(strlen(string) + 1);
        if (temp_sentence_copy == NULL)
        {
            perror("Ошибка выделения памяти для временной копии поля 'строка'");
            exit(EXIT_FAILURE);
        }
        strcpy(temp_sentence_copy, string);

        char *token = strtok(temp_sentence_copy, " ");
        while (token != NULL)
        {
            AddWordString(token);
            token = strtok(NULL, " ");
        }
        free(temp_sentence_copy);
    }

    void SortWords()
    {
        qsort(words, wordCount, sizeof(char *), compareWordStringsByLength);
    }

    void ReverseWords()
    {
        if (!words || wordCount == 0)
        {
            // если набор слов пуст, делаем поле 'строка' пустым
            if (string)
            {
                free(string);
            }
            string = (char *)malloc(1);
            if (string == NULL)
            {
                perror("Ошибка выделения памяти для пустой строки");
                exit(EXIT_FAILURE);
            }
            string[0] = '\0';
            return;
        }

        int total_length = 0;
        for (int i = 0; i < wordCount; ++i)
        {
            total_length += strlen(words[i]);
            if (i < wordCount - 1)
            {
                total_length++; // для пробела между словами
            }
        }

        if (string)
        {
            free(string);
        }
        string = (char *)malloc(total_length + 1);
        if (string == NULL)
        {
            perror("Ошибка выделения памяти для результирующей строки");
            exit(EXIT_FAILURE);
        }
        string[0] = '\0';

        for (int i = 0; i < wordCount; ++i)
        {
            char *reversed_word_copy = (char *)malloc(strlen(words[i]) + 1);
            if (reversed_word_copy == NULL)
            {
                perror("Ошибка выделения памяти для перевернутой копии слова");
                exit(EXIT_FAILURE);
            }
            strcpy(reversed_word_copy, words[i]);
            my_strrev(reversed_word_copy);

            strcat(string, reversed_word_copy);
            free(reversed_word_copy);

            if (i < wordCount - 1)
            {
                strcat(string, " ");
            }
        }
    }

    void PrintSentenceString()
    {
        if (string && strlen(string) > 0)
        {
            puts(string);
        }
        else
        {
            printf("Поле 'строка' объекта Sentence пусто или NULL.\n");
        }
    }

    char **GetWords()
    {
        return words;
    }

    int GetWordsCount()
    {
        return wordCount;
    }
};

int main()
{
    Sentence originalSentence;

    printf("Исходное предложение:\n");
    originalSentence.PrintSentenceString();

    Sentence newSentence(originalSentence.GetWords(), originalSentence.GetWordsCount());

    newSentence.SortWords();

    newSentence.ReverseWords();

    printf("\nНовое предложение (слова по длине, символы перевернуты):\n");
    newSentence.PrintSentenceString();

    printf("\nНажмите Enter для завершения...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // пауза консоли

    return 0;
}