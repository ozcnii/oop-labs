#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Константы для имен файлов
const char *FIO_FILE = "FIO.DAT";
const char *GROUP_FILE = "GROUP.DAT";
const char *LINK_FILE = "LINK.IDX";
const int MAX_RECORD_LENGTH = 100;

// Вспомогательная функция для очистки буфера ввода
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
 * @brief Класс для управления информационной базой данных
 */
class StudentGroupDB
{
private:
    struct Link
    {
        unsigned long student_idx;
        unsigned long group_idx;
    };

    FILE *fioFile;
    FILE *groupFile;
    FILE *linkFile;

    /**
     * @brief Проверяет, существует ли указанная группа.
     */
    bool isGroupExists(const char *groupNumber)
    {
        if (fseek(groupFile, 0, SEEK_SET) != 0)
        {
            perror("Ошибка при позиционировании в файле групп");
            return false;
        }
        char buffer[MAX_RECORD_LENGTH];
        while (fgets(buffer, sizeof(buffer), groupFile) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strcmp(buffer, groupNumber) == 0)
                return true;
        }
        if (ferror(groupFile))
        {
            perror("Ошибка чтения файла групп");
            return false;
        }
        return false;
    }

    /**
     * @brief Находит адрес записи группы по ее номеру.
     * @param groupNumber Номер группы для поиска.
     * @return Позиция в файле или -1, если не найдено.
     */
    long findGroupAddress(const char *groupNumber)
    {
        if (fseek(groupFile, 0, SEEK_SET) != 0)
        {
            perror("Ошибка при позиционировании в файле групп");
            return -1;
        }
        char buffer[MAX_RECORD_LENGTH];
        long position;
        while (!feof(groupFile))
        {
            position = ftell(groupFile);
            if (position == -1L)
            {
                perror("Ошибка при получении позиции в файле групп");
                return -1;
            }
            if (fgets(buffer, sizeof(buffer), groupFile) != NULL)
            {
                buffer[strcspn(buffer, "\n")] = 0;
                if (strcmp(buffer, groupNumber) == 0)
                {
                    return position; // Нашли группу, возвращаем ее адрес
                }
            }
        }
        if (ferror(groupFile))
        {
            perror("Ошибка чтения файла групп");
            return -1;
        }
        return -1; // Группа не найдена
    }

public:
    /**
     * @brief Конструктор базы данных. Открывает или создает все файлы.
     */
    StudentGroupDB()
    {
        fioFile = fopen(FIO_FILE, "a+");
        if (!fioFile)
        {
            perror("Критическая ошибка: не удалось открыть файл FIO.DAT");
            exit(1);
        }

        groupFile = fopen(GROUP_FILE, "a+");
        if (!groupFile)
        {
            perror("Критическая ошибка: не удалось открыть файл GROUP.DAT");
            if (fioFile)
                fclose(fioFile);
            exit(1);
        }

        linkFile = fopen(LINK_FILE, "ab+");
        if (!linkFile)
        {
            perror("Критическая ошибка: не удалось открыть файл LINK.IDX");
            if (fioFile)
                fclose(fioFile);
            if (groupFile)
                fclose(groupFile);
            exit(1);
        }
    }

    /**
     * @brief Деструктор базы данных. Закрывает все открытые файлы.
     */
    ~StudentGroupDB()
    {
        if (fioFile)
        {
            if (fclose(fioFile) != 0)
            {
                perror("Ошибка при закрытии файла FIO.DAT");
            }
            fioFile = NULL;
        }
        if (groupFile)
        {
            if (fclose(groupFile) != 0)
            {
                perror("Ошибка при закрытии файла GROUP.DAT");
            }
            groupFile = NULL;
        }
        if (linkFile)
        {
            if (fclose(linkFile) != 0)
            {
                perror("Ошибка при закрытии файла LINK.IDX");
            }
            linkFile = NULL;
        }
        printf("Файлы базы данных закрыты.\n");
    }

    /**
     * @brief Добавляет новую группу в файл GROUP.DAT
     */
    void addGroup()
    {
        char newGroupNumber[MAX_RECORD_LENGTH];
        printf("Введите номер новой группы: ");
        if (scanf("%99s", newGroupNumber) != 1)
        {
            printf("Ошибка: не удалось прочитать номер группы.\n");
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        if (isGroupExists(newGroupNumber))
        {
            printf("Ошибка: группа с номером '%s' уже существует.\n", newGroupNumber);
            return;
        }

        if (fseek(groupFile, 0, SEEK_END) != 0)
        {
            perror("Ошибка при позиционировании в конец файла групп");
            return;
        }

        if (fprintf(groupFile, "%s\n", newGroupNumber) < 0)
        {
            perror("Ошибка при записи группы в файл");
            return;
        }

        if (fflush(groupFile) != 0)
        {
            perror("Ошибка при сохранении данных группы");
            return;
        }

        printf("Группа '%s' успешно добавлена.\n", newGroupNumber);
    }

    /**
     * @brief Добавляет студента и связывает его с существующей группой.
     */
    void addStudentWithLink()
    {
        char targetGroupNumber[100];
        printf("Введите НОМЕР группы для нового студента: ");
        if (scanf("%99s", targetGroupNumber) != 1)
        {
            printf("Ошибка: не удалось прочитать номер группы.\n");
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        long group_idx = findGroupAddress(targetGroupNumber);
        if (group_idx == -1)
        {
            printf("Ошибка: группы с номером '%s' не существует. Сначала добавьте группу.\n", targetGroupNumber);
            return;
        }

        char newStudentName[MAX_RECORD_LENGTH];
        printf("Введите фамилию студента: ");
        if (scanf("%99s", newStudentName) != 1)
        {
            printf("Ошибка: не удалось прочитать фамилию студента.\n");
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        if (fseek(fioFile, 0, SEEK_END) != 0)
        {
            perror("Ошибка при позиционировании в конец файла ФИО");
            return;
        }

        long student_idx_long = ftell(fioFile);
        if (student_idx_long == -1L)
        {
            perror("Ошибка при получении позиции в файле ФИО");
            return;
        }
        unsigned long student_idx = (unsigned long)student_idx_long;

        if (fprintf(fioFile, "%s\n", newStudentName) < 0)
        {
            perror("Ошибка при записи ФИО студента в файл");
            return;
        }

        if (fflush(fioFile) != 0)
        {
            perror("Ошибка при сохранении данных студента");
            return;
        }

        Link newLink = {student_idx, (unsigned long)group_idx};
        if (fseek(linkFile, 0, SEEK_END) != 0)
        {
            perror("Ошибка при позиционировании в конец файла связей");
            return;
        }

        if (fwrite(&newLink, sizeof(Link), 1, linkFile) != 1)
        {
            perror("Ошибка при записи связи студент-группа");
            return;
        }

        if (fflush(linkFile) != 0)
        {
            perror("Ошибка при сохранении связи");
            return;
        }

        printf("Студент '%s' успешно добавлен и привязан к группе '%s'.\n", newStudentName, targetGroupNumber);
    }

    /**
     * @brief Выводит студентов, принадлежащих указанной группе.
     */
    void findStudentsByGroup()
    {
        char targetGroupNumber[100];
        printf("Введите НОМЕР группы для вывода списка студентов: ");
        if (scanf("%99s", targetGroupNumber) != 1)
        {
            printf("Ошибка: не удалось прочитать номер группы.\n");
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        long group_idx = findGroupAddress(targetGroupNumber);
        if (group_idx == -1)
        {
            printf("Ошибка: группа с номером '%s' не найдена.\n", targetGroupNumber);
            return;
        }

        printf("\nСтуденты в группе '%s':\n", targetGroupNumber);

        if (fseek(linkFile, 0, SEEK_SET) != 0)
        {
            perror("Ошибка при позиционировании в файле связей");
            return;
        }

        Link currentLink;
        int count = 0;

        while (fread(&currentLink, sizeof(Link), 1, linkFile) == 1)
        {
            if (currentLink.group_idx == (unsigned long)group_idx)
            {
                if (fseek(fioFile, currentLink.student_idx, SEEK_SET) != 0)
                {
                    perror("Ошибка при позиционировании в файле ФИО");
                    continue;
                }
                char studentBuffer[MAX_RECORD_LENGTH];
                if (fgets(studentBuffer, sizeof(studentBuffer), fioFile) != NULL)
                {
                    studentBuffer[strcspn(studentBuffer, "\n")] = 0;
                    printf("- %s\n", studentBuffer);
                    count++;
                }
                else if (ferror(fioFile))
                {
                    perror("Ошибка чтения ФИО студента");
                    continue;
                }
            }
        }

        if (ferror(linkFile))
        {
            perror("Ошибка чтения файла связей");
            return;
        }

        if (count == 0)
            printf("В этой группе нет студентов.\n");
    }

    /**
     * @brief Показывает список всех групп, сохраненных в базе.
     */
    void listGroups()
    {
        printf("\n--- Текущий список групп ---\n");
        if (fseek(groupFile, 0, SEEK_SET) != 0)
        {
            perror("Ошибка при позиционировании в файле групп");
            return;
        }
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), groupFile) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            printf("- %s\n", buffer);
        }
        if (ferror(groupFile))
        {
            perror("Ошибка чтения файла групп");
            return;
        }
        printf("-----------------------------\n");
    }
};

int main()
{
    // 1 Создаем объект базы данных (инициализирует файлы и проверяет доступность)
    StudentGroupDB database;
    int choice = -1;

    // 2 Запускаем основной цикл меню (позволяет пользователю выбирать операции)
    do
    {
        // 2.1 Очищаем экран и выводим список доступных действий
        system("clear || cls");
        printf("--- Меню ---\n");
        printf("1. Добавить новую группу\n");
        printf("2. Добавить нового студента\n");
        printf("3. Показать студентов по номеру группы\n");
        printf("4. Показать все группы\n");
        printf("0. Выход\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка: введено некорректное значение. Пожалуйста, введите число.\n");
            choice = -1;
        }
        clear_input_buffer();

        // 2.2 Обрабатываем выбранный пункт меню и вызываем соответствующую процедуру
        switch (choice)
        {
        case 1:
            database.addGroup();
            break;
        case 2:
            database.addStudentWithLink();
            break;
        case 3:
            database.findStudentsByGroup();
            break;
        case 4:
            database.listGroups();
            break;
        case 0:
            printf("Завершение работы программы.\n");
            break;
        default:
            printf("Неверный пункт меню.\n");
            break;
        }

        // 2.3 При необходимости ждем подтверждение пользователя перед повторным показом меню
        if (choice != 0)
        {
            printf("\nНажмите Enter, чтобы продолжить...");
            getchar();
        }

    } while (choice != 0);

    // 3 Завершаем работу программы
    return 0;
}