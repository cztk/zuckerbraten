#include "servercommands.h"

    vector<servcommand> servcommands;
    vector<servaliascommand> servcommandaliases;

    int hasservcmd(char * name)
    {
        int i;
        int l = servcommands.length();

        if( l > 0 )
        {
            for( i =0; i < l; i++ )
            {
                if( 0 == strcmp( name, servcommands[i].name ) ) return i;
            }
        }

        int a = servcommandaliases.length();
        if( a > 0 )
        {
            for( i =0; i < a; i++ )
            {
                if( 0 == strcmp( name, servcommandaliases[i].name ) ) return servcommandaliases[i].commandid;
            }
        }

        return -1;
    }

    bool addservcmd(char *name, int minprivilege, bool enabled, int (*run)(int, vector<char *>))
    {
        if( -1 == hasservcmd(name) )
        {
            servcommand ieatmemory = { name, minprivilege, enabled, run };
            servcommands.add( ieatmemory );
            return true;
        }
        return false;
    }

    bool addservcmdalias(char *name, char *aliasname)
    {
        int i = hasservcmd( name );
        if( -1 != i )
        {
            servaliascommand ieatmemorytoo = { aliasname, i };
            servcommandaliases.add( ieatmemorytoo );
            return true;
        }
        return false;
    }

    bool setenabledservcmd(char *name, bool enabled)
    {
        int i = hasservcmd( name );
        if( -1 != i )
        {
            servcommands[i].enabled = enabled;
            return true;
        }
        return false;
    }

    bool setminprivilegeservcmd(char *name, bool minprivilege)
    {
        int i = hasservcmd( name );
        if( -1 != i )
        {
            servcommands[i].minprivilege = minprivilege;
            return true;
        }
        return false;
    }

    int runservcmd(int cn, char *name, vector<char *> args)
    {
        int i = hasservcmd( name );
        
        if( -1 != i )
        {
            clientinfo *f = getinfo(cn);
            if( f->privilege >= servcommands[i].minprivilege && servcommands[i].enabled )
            {
                return servcommands[i].run( cn, args );
            }
            else
            {
                sendf(cn, 1, "ris", N_SERVMSG, "unknown command.");
            }
        }
        else
        {
            sendf(cn, 1, "ris", N_SERVMSG, "unknown command.");
        }
        return -1;
    }


//  ----------------
//  functions
//  ----------------

    int test(int cn, vector<char *> args)
    {
        sendf(cn, 1, "ris", N_SERVMSG, "you just wasted some time, thank you for testing me.");
        setenabledservcmd("#test", false);
        return 0;
    }

    int fcanspawnitem(int cn, vector<char *> args)
    {
        string msg;

        if( 3 > args.length() || ( 2 <= args.length() && 0 == strcmp("help", args[1]) ) )
        {
            formatstring( msg, "%s takes two arguments: item name and status. Enable quad: %s quad 1 . Disable yellow armour: %s yellowarmour 0", args[0], args[0], args[0] );
            sendf(cn, 1, "ris", N_SERVMSG, msg);
        }
        else
        {
            int itemnum = entities::entnum(args[1]);
            if(-1 == itemnum)
            {
                formatstring( msg, "%s is not a valid item. Try one of: shells, bullets, rockets, rounds, grenades, catridges, health, boost, greenarmour, yellowarmour, quad", args[1] );
                sendf(cn, 1, "ris", N_SERVMSG, msg);
                return -1;
            }

            if( forcements.length() < itemnum )
            {
                for(int i=forcements.length();i<itemnum;i++)
                {
                    forcements.add(false);
                }
            }
            formatstring( msg, "%s spawning is now set to %s", args[1], args[2] );
            sendf(cn, 1, "ris", N_SERVMSG, msg);
            forcements[itemnum] = ( 0 == strcmp("0" , args[2])  ) ? false : true;
        }
        return 0;
    }

    void installservcmds()
    {
        addservcmd("#test", 0, true, test);
        addservcmdalias("#test", "#foo");

        addservcmd("#fcanspawnitem", 1, true, fcanspawnitem);
    }

