#include "stdafx.h"
#include "CUser.h"
#include "utils.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace StringEdit
{

std::string string_linedel(std::string, int );
std::string string_lineadd(std::string, std::string, int );
std::string NumberLines(std::string);
std::string first_arg( const std::string & argument, std::string & arg_first, bool fCase );

/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit(User * user, std::string * pString)
{
    user->Send( "-========- Entering EDIT Mode -=========-\r\n");
    user->Send( "    Type .h on a new line for help\r\n");
    user->Send( " Terminate with a @ on a blank line.\r\n");
    user->Send( "-=======================================-\r\n");

    user->stringEdit = pString;
    user->stringEdit->clear();
}

/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append(User * user, std::string * pString)
{
    user->Send( "-=======- Entering APPEND Mode -========-\r\n");
    user->Send( "    Type .h on a new line for help\r\n" );
    user->Send( " Terminate with a @ on a blank line.\r\n");
    user->Send( "-=======================================-\r\n");

    user->Send( NumberLines(*pString));
    user->stringEdit = pString;

    return;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
//Find old in orig, and replace with replace
std::string string_replace(std::string orig, std::string old, std::string replace )
{
    int pos = (int)orig.find(old);
    if(pos == std::string::npos)
        return orig;

    return orig.replace(pos, old.length(), replace);
}



/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( User * user, std::string argument )
{
	std::string buf;

    /*
     * Thanks to James Seng
     */
    //smash_tilde( argument );

    if (!argument.empty() && argument[0] == '.')
    {
		std::string arg1;
		std::string arg2;
		std::string arg3;
		std::string tmparg3;

        argument = Utilities::one_argument( argument, arg1 );
        argument = first_arg( argument, arg2, false );
	    tmparg3 = argument;
        argument = first_arg( argument, arg3, false );

        if ( !Utilities::str_cmp( arg1, ".c" ) )
        {
            user->Send( "String cleared.\r\n" );
            user->stringEdit->clear();
            return;
        }

        if ( !Utilities::str_cmp( arg1, ".s" ) )
        {
            user->Send( "String so far:\r\n" );
            user->Send( NumberLines(*user->stringEdit) );
            return;
        }

        if ( !Utilities::str_cmp( arg1, ".r" ) )
        {
            if ( arg2.empty() )
            {
                user->Send("usage:  .r \"old string\" \"new string\"\r\n" );
                return;
            }

            *user->stringEdit = string_replace( *user->stringEdit, arg2, arg3 );
            user->Send("'" + arg2 + "' replaced with '" + arg3 + "'.\r\n");
            return;
        }

        if ( !Utilities::str_cmp( arg1, ".f" ) )
        {
            //*user->stringEdit = format_string( *user->stringEdit );
            //user->Send( "String formatted.\r\n" );
            return;
        }
        
	    if ( !Utilities::str_cmp( arg1, ".ld" ) )
	    {
		    *user->stringEdit = string_linedel( *user->stringEdit, Utilities::atoi(arg2) );
		    user->Send( "Line erased.\r\n" );
		    return;
	    }

	    if ( !Utilities::str_cmp( arg1, ".li" ) )
	    {
		    *user->stringEdit = string_lineadd( *user->stringEdit, tmparg3, Utilities::atoi(arg2) );
		    user->Send( "Line inserted.\r\n" );
		    return;
	    }

	    if ( !Utilities::str_cmp( arg1, ".lr" ) )
	    {
		    *user->stringEdit = string_linedel( *user->stringEdit, Utilities::atoi(arg2) );
		    *user->stringEdit = string_lineadd( *user->stringEdit, tmparg3, Utilities::atoi(arg2) );
		    user->Send( "Line replaced.\r\n" );
		    return;
	    }

        if ( !Utilities::str_cmp( arg1, ".h" ) )
        {
            user->Send( "Sedit help (commands on blank line):   \r\n" );
            user->Send( ".r 'old' 'new'   - replace a substring \r\n" );
            user->Send( "                   (requires '', \"\") \r\n" );
            user->Send( ".h               - get help (this info)\r\n" );
            user->Send( ".s               - show string so far  \r\n" );
            //user->Send( ".f               - (word wrap) string  \r\n" );
            user->Send( ".c               - clear string so far \r\n" );
            user->Send( ".ld <num>        - delete line number <num>\r\n" );
            user->Send( ".li <num> <str>  - insert <str> before line <num>\r\n" );
	        user->Send( ".lr <num> <str>  - replace line <num> with <str>\r\n" );
            user->Send( "@                - end string          \r\n" );
            return;
        }

        user->Send( "SEdit:  Invalid dot command.\r\n" );
        return;
    }

    if ( !argument.empty() && argument[0] == '@' )
    {
        user->stringEdit = nullptr;
        return;
    }

    buf = *user->stringEdit;

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     * --------------------------------------
     */
    if ( buf.length() + argument.length() >= ( 4604 ) )
    {
        user->Send( "String too long, last line skipped.\r\n" );

	/* Force character out of editing mode. */
        user->stringEdit = nullptr;
        return;
    }

    /*
     * Ensure no tilde's inside string.
     * --------------------------------
     */
    //smash_tilde( argument );

    buf += argument + "\r\n";
    *user->stringEdit = buf;
    return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
/*char *format_string( char *oldstring )
{
  char xbuf[MAX_STRING_LENGTH];
  char xbuf2[MAX_STRING_LENGTH];
  char *rdesc;
  int i=0;
  bool cap=TRUE;
  
  xbuf[0]=xbuf2[0]=0;
  
  i=0;
  
  for (rdesc = oldstring; *rdesc; rdesc++)
  {
    if (*rdesc=='\n')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc=='\r') ;
    else if (*rdesc==' ')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc==')')
    {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!'))
      {
        xbuf[i-2]=*rdesc;
        xbuf[i-1]=' ';
        xbuf[i]=' ';
        i++;
      }
      else
      {
        xbuf[i]=*rdesc;
        i++;
      }
    }
    else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!') {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
        xbuf[i-2]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i-1]=' ';
          xbuf[i]=' ';
          i++;
        }
        else
        {
          xbuf[i-1]='\"';
          xbuf[i]=' ';
          xbuf[i+1]=' ';
          i+=2;
          rdesc++;
        }
      }
      else
      {
        xbuf[i]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i+1]=' ';
          xbuf[i+2]=' ';
          i += 3;
        }
        else
        {
          xbuf[i+1]='\"';
          xbuf[i+2]=' ';
          xbuf[i+3]=' ';
          i += 4;
          rdesc++;
        }
      }
      cap = TRUE;
    }
    else
    {
      xbuf[i]=*rdesc;
      if ( cap )
        {
          cap = FALSE;
          xbuf[i] = UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i]=0;
  strcpy(xbuf2,xbuf);
  
  rdesc=xbuf2;
  
  xbuf[0]=0;
  
  for ( ; ; )
  {
    for (i=0; i<77; i++)
    {
      if (!*(rdesc+i)) break;
    }
    if (i<77)
    {
      break;
    }
    for (i=(xbuf[0]?76:73) ; i ; i--)
    {
      if (*(rdesc+i)==' ') break;
    }
    if (i)
    {
      *(rdesc+i)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"\r\n");
      rdesc += i+1;
      while (*rdesc == ' ') rdesc++;
    }
    else
    {
      bug ("No spaces", 0);
      *(rdesc+75)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"-\r\n");
      rdesc += 76;
    }
  }
  while (*(rdesc+i) && (*(rdesc+i)==' '||
                        *(rdesc+i)=='\n'||
                        *(rdesc+i)=='\r'))
    i--;
  *(rdesc+i+1)=0;
  strcat(xbuf,rdesc);
  if (xbuf[strlen(xbuf)-2] != '\n')
    strcat(xbuf,"\r\n");

  free_string(oldstring);
  return(str_dup(xbuf));
}*/



/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
std::string first_arg( const std::string & argument, std::string & arg_first, bool fCase )
{
    char cEnd;
    int i = 0;
    arg_first.clear();

    while ( argument[i] == ' ' )
	    i++;

    cEnd = ' ';
    if ( argument[i] == '\'' || argument[i] == '"'
      || argument[i] == '%'  || argument[i] == '(' )
    {
        if ( argument[i] == '(' )
        {
            cEnd = ')';
            i++;
        }
        else cEnd = argument[i++];
    }

    while ( i < (int)argument.length() )
    {
	    if (argument[i] == cEnd )
	    {
	        i++;
	        break;
	    }
        if ( fCase ) 
            arg_first += Utilities::LOWER(argument[i]);
        else 
    	    arg_first += argument[i];
	    i++;
    }

    /*while ( argument[i] == ' ' )
		i++;*/

    return argument.substr(i, argument.length());
}

/*
 * Used in olc_act.c for aedit_builders.
 */
/*char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}*/



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
/*char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
        {
            s++;
        }
    }

    return argument;
}*/

std::string string_linedel(std::string str, int line)
{
    int linectr = 1;
    int first = 0, last = 0;

    while((last = (int)str.find('\n', first)) != std::string::npos)
    {
        if(linectr == line)
        {
            if(str[last+1] == '\r')
                ++last;
            str.replace(first, last-first, "");
            break;
        }
        ++linectr;
        first = last + 1;
        if(str[first] == '\r')
            ++first;
    }
    return str;
}

//Inserts stradd into str at line
std::string string_lineadd(std::string str, std::string stradd, int line)
{
    int linectr = 1;
    int first = 0, last = 0;

    while((last = (int)str.find('\n', first)) != std::string::npos)
    {
        if(linectr == line)
        {
            str.insert(first, stradd + "\r\n");
            break;
        }
        ++linectr;
        first = last + 1;
        if(str[first] == '\r')
            ++first;
    }
    return str;
}

std::string NumberLines(std::string str)
{
	std::stringstream formatted;
    int first = 0, last = 0;
    int ctr = 1;

    do{
        last = (int)str.find('\n', first);
        if(last != std::string::npos)
            formatted << std::setw(2) << ctr << ". " << str.substr(first, last-first) << "\r\n";
        first = last+1;
        ++ctr;
    }while(last != std::string::npos);

    return formatted.str();
}

} //namespace StringEdit