#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CItem.h"
#include "CSkill.h"
#include "CClass.h"
#include "CExit.h"
#include "CReset.h"
#include "CArea.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CPlayer.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "utils.h"
#include "mud.h"

using namespace std;

namespace StringEdit
{

string string_linedel( string, int );
string string_lineadd( string, string, int );
string NumberLines(string);
string first_arg( const string & argument, string & arg_first, bool fCase );

/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit(User * user, string * pString)
{
    user->Send( "-========- Entering EDIT Mode -=========-\n\r");
    user->Send( "    Type .h on a new line for help\n\r");
    user->Send( " Terminate with a @ on a blank line.\n\r");
    user->Send( "-=======================================-\n\r");

    user->stringEdit = pString;
    user->stringEdit->clear();
}

/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append(User * user, string * pString)
{
    user->Send( "-=======- Entering APPEND Mode -========-\n\r");
    user->Send( "    Type .h on a new line for help\n\r" );
    user->Send( " Terminate with a @ on a blank line.\n\r");
    user->Send( "-=======================================-\n\r");

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
string string_replace( string orig, string old, string replace )
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
void string_add( User * user, string argument )
{
    string buf;

    /*
     * Thanks to James Seng
     */
    //smash_tilde( argument );

    if (!argument.empty() && argument[0] == '.')
    {
        string arg1;
        string arg2;
        string arg3;
        string tmparg3;

        argument = Utilities::one_argument( argument, arg1 );
        argument = first_arg( argument, arg2, false );
	    tmparg3 = argument;
        argument = first_arg( argument, arg3, false );

        if ( !Utilities::str_cmp( arg1, ".c" ) )
        {
            user->Send( "String cleared.\n\r" );
            user->stringEdit->clear();
            return;
        }

        if ( !Utilities::str_cmp( arg1, ".s" ) )
        {
            user->Send( "String so far:\n\r" );
            user->Send( NumberLines(*user->stringEdit) );
            return;
        }

        if ( !Utilities::str_cmp( arg1, ".r" ) )
        {
            if ( arg2.empty() )
            {
                user->Send("usage:  .r \"old string\" \"new string\"\n\r" );
                return;
            }

            *user->stringEdit = string_replace( *user->stringEdit, arg2, arg3 );
            user->Send("'" + arg2 + "' replaced with '" + arg3 + "'.\n\r");
            return;
        }

        if ( !Utilities::str_cmp( arg1, ".f" ) )
        {
            //*user->stringEdit = format_string( *user->stringEdit );
            //user->Send( "String formatted.\n\r" );
            return;
        }
        
	    if ( !Utilities::str_cmp( arg1, ".ld" ) )
	    {
		    *user->stringEdit = string_linedel( *user->stringEdit, Utilities::atoi(arg2) );
		    user->Send( "Line erased.\n\r" );
		    return;
	    }

	    if ( !Utilities::str_cmp( arg1, ".li" ) )
	    {
		    *user->stringEdit = string_lineadd( *user->stringEdit, tmparg3, Utilities::atoi(arg2) );
		    user->Send( "Line inserted.\n\r" );
		    return;
	    }

	    if ( !Utilities::str_cmp( arg1, ".lr" ) )
	    {
		    *user->stringEdit = string_linedel( *user->stringEdit, Utilities::atoi(arg2) );
		    *user->stringEdit = string_lineadd( *user->stringEdit, tmparg3, Utilities::atoi(arg2) );
		    user->Send( "Line replaced.\n\r" );
		    return;
	    }

        if ( !Utilities::str_cmp( arg1, ".h" ) )
        {
            user->Send( "Sedit help (commands on blank line):   \n\r" );
            user->Send( ".r 'old' 'new'   - replace a substring \n\r" );
            user->Send( "                   (requires '', \"\") \n\r" );
            user->Send( ".h               - get help (this info)\n\r" );
            user->Send( ".s               - show string so far  \n\r" );
            //user->Send( ".f               - (word wrap) string  \n\r" );
            user->Send( ".c               - clear string so far \n\r" );
            user->Send( ".ld <num>        - delete line number <num>\n\r" );
            user->Send( ".li <num> <str>  - insert <str> before line <num>\n\r" );
	        user->Send( ".lr <num> <str>  - replace line <num> with <str>\n\r" );
            user->Send( "@                - end string          \n\r" );
            return;
        }

        user->Send( "SEdit:  Invalid dot command.\n\r" );
        return;
    }

    if ( /**argument == '~' ||*/ !argument.empty() && argument[0] == '@' )
    {
	    /*if ( ch->desc->editor == ED_MPCODE )
	    {
		    MOB_INDEX_DATA *mob;
		    int hash;
		    MPROG_LIST *mpl;
		    MPROG_CODE *mpc;

		    EDIT_MPCODE(ch, mpc);

		    if ( mpc != NULL )
			    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
				    for ( mob = mob_index_hash[hash]; mob; mob = mob->next )
					    for ( mpl = mob->mprogs; mpl; mpl = mpl->next )
						    if ( mpl->vnum == mpc->vnum )
						    {
							    sprintf( buf, "Changed mob %d.\n\r", mob->vnum );
							    send_to_char( buf, ch );
							    mpl->code = mpc->code;
						    }
	    }*/

        user->stringEdit = NULL;
        return;
    }

    buf = *user->stringEdit;

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     * --------------------------------------
     */
    if ( buf.length() + argument.length() >= ( 4604 ) )
    {
        user->Send( "String too long, last line skipped.\n\r" );

	/* Force character out of editing mode. */
        user->stringEdit = NULL;
        return;
    }

    /*
     * Ensure no tilde's inside string.
     * --------------------------------
     */
    //smash_tilde( argument );

    buf += argument + "\n\r";
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
      strcat(xbuf,"\n\r");
      rdesc += i+1;
      while (*rdesc == ' ') rdesc++;
    }
    else
    {
      bug ("No spaces", 0);
      *(rdesc+75)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"-\n\r");
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
    strcat(xbuf,"\n\r");

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
string first_arg( const string & argument, string & arg_first, bool fCase )
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

string string_linedel(string str, int line)
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
//TODO update these functions to use reference parameters
string string_lineadd(string str, string stradd, int line)
{
    int linectr = 1;
    int first = 0, last = 0;

    while((last = (int)str.find('\n', first)) != std::string::npos)
    {
        if(linectr == line)
        {
            str.insert(first, stradd + "\n\r");
            break;
        }
        ++linectr;
        first = last + 1;
        if(str[first] == '\r')
            ++first;
    }
    return str;
}

string NumberLines(string str)
{
    stringstream formatted;
    int first = 0, last = 0;
    int ctr = 1;

    do{
        last = (int)str.find('\n', first);
        if(last != std::string::npos)
            formatted << setw(2) << ctr << ". " << str.substr(first, last-first) << "\n\r";
        first = last+1;
        ++ctr;
    }while(last != std::string::npos);

    return formatted.str();
}

} //namespace StringEdit