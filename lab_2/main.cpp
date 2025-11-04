#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 1024
#define INITIAL_STATS_CAPACITY 10

// Структура для хранения статистики по одной строке файла.
typedef struct
{
    int lineNumber;
    int duplicateCharCount;
} LineStat;

/**
 * @brief Функция сравнения для qsort. Она является глобальной, так как используется
 *        внешней функцией qsort и не привязана к классу.
 * @param a Указатель на первый элемент (LineStat).
 * @param b Указатель на второй элемент (LineStat).
 * @return <0 если a < b, 0 если a == b, >0 если a > b по полю duplicateCharCount.
 */
int compareLineStats(const void *a, const void *b)
{
    const LineStat *statA = (const LineStat *)a;
    const LineStat *statB = (const LineStat *)b;
    return statA->duplicateCharCount - statB->duplicateCharCount;
}

/**
 * @brief Класс, отвечающий за анализ файла и хранение данных.
 */
class FileAnalyzer
{
private:
    char *filename;
    LineStat *stats;
    int lineCount;
    int capacity;

    /**
     * @brief Запрашивает у пользователя имя файла и сохраняет его в поле filename.
     */
    void GetFilenameFromUser()
    {
        char buffer[MAX_FILENAME_LENGTH];
        printf("Введите имя файла для анализа (например, test.txt):\n>>> ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0; // Убираем символ новой строки
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

    /**
     * @brief Подсчитывает количество уникальных символов, которые встречаются в строке более одного раза.
     * @param line Строка для анализа.
     * @return Количество символов-дубликатов.
     */
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

    /**
     * @brief Добавляет новую запись статистики в динамический массив, при необходимости увеличивая его емкость.
     * @param newStat Новая структура для добавления.
     */
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
    /**
     * @brief Конструктор класса. Инициализирует поля, выделяет начальную память и запрашивает имя файла.
     */
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

    /**
     * @brief Деструктор класса. Освобождает всю выделенную динамическую память.
     */
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

    /**
     * @brief Основной метод анализа. Открывает файл, читает его построчно и заполняет набор статистики.
     */
    void Analyze()
    {
        FILE *file_ptr = fopen(filename, "r");
        if (file_ptr == NULL)
        {
            perror("Не удалось открыть файл");
            return;
        }
        char lineBuffer[MAX_LINE_LENGTH];
        int currentLineNumber = 0;
        printf("\nАнализ файла '%s'...\n", filename);
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

    /**
     * @brief Предоставляет доступ к набору статистики для внешних операций (например, сортировки).
     * @return Указатель на начало массива LineStat.
     */
    LineStat *getStats()
    {
        return stats;
    }

    /**
     * @brief Возвращает текущее количество записей в наборе статистики.
     * @return Количество обработанных строк.
     */
    int getLineCount()
    {
        return lineCount;
    }

    /**
     * @brief Выводит содержимое набора статистики на экран в простом формате.
     */
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
    // 1 Создание объекта (Запрашивает у пользователя имя файла и сохраняет его в поле filename)
    FileAnalyzer analyzer;

    // 2 Выполнение анализа файла (Открывает файл, читает его построчно и заполняет набор статистики)
    analyzer.Analyze();

    // 3 Внешняя сортировка полученных данных
    if (analyzer.getLineCount() > 0)
    {
        qsort(analyzer.getStats(), analyzer.getLineCount(), sizeof(LineStat), compareLineStats);
    }

    // 4 Печать отсортированного результата
    analyzer.PrintStats();

    // 5 Завершение работы
    printf("\nНажмите Enter для завершения...");
    getchar();

    return 0;
}