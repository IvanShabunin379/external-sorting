/* 2A. Концерн "Видеосервис" предоставляет услуги по прокату и продаже видеофильмов частным
лицам и организациям. Сведения о фильмах, имеющихся в наличии хранятся в электронном виде.
Для облегчения выбора заказа покупателю предоставляется возможность сократить каталог просмотра
путем задания даты выхода фильма или, например киностудии, выпустившей этот фильм.
Сведения о фильмах включают в себя следующие данные:
- название;
- год выпуска;
- киностудия;
- режиссер;
- длительность фильма;
- наличие приза;
- три главные роли.

Вывести в алфавитном порядке сведения о видеофильмах с указанием киностудий, их выпустивших
в заданный период времени (для периода задан год начала и год конца).

Использовать многопутевое естественное несбалансированное слияние. */

#include "FileSorter.h"
#include "Film.h"
#include <Windows.h>


void menu();
int input_value(int leftLim = INT_MIN, int rightLim = INT_MAX, const char* msg = "-> ");
size_t find_good_films(std::ifstream& allFilms, std::ofstream& goodFilms, int beginning, int end);
void print_films(std::ifstream& films);


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    FileSorter<Film>::sort("films.txt", [](const Film& film1, const Film& film2) {return film1.get_year_of_release() < film2.get_year_of_release(); });
    menu();

    return 0;
}


void menu()
{
    int choice = 0;
    do
    {
        std::cout << "1. Вывести все фильмы, выпущенные в заданный период времени\n";
        std::cout << "0. Завершить работу программы\n";
        choice = input_value(0, 1);

        if (choice == 1)
        {
            int beginning = input_value(1900, 2023, "Введите год начала периода: ");
            int end = input_value(beginning, 2023, "Введите год конца периода: ");

            std::ifstream allFilms("films.txt");
            std::ofstream goodFilms("good_films.txt");

            size_t count = find_good_films(allFilms, goodFilms, beginning, end);
            allFilms.close();
            goodFilms.close();

            if (!count)
                std::cout << "В списке отсутствуют фильмы, выпущенные с " << beginning << " по " << end << " год:\n\n";
            else
            {
                FileSorter<Film>::sort("good_films.txt", [](const Film& film1, const Film& film2) {return film1.get_title() < film2.get_title(); });
                std::cout << "\nФильмы, выпущенные с " << beginning << " по " << end << " год:\n";
                std::ifstream goodFilms("good_films.txt");
                print_films(goodFilms);
                goodFilms.close();
            }

            std::remove("good_films.txt");
        }

    } while (choice);
    
}

size_t find_good_films(std::ifstream& allFilms, std::ofstream& goodFilms, int beginning, int end)
{
    size_t result = 0;

    Film curFilm;
    while (allFilms >> curFilm && curFilm.get_year_of_release() <= end)
        if (curFilm.get_year_of_release() >= beginning)
        {
            goodFilms << curFilm;
            ++result;
        }

    return result;
}

void print_films(std::ifstream& films)
{
    Film curFilm;
    while (films >> curFilm)
    {
        std::cout << "Название: " << curFilm.get_title() << '\n';
        std::cout << "Год выпуска: " << curFilm.get_year_of_release() << '\n';
        std::cout << "Киностудия: " << curFilm.get_studio() << "\n\n";
    }
}

int input_value(int leftLim, int rightLim, const char* msg)
{
    int value;
    bool isCorrectValue = false;

    while (!isCorrectValue)
    {
        std::cout << msg;
        std::cin >> value;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "Вы ввели некорректное значение! Повторите попытку:\n";
        }
        else if (value < leftLim || value > rightLim)
            std::cout << "Вы ввели значение вне требуемого диапазона! Повторите попытку:\n";
        else
            isCorrectValue = true;

        std::cin.ignore(32767, '\n');
    }

    return value;
}
