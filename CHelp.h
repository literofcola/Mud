#ifndef CHELP_H
#define CHELP_H

class Help
{
public:
	Help();
	Help(std::string name, std::string search_string, int id);
	~Help();
	void Save();

	int id;
	std::string title;
	std::string search_string;
	std::string text;

	bool changed;
	bool remove;
private:

};

#endif