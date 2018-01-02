#ifndef CCOMMAND_H
#define CCOMMAND_H

class Server;

//Structure for a command in the command lookup table.
class Command
{
public:
	std::string name;
    //void (*cmd_func)(Server_ptr server, User *user, std::string argument);
    void (*cmd_func)(Character * ch, std::string argument);
    int	level;
    int isMovement;
    int whileCorpse;
    int whileGhost;

	static bool Interpret(Character * ch, std::string argument);
	static Command * GetCurrentCmdTable(Character * ch);
};

#endif