#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Константы для имен файлов
const char *FIO_FILE = "FIO.DAT";
const char *GROUP_FILE = "GROUP.DAT";
const char *LINK_FILE = "LINK.IDX";

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

    bool isGroupExists(const char *groupNumber)
    {
        rewind(groupFile);
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), groupFile) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strcmp(buffer, groupNumber) == 0)
                return true;
        }
        return false;
    }

    /**
     * @brief Находит индекс группы по ее номеру.
     * @param groupNumber Номер группы для поиска.
     * @return Адрес группы в файле или -1, если группа не найдена.
     */
    long findGroupAddress(const char *groupNumber)
    {
        rewind(groupFile);
        char buffer[100];
        long position;
        while (!feof(groupFile))
        {
            position = ftell(groupFile);
            if (fgets(buffer, sizeof(buffer), groupFile) != NULL)
            {
                buffer[strcspn(buffer, "\n")] = 0;
                if (strcmp(buffer, groupNumber) == 0)
                {
                    return position; // Нашли группу, возвращаем ее адрес
                }
            }
        }
        return -1; // Группа не найдена
    }

public:
    StudentGroupDB()
    {
        fioFile = fopen(FIO_FILE, "a+");
        groupFile = fopen(GROUP_FILE, "a+");
        linkFile = fopen(LINK_FILE, "ab+");

        if (!fioFile || !groupFile || !linkFile)
        {
            printf("Критическая ошибка: не удалось открыть файлы базы данных.\n");
            exit(1);
        }
    }

    ~StudentGroupDB()
    {
        if (fioFile)
        {
            fclose(fioFile);
            fioFile = NULL;
        }
        if (groupFile)
        {
            fclose(groupFile);
            groupFile = NULL;
        }
        if (linkFile)
        {
            fclose(linkFile);
            linkFile = NULL;
        }
        printf("Файлы базы данных закрыты.\n");
    }

    // 1. Процедура "Добавление новой группы"
    void addGroup()
    {
        char newGroupNumber[100];
        printf("Введите номер новой группы: ");
        scanf("%99s", newGroupNumber);
        clear_input_buffer();

        if (isGroupExists(newGroupNumber))
        {
            printf("Ошибка: группа с номером '%s' уже существует.\n", newGroupNumber);
            return;
        }

        fseek(groupFile, 0, SEEK_END);
        fprintf(groupFile, "%s\n", newGroupNumber);
        printf("Группа '%s' успешно добавлена.\n", newGroupNumber);
    }

    // 2. Процедура "Включение новой фамилии студента с указанием группы"
    void addStudentWithLink()
    {
        char targetGroupNumber[100];
        printf("Введите НОМЕР группы для нового студента: ");
        scanf("%99s", targetGroupNumber);
        clear_input_buffer();

        long group_idx = findGroupAddress(targetGroupNumber);
        if (group_idx == -1)
        {
            printf("Ошибка: группы с номером '%s' не существует. Сначала добавьте группу.\n", targetGroupNumber);
            return;
        }

        char newStudentName[100];
        printf("Введите фамилию студента: ");
        scanf("%99s", newStudentName);
        clear_input_buffer();

        fseek(fioFile, 0, SEEK_END);
        unsigned long student_idx = ftell(fioFile);
        fprintf(fioFile, "%s\n", newStudentName);

        Link newLink = {(unsigned long)student_idx, (unsigned long)group_idx};
        fseek(linkFile, 0, SEEK_END);
        fwrite(&newLink, sizeof(Link), 1, linkFile);

        printf("Студент '%s' успешно добавлен и привязан к группе '%s'.\n", newStudentName, targetGroupNumber);
    }

    // 3. Процедура "Поиск студентов по группе"
    void findStudentsByGroup()
    {
        char targetGroupNumber[100];
        printf("Введите НОМЕР группы для вывода списка студентов: ");
        scanf("%99s", targetGroupNumber);
        clear_input_buffer();

        long group_idx = findGroupAddress(targetGroupNumber);
        if (group_idx == -1)
        {
            printf("Ошибка: группа с номером '%s' не найдена.\n", targetGroupNumber);
            return;
        }

        printf("\nСтуденты в группе '%s':\n", targetGroupNumber);

        rewind(linkFile);
        Link currentLink;
        int count = 0;

        while (fread(&currentLink, sizeof(Link), 1, linkFile) == 1)
        {
            if (currentLink.group_idx == (unsigned long)group_idx)
            {
                fseek(fioFile, currentLink.student_idx, SEEK_SET);
                char studentBuffer[100];
                if (fgets(studentBuffer, sizeof(studentBuffer), fioFile) != NULL)
                {
                    studentBuffer[strcspn(studentBuffer, "\n")] = 0;
                    printf("- %s\n", studentBuffer);
                    count++;
                }
            }
        }
        if (count == 0)
            printf("В этой группе нет студентов.\n");
    }

    // Вспомогательная функция для вывода списка групп (по заданию не обязательна, но полезна)
    void listGroups()
    {
        printf("\n--- Текущий список групп ---\n");
        rewind(groupFile);
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), groupFile) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            printf("- %s\n", buffer);
        }
        printf("-----------------------------\n");
    }
};

int main()
{
    StudentGroupDB database;
    int choice = -1;

    do
    {
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
            choice = -1;
        }
        clear_input_buffer();

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

        if (choice != 0)
        {
            printf("\nНажмите Enter, чтобы продолжить...");
            getchar();
        }

    } while (choice != 0);

    return 0;
}