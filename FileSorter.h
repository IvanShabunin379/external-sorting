#pragma once
#include <fstream>
#include <string>
#include <functional>
#include <cstdio>

template <typename T>
class FileSorter {
private:
	static const size_t COUNT_WAYS = 4;

	static void distribute(const std::string& mainFileName, const std::string* auxFilesNames, std::function<bool(const T& item1, const T& item2)> cmp);
	static void merge(const std::string& mainfileName, const std::string* auxFilesNames, size_t& countRuns, std::function<bool(const T& item1, const T& item2)> cmp);
public:
	static void sort(const std::string& fileName, std::function<bool(const T& item1, const T& item2)> cmp);
};


// Функция распределения серий из исходного файла во вспомогательные.
template<typename T>
inline void FileSorter<T>::distribute(const std::string& mainFileName, const std::string* auxFilesNames, std::function<bool(const T& item1, const T& item2)> cmp)
{
	// Открываем основной файл на чтение и вспомогательные файлы на запись.
	std::ifstream mainFile(mainFileName);
	std::ofstream auxFiles[COUNT_WAYS];
	for (int i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].open(auxFilesNames[i]);

	T curItem, prevItem;
	bool endOfMain = !(mainFile >> curItem);
	size_t i = 0; // индекс файла, в который будет записываться текущий элемент

	// Если главный файл не пуст, сначала записываем первый элемент из него в первый вспомогательный файл.
	if (!endOfMain)
	{
		auxFiles[i] << curItem;
		prevItem = curItem;
		endOfMain = !(mainFile >> curItem);
	}

	while (!endOfMain) // Пока файл не закончен
	{
		// Если серия закончена, то начинаем записывать элементы в следующий файл. 
		// Признак конца серии - текущий элемент меньше предыдущего.
		if (cmp(curItem, prevItem))
			i = (i + 1) % COUNT_WAYS;

		// Запись текущего элемента в текущий файл.
		auxFiles[i] << curItem;
		prevItem = curItem;
		endOfMain = !(mainFile >> curItem);
	}

	// Закрываем основной и вспомогательные файлы.
	mainFile.close();
	for (size_t i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].close();
}

// Функция слияния серий из вспомогательный файлов в исходный с подсчетом количества серий.
template<typename T>
inline void FileSorter<T>::merge(const std::string& mainFileName, const std::string* auxFilesNames, size_t& countRuns, std::function<bool(const T& item1, const T& item2)> cmp)
{
	// Открываем основной файл на запись и вспомогательные файлы на чтение.
	std::ofstream mainFile(mainFileName);
	std::ifstream auxFiles[COUNT_WAYS];
	for (int i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].open(auxFilesNames[i]);

	countRuns = 0; 

	// Структура для хранения актуальной информации о вспомогательных файлах.
	struct {
		T curItem;
		bool endOfRun;
		bool endOfFile;
	} auxFilesInfo[COUNT_WAYS];
	
	// Лямбда для нахождения индекса минимального элемента из текущих элементов вспом. файлов, если они входят в текущую серию. Если серия закончилась, возвращает -1.
	auto find_ind_min = [&auxFilesInfo, cmp]()
	{
		int res = -1;
		for (int i = 0; i < COUNT_WAYS; ++i)
		{
			if (!auxFilesInfo[i].endOfRun)
			{
				if (res == -1)
					res = i;
				else
					if (cmp(auxFilesInfo[i].curItem, auxFilesInfo[res].curItem))
						res = i;
			}
		}

		return res;
	};

	// Считываем первые элементы всех вспомогательных файлов, одновременно проверяя их на пустоту.
	for (int i = 0; i < COUNT_WAYS; ++i)
		auxFilesInfo[i].endOfRun = auxFilesInfo[i].endOfFile = !(auxFiles[i] >> auxFilesInfo[i].curItem);

	bool allAuxEnded = false;
	while (!allAuxEnded) // Пока все вспомогательные файлы не закончились
	{
		int ind = find_ind_min(); // Находим индекс файла, из которого нужно записать элемент.
		if (ind != -1) // Если нашли
		{
			mainFile << auxFilesInfo[ind].curItem; // Записываем этот элемент в главный файл
			
			// Считываем следущий элемент из вспомогательного файла, одновременно проверяя этот файл на конец серии и на конец в целом.
			T item;
			auxFilesInfo[ind].endOfFile = auxFilesInfo[ind].endOfRun = !(auxFiles[ind] >> item);
			if (!auxFilesInfo[ind].endOfFile)
			{
				auxFilesInfo[ind].endOfRun = cmp(item, auxFilesInfo[ind].curItem);
				auxFilesInfo[ind].curItem = item;
			}
		}
		else // Если серия закончена
		{
			// Проверяем не закончилась ли все файлы.
			// И если нет, то переходим к следующей серии в каждом незаконченном файле
			allAuxEnded = true;

			for (int i = 0; i < COUNT_WAYS; ++i)
			{
				if (!auxFilesInfo[i].endOfFile)
				{
					auxFilesInfo[i].endOfRun = false;
					allAuxEnded = false;
				}
			}

			++countRuns;
		}
	}

	// Закрываем основной и вспомогательные файлы.
	mainFile.close();
	for (size_t i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].close();
}

// Сортировка файла с помощью многопутевого естественного несбалансированного слияния
template<typename T>
inline void FileSorter<T>::sort(const std::string& fileName, std::function<bool(const T& item1, const T& item2)> cmp)
{
	// Создание имён для вспомогательных файлов.
	std::string auxFilesNames[COUNT_WAYS];
	for (size_t i = 0; i < COUNT_WAYS; i++)
		auxFilesNames[i] = "auxFile" + std::to_string(i + 1) + ".txt";

	size_t countRuns;
	do 
	{
		distribute(fileName, auxFilesNames, cmp); // Распределение серий
		merge(fileName, auxFilesNames, countRuns, cmp); // Слияние серий
	} while (countRuns > 1);

	// Удаление вспомогательных файлов.
	for (size_t i = 0; i < COUNT_WAYS; i++)
		std::remove(auxFilesNames[i].c_str());
}
