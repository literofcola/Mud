#ifndef CHELP_H
#define CHELP_H

class Help
{
public:
	Help();
	Help(std::string name, int id);
	~Help();
	void Save();

	int id;
	std::string name;
	std::string text;

	bool changed;
	bool remove;
private:

};

#endif