#include "Film.h"

Film::Film(const std::string title, int year, const std::string & studio, const std::string & director, int duration, bool hasAward, const std::string starring[3])
	: title(title), yearOfRelease(year), studio(studio), director(director), duration(duration), hasAward(hasAward)
{
	for (size_t i = 0; i < 3; ++i)
		this->starring[i] = starring[i];
}

std::string Film::get_title() const
{
	return title;
}

int Film::get_year_of_release() const
{
	return yearOfRelease;
}

std::string Film::get_studio() const
{
	return studio;
}

std::istream& operator>>(std::istream& fin, Film& film)
{
	std::string title;
	int yearOfRelease;
	std::string studio;
	std::string director;
	int duration;
	bool hasAward;
	std::string starring[3];

	if (std::getline(fin, title) &&
		(fin >> yearOfRelease >> std::ws) &&
		std::getline(fin, studio) &&
		std::getline(fin, director) &&
		(fin >> duration >> std::ws) &&
		(fin >> std::boolalpha >> hasAward >> std::ws) &&
		std::getline(fin, starring[0]) &&
		std::getline(fin, starring[1]) &&
		std::getline(fin, starring[2]))
	{
		film = Film(title, yearOfRelease, studio, director, duration, hasAward, starring);
	}

	if (fin.peek() == '\n')
		fin.ignore();

	return fin;
}

std::ostream& operator<<(std::ostream& fout, const Film& film)
{
	fout << film.title << '\n';
	fout << film.yearOfRelease << '\n';
	fout << film.studio << '\n';
	fout << film.director << '\n';
	fout << film.duration << '\n';
	fout << std::boolalpha << film.hasAward << '\n';
	for (const auto& actor : film.starring)
		fout << actor << '\n';
	fout << '\n';

	return fout;
}
