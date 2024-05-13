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


// ������� ������������� ����� �� ��������� ����� �� ���������������.
template<typename T>
inline void FileSorter<T>::distribute(const std::string& mainFileName, const std::string* auxFilesNames, std::function<bool(const T& item1, const T& item2)> cmp)
{
	// ��������� �������� ���� �� ������ � ��������������� ����� �� ������.
	std::ifstream mainFile(mainFileName);
	std::ofstream auxFiles[COUNT_WAYS];
	for (int i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].open(auxFilesNames[i]);

	T curItem, prevItem;
	bool endOfMain = !(mainFile >> curItem);
	size_t i = 0; // ������ �����, � ������� ����� ������������ ������� �������

	// ���� ������� ���� �� ����, ������� ���������� ������ ������� �� ���� � ������ ��������������� ����.
	if (!endOfMain)
	{
		auxFiles[i] << curItem;
		prevItem = curItem;
		endOfMain = !(mainFile >> curItem);
	}

	while (!endOfMain) // ���� ���� �� ��������
	{
		// ���� ����� ���������, �� �������� ���������� �������� � ��������� ����. 
		// ������� ����� ����� - ������� ������� ������ �����������.
		if (cmp(curItem, prevItem))
			i = (i + 1) % COUNT_WAYS;

		// ������ �������� �������� � ������� ����.
		auxFiles[i] << curItem;
		prevItem = curItem;
		endOfMain = !(mainFile >> curItem);
	}

	// ��������� �������� � ��������������� �����.
	mainFile.close();
	for (size_t i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].close();
}

// ������� ������� ����� �� ��������������� ������ � �������� � ��������� ���������� �����.
template<typename T>
inline void FileSorter<T>::merge(const std::string& mainFileName, const std::string* auxFilesNames, size_t& countRuns, std::function<bool(const T& item1, const T& item2)> cmp)
{
	// ��������� �������� ���� �� ������ � ��������������� ����� �� ������.
	std::ofstream mainFile(mainFileName);
	std::ifstream auxFiles[COUNT_WAYS];
	for (int i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].open(auxFilesNames[i]);

	countRuns = 0; 

	// ��������� ��� �������� ���������� ���������� � ��������������� ������.
	struct {
		T curItem;
		bool endOfRun;
		bool endOfFile;
	} auxFilesInfo[COUNT_WAYS];
	
	// ������ ��� ���������� ������� ������������ �������� �� ������� ��������� �����. ������, ���� ��� ������ � ������� �����. ���� ����� �����������, ���������� -1.
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

	// ��������� ������ �������� ���� ��������������� ������, ������������ �������� �� �� �������.
	for (int i = 0; i < COUNT_WAYS; ++i)
		auxFilesInfo[i].endOfRun = auxFilesInfo[i].endOfFile = !(auxFiles[i] >> auxFilesInfo[i].curItem);

	bool allAuxEnded = false;
	while (!allAuxEnded) // ���� ��� ��������������� ����� �� �����������
	{
		int ind = find_ind_min(); // ������� ������ �����, �� �������� ����� �������� �������.
		if (ind != -1) // ���� �����
		{
			mainFile << auxFilesInfo[ind].curItem; // ���������� ���� ������� � ������� ����
			
			// ��������� �������� ������� �� ���������������� �����, ������������ �������� ���� ���� �� ����� ����� � �� ����� � �����.
			T item;
			auxFilesInfo[ind].endOfFile = auxFilesInfo[ind].endOfRun = !(auxFiles[ind] >> item);
			if (!auxFilesInfo[ind].endOfFile)
			{
				auxFilesInfo[ind].endOfRun = cmp(item, auxFilesInfo[ind].curItem);
				auxFilesInfo[ind].curItem = item;
			}
		}
		else // ���� ����� ���������
		{
			// ��������� �� ����������� �� ��� �����.
			// � ���� ���, �� ��������� � ��������� ����� � ������ ������������� �����
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

	// ��������� �������� � ��������������� �����.
	mainFile.close();
	for (size_t i = 0; i < COUNT_WAYS; ++i)
		auxFiles[i].close();
}

// ���������� ����� � ������� ������������� ������������� ������������������� �������
template<typename T>
inline void FileSorter<T>::sort(const std::string& fileName, std::function<bool(const T& item1, const T& item2)> cmp)
{
	// �������� ��� ��� ��������������� ������.
	std::string auxFilesNames[COUNT_WAYS];
	for (size_t i = 0; i < COUNT_WAYS; i++)
		auxFilesNames[i] = "auxFile" + std::to_string(i + 1) + ".txt";

	size_t countRuns;
	do 
	{
		distribute(fileName, auxFilesNames, cmp); // ������������� �����
		merge(fileName, auxFilesNames, countRuns, cmp); // ������� �����
	} while (countRuns > 1);

	// �������� ��������������� ������.
	for (size_t i = 0; i < COUNT_WAYS; i++)
		std::remove(auxFilesNames[i].c_str());
}
