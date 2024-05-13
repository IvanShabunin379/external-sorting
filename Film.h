#pragma once
#include <string>
#include <iostream>

class Film {
private:
	std::string title; // ��������
	int yearOfRelease; // ��� �������
	std::string studio; // ����������
	std::string director; // �������
	int duration; // ������������ � �������
	bool hasAward; // ������� �������
	std::string starring[3]; // � ������� �����
public:
	Film() = default;;
	Film(const std::string title, int yearOfRelease, const std::string& studio, const std::string& director, int duration, bool hasAward, const std::string starring[3]);

	std::string get_title() const;
	int get_year_of_release() const;
	std::string get_studio() const;

	friend std::istream& operator>>(std::istream& in, Film& film);
	friend std::ostream& operator<<(std::ostream& out, const Film& film);
};

