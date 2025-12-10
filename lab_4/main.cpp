#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <limits>
#include <algorithm>

class SportsManager
{
public:
    /**
     * @brief Добавляет нового спортсмена и связывает его с видом спорта.
     */
    void addAthlete()
    {
        std::string currentAthleteName, currentSportName;
        std::cout << "Введите ФИО спортсмена: ";
        std::getline(std::cin, currentAthleteName);
        if (currentAthleteName.empty())
            throw("Ошибка: ФИО не может быть пустым.");
        if (findAthlete(currentAthleteName) != -1)
            throw("Ошибка: Спортсмен уже существует.");

        std::cout << "Введите вид спорта: ";
        std::getline(std::cin, currentSportName);
        if (currentSportName.empty())
            throw("Ошибка: Название вида спорта не может быть пустым.");

        int sportIndex = getOrCreateSport(currentSportName);

        athleteNames.push_back(currentAthleteName);
        int athleteIndex = athleteNames.size() - 1;

        links.push_back({athleteIndex, sportIndex});
        std::cout << "=> Спортсмен '" << currentAthleteName << "' успешно добавлен." << std::endl;
    }

    /**
     * @brief Изменяет вид спорта для существующего спортсмена.
     */
    void changeAthleteSport()
    {
        std::string currentAthleteName, newSportName;
        std::cout << "Введите ФИО спортсмена: ";
        std::getline(std::cin, currentAthleteName);

        int athleteIndex = findAthlete(currentAthleteName);
        if (athleteIndex == -1)
            throw("Ошибка: Спортсмен не найден.");

        std::cout << "Введите новое название вида спорта: ";
        std::getline(std::cin, newSportName);
        if (newSportName.empty())
            throw("Ошибка: Название не может быть пустым.");

        int newSportIndex = getOrCreateSport(newSportName);

        for (auto &link : links)
        {
            if (link.first == athleteIndex)
            {
                link.second = newSportIndex;
                break;
            }
        }
        std::cout << "=> Вид спорта для '" << currentAthleteName << "' изменен на '" << newSportName << "'." << std::endl;
    }

    /**
     * @brief Выводит на экран (без сортировки) список спортсменов для указанного вида спорта.
     */
    void displayAthletesBySport()
    {
        std::string currentSportName;
        std::cout << "Введите название вида спорта: ";
        std::getline(std::cin, currentSportName);

        int sportIndex = findSport(currentSportName);
        if (sportIndex == -1)
            throw("Ошибка: Вид спорта не найден.");

        std::cout << "\n--- Спортсмены в виде спорта '" << currentSportName << "' ---" << std::endl;
        bool found = false;
        for (auto &link : links)
        {
            if (link.second == sportIndex)
            {
                std::cout << "- " << athleteNames[link.first] << std::endl;
                found = true;
            }
        }
        if (!found)
            std::cout << "В этом виде спорта нет спортсменов." << std::endl;
    }

    /**
     * @brief Выводит отсортированный по ФИО список всех спортсменов с их видами спорта.
     */
    void displayAllAthletesSorted()
    {
        if (links.empty())
        {
            std::cout << "Список спортсменов пуст." << std::endl;
            return;
        }

        auto sortedLinks = links;
        customQuickSortLinks(sortedLinks, 0, sortedLinks.size() - 1);

        std::cout << "\n--- Все спортсмены (отсортировано по ФИО) ---" << std::endl;
        for (auto &link : sortedLinks)
        {
            int athleteIdx = link.first;
            int sportIdx = link.second;
            std::cout << "- " << athleteNames[athleteIdx]
                      << " (" << sportNames[sportIdx] << ")" << std::endl;
        }
    }

    /**
     * @brief Выводит отсортированный по ФИО список спортсменов в указанном виде спорта.
     */
    void displayAthletesInSportSorted()
    {
        std::string currentSportName;
        std::cout << "Введите название вида спорта: ";
        std::getline(std::cin, currentSportName);

        int sportIndex = findSport(currentSportName);
        if (sportIndex == -1)
            throw("Ошибка: Вид спорта не найден.");

        std::vector<std::pair<int, int>> filteredLinks;
        for (auto &link : links)
        {
            if (link.second == sportIndex)
            {
                filteredLinks.push_back(link);
            }
        }

        if (filteredLinks.empty())
        {
            std::cout << "В этом виде спорта нет спортсменов." << std::endl;
            return;
        }

        customQuickSortLinks(filteredLinks, 0, filteredLinks.size() - 1);

        std::cout << "\n--- Спортсмены в виде спорта '" << currentSportName << "' (отсортировано по ФИО) ---" << std::endl;
        for (auto &link : filteredLinks)
        {
            std::cout << "- " << athleteNames[link.first] << std::endl;
        }
    }

private:
    std::vector<std::string> athleteNames;
    std::vector<std::string> sportNames;

    // Хранит пары индексов, связывая `athleteNames` и `sportNames`.
    // - pair.first:  индекс спортсмена в векторе `athleteNames`.
    // - pair.second: индекс вида спорта в векторе `sportNames`.
    std::vector<std::pair<int, int>> links;

    /**
     * @brief Ищет индекс спортсмена по его ФИО.
     */
    int findAthlete(std::string &name)
    {
        for (int i = 0; i < athleteNames.size(); ++i)
        {
            if (athleteNames[i] == name)
                return i;
        }
        return -1;
    }

    /**
     * @brief Ищет индекс вида спорта по его названию.
     */
    int findSport(std::string &name)
    {
        for (int i = 0; i < sportNames.size(); ++i)
        {
            if (sportNames[i] == name)
                return i;
        }
        return -1;
    }

    /**
     * @brief Находит или создает новый вид спорта.
     */
    int getOrCreateSport(std::string &name)
    {
        int sportIndex = findSport(name);
        if (sportIndex == -1)
        {
            sportNames.push_back(name);
            sportIndex = sportNames.size() - 1;
            std::cout << "-> Добавлен новый вид спорта: " << name << std::endl;
        }
        return sportIndex;
    }

    /**
     * @brief Часть алгоритма Quick Sort. Перераспределяет элементы относительно опорного.
     * @param linksToSort Ссылка на вектор, который нужно отсортировать.
     * @param low Начальный индекс подмассива.
     * @param high Конечный индекс подмассива.
     * @return Индекс, на котором теперь стоит опорный элемент.
     */
    int partitionLinks(std::vector<std::pair<int, int>> &linksToSort, int low, int high)
    {
        // 1. Выбираем опорный элемент (pivot). В нашем случае берем последний элемент в текущем диапазоне.
        //    Мы сравниваем не сами пары {индекс, индекс}, а ФИО спортсменов, на которые они указывают.
        std::string pivot_name = athleteNames[linksToSort[high].first];

        // 2. 'i' - это индекс последнего элемента, который МЕНЬШЕ опорного.
        //    Изначально таких элементов нет, поэтому 'i' указывает на позицию перед началом диапазона.
        int i = (low - 1);

        // 3. Проходим по всем элементам диапазона, КРОМЕ опорного (который в конце).
        for (int j = low; j <= high - 1; j++)
        {
            // 4. Сравниваем текущий элемент ('j') с опорным.
            if (athleteNames[linksToSort[j].first] <= pivot_name)
            {
                // 5. Если текущий элемент меньше или равен опорному:
                i++;                                       // Сначала сдвигаем границу "меньшей" группы.
                std::swap(linksToSort[i], linksToSort[j]); // Затем меняем текущий элемент с первым элементом "большей" группы.
                                                           // Таким образом, "маленький" элемент перемещается в "маленькую" группу.
            }
        }

        // 6. После цикла все элементы до 'i' включительно меньше опорного.
        //    Ставим опорный элемент на его законное место - сразу после "меньшей" группы.
        std::swap(linksToSort[i + 1], linksToSort[high]);

        // 7. Возвращаем индекс, на котором теперь стоит опорный элемент.
        return (i + 1);
    }

    /**
     * @brief Собственная рекурсивная реализация Quick Sort.
     * @param linksToSort Ссылка на вектор, который нужно отсортировать.
     * @param low Начальный индекс подмассива.
     * @param high Конечный индекс подмассива.
     */
    void customQuickSortLinks(std::vector<std::pair<int, int>> &linksToSort, int low, int high)
    {
        // Условие выхода из рекурсии: если в подмассиве 0 или 1 элемент, он уже отсортирован.
        if (low < high)
        {
            // 1. Разделяем массив на две части относительно опорного элемента.
            //    'pi' - это индекс, на котором опорный элемент оказался после разделения.
            int pi = partitionLinks(linksToSort, low, high);

            // 2. Рекурсивно вызываем сортировку для двух полученных подмассивов:
            //    - для левой части (элементы до опорного)
            customQuickSortLinks(linksToSort, low, pi - 1);
            //    - для правой части (элементы после опорного)
            customQuickSortLinks(linksToSort, pi + 1, high);
        }
    }
};

/**
 * @brief Отображает в консоли текстовое меню для пользователя.
 */
void printMenu()
{
    std::cout << "\n===== Меню =====\n"
              << "1. Добавить нового спортсмена\n"
              << "2. Изменить вид спорта для спортсмена\n"
              << "3. Показать спортсменов по виду спорта\n"
              << "4. Показать всех спортсменов (сортировка по ФИО)\n"
              << "5. Показать спортсменов в виде спорта (сортировка по ФИО)\n"
              << "0. Выход\n"
              << "Выберите опцию: ";
}

/**
 * @brief Главная функция программы. Точка входа.
 */
int main()
{
    SportsManager manager;
    int choice;
    while (true)
    {
        printMenu();
        std::cin >> choice;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка: некорректный ввод.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 0)
            break;
        try
        {
            switch (choice)
            {
            case 1:
                manager.addAthlete();
                break;
            case 2:
                manager.changeAthleteSport();
                break;
            case 3:
                manager.displayAthletesBySport();
                break;
            case 4:
                manager.displayAllAthletesSorted();
                break;
            case 5:
                manager.displayAthletesInSportSorted();
                break;
            default:
                std::cout << "Неверный выбор.\n";
            }
        }
        catch (char *e)
        {
            std::cerr << "Произошла ошибка: " << e << std::endl;
        }
        catch (std::exception &e)
        {
            std::cerr << "Произошла ошибка: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "Произошла неизвестная ошибка." << std::endl;
        }
    }
    return 0;
}