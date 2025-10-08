#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 1024
#define INITIAL_STATS_CAPACITY 10

typedef struct
{
    int lineNumber;
    int duplicateCharCount;
} LineStat;

int compareLineStats(const void *a, const void *b)
{
    const LineStat *statA = (const LineStat *)a;
    const LineStat *statB = (const LineStat *)b;
    return statA->duplicateCharCount - statB->duplicateCharCount;
}

class FileAnalyzer
{
private:
    char *filename;
    LineStat *stats;
    int lineCount;
    int capacity;

    void GetFilenameFromUser()
    {
        char buffer[MAX_FILENAME_LENGTH];
        printf("Введите имя файла для анализа (например, test.txt):\n>>> ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
        }
        else
        {
            fprintf(stderr, "Ошибка чтения имени файла.\n");
            exit(EXIT_FAILURE);
        }
        filename = (char *)malloc(strlen(buffer) + 1);
        if (filename == NULL)
        {
            perror("Ошибка выделения памяти для имени файла");
            exit(EXIT_FAILURE);
        }
        strcpy(filename, buffer);
    }

    int CountDuplicateChars(const char *line)
    {
        int counts[256] = {0};
        for (int i = 0; line[i] != '\0'; ++i)
        {
            counts[(unsigned char)line[i]]++;
        }
        int duplicateCount = 0;
        for (int i = 0; i < 256; ++i)
        {
            if (counts[i] > 1)
            {
                duplicateCount++;
            }
        }
        return duplicateCount;
    }

    void AddStat(LineStat newStat)
    {
        if (lineCount == capacity)
        {
            capacity *= 2;
            LineStat *new_stats_ptr = (LineStat *)realloc(stats, capacity * sizeof(LineStat));
            if (new_stats_ptr == NULL)
            {
                perror("Ошибка перевыделения памяти для массива статистики");
                exit(EXIT_FAILURE);
            }
            stats = new_stats_ptr;
        }
        stats[lineCount] = newStat;
        lineCount++;
    }

public:
    FileAnalyzer()
    {
        filename = NULL;
        lineCount = 0;
        capacity = INITIAL_STATS_CAPACITY;
        stats = (LineStat *)malloc(capacity * sizeof(LineStat));
        if (stats == NULL)
        {
            perror("Ошибка выделения памяти в конструкторе");
            exit(EXIT_FAILURE);
        }
        GetFilenameFromUser();
    }

    ~FileAnalyzer()
    {
        if (filename)
        {
            free(filename);
            filename = NULL;
        }
        if (stats)
        {
            free(stats);
            stats = NULL;
        }
    }

    void Analyze()
    {
        FILE *file_ptr = fopen(filename, "r");
        if (file_ptr == NULL)
        {
            perror("Не удалось открыть файл");
            return;
        }
        printf("\nАнализ файла '%s'...\n", filename);
        char lineBuffer[MAX_LINE_LENGTH];
        int currentLineNumber = 0;
        while (fgets(lineBuffer, sizeof(lineBuffer), file_ptr) != NULL)
        {
            currentLineNumber++;
            int count = CountDuplicateChars(lineBuffer);
            LineStat currentStat = {currentLineNumber, count};
            AddStat(currentStat);
        }
        fclose(file_ptr);
        printf("Анализ завершен. Обработано строк: %d\n", lineCount);
    }

    LineStat *getStats() { return stats; }
    int getLineCount() { return lineCount; }

    void PrintStats()
    {
        if (lineCount == 0)
        {
            printf("Нет данных для вывода.\n");
            return;
        }
        printf("\nРезультат (отсортировано по количеству символов с повторами):\n");
        for (int i = 0; i < lineCount; ++i)
        {
            printf("Строка №%d: найдено %d символов с повторами.\n", stats[i].lineNumber, stats[i].duplicateCharCount);
        }
    }
};

int main()
{
    // 1. Создаем объект, который отвечает за анализ файла
    FileAnalyzer analyzer;

    // 2. Вызываем метод, который заполняет объект данными из файла
    analyzer.Analyze();

    // 3. Сортировка
    LineStat *dataToSort = analyzer.getStats();
    int dataCount = analyzer.getLineCount();

    if (dataCount > 0)
    {
        qsort(dataToSort, dataCount, sizeof(LineStat), compareLineStats);
        printf("\nДанные были отсортированы.\n");
    }

    // 4. Вызываем метод объекта для печати результата
    analyzer.PrintStats();

    printf("\nНажмите Enter для завершения...");
    getchar();
    return 0;
}