#include "servercommands.h"

    vector<servcommand> servcommands;
    vector<servaliascommand> servcommandaliases;

    /*
        Finds command entry number within servcommands vector. If none found, returns value is -1.
    */
    int hasservcmd(const char * name)
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

    /*
        Add a new command entry to servcommands vector.
    */
    bool addservcmd(const char *name, int minprivilege, bool enabled, int (*run)(int, vector<char *>))
    {
        if( -1 == hasservcmd(name) )
        {
            servcommand ieatmemory = { name, minprivilege, enabled, run };
            servcommands.add( ieatmemory );
            return true;
        }
        return false;
    }

    /*
        Add an alias for any command in servcommands vector. Aliases are stored in servcommandaliases vector and only contain the
        alias name and index number to the real command within servcommands vector.
    */
    bool addservcmdalias(const char *name, const char *aliasname)
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

    /*
        Enable/Disable a command.
    */
    bool setenabledservcmd(const char *name, bool enabled)
    {
        int i = hasservcmd( name );
        if( -1 != i )
        {
            servcommands[i].enabled = enabled;
            return true;
        }
        return false;
    }

    /*
        Sets minimum privilege to run a command.
    */
    bool setminprivilegeservcmd(const char *name, bool minprivilege)
    {
        int i = hasservcmd( name );
        if( -1 != i )
        {
            servcommands[i].minprivilege = minprivilege;
            return true;
        }
        return false;
    }

    /*
        Try running a command. Takes id of the clientnumber calling the command, command name without starting #
        aswell as a list of arguments.
        First argument in args vector should be the command which has been called ( or the alias command name  ) with starting #
        Called functions shall not return -1
        The help command may return -1 because of lazyness 
    */
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
        }
        string msg;
        formatstring( msg, "\f3unknown command: \f0%s", name);
        sendf(cn, 1, "ris", N_SERVMSG, msg);
        return -1;
    }

//  ----------------
//  functions
//  ----------------

    /*
        Resets all gamemode modifications altering gameplay.
        TODO use default values from config.
    */
    int srvcmd_resetmodifications(int cn, vector<char *> args)
    {
        forcements.setsize(0);

        return 0;
    }

    /*
        If a command a user requests help for is found, the command will be called
        with "help" as first parameter ( after the #commandname argument in args list )
    */
    int srvcmd_help(int cn, vector<char *> args)
    {
        vector<char *> args2;
        if( 2 <= args.length() )
        {
            string commandarg;
            formatstring( commandarg, "#%s", args[1] );
            char *helpcmd = (char *)"help";
            args2.add( commandarg );
            args2.add( helpcmd );
            return runservcmd(cn, args[1], args2 );
        }
        string msg;
        formatstring( msg, "\f0#%s \f2usage: \f0#%s command", args[0], args[0] );
        sendf(cn, 1, "ris", N_SERVMSG, msg);
        return 0;
    }

    /*
        be the first to disable this command :o)
    */
    int srvcmd_test(int cn, vector<char *> args)
    {
        sendf(cn, 1, "ris", N_SERVMSG, "\f5you just wasted some time, thank you for testing me.");
        setenabledservcmd("test", false);
        return 0;
    }

    /*
        Adding item to forcements vector with status.
        *.cfg call example:
        fcanspawnitem SHELLS 1
    */
    int setfcanspawnitem(char *itemname, int status)
    {
            int itemnum = entities::entnum(itemname);
            if(-1 == itemnum)
            {
                return -1;
            }
            if( forcements.length() < itemnum )
            {
                for(int i=forcements.length();i<=itemnum;i++)
                {
                    forcements.add(false);
                }
            }
            forcements[itemnum] = (status);
            return itemnum;
    }
    COMMANDN(fcanspawnitem, setfcanspawnitem, "si");

    /*
        Enables/Disables spawning of several entities on any game mode.
    */
    int srvcmd_fcanspawnitem(int cn, vector<char *> args)
    {
        string msg;
        clientinfo *ci = getinfo(cn);

        if( 3 > args.length() || ( 2 <= args.length() && 0 == strcmp("help", args[1]) ) || ci->privilege < PRIV_MASTER )
        {
            if( 2 <= args.length() && 0 != strcmp("help", args[1]) )
            {
                if( 0 == strcmp("state", args[1]) )
                {
                    formatstring( msg, "\f1spawnstates: shells=\f7%i\f1, bullets=\f7%i\f1, rockets=\f7%i\f1, rounds=\f7%i\f1, grenades=\f7%i\f1, catridges=\f7%i\f1, health=\f7%i\f1, boost=\f7%i\f1, greenarmour=\f7%i\f1, yellowarmour=\f7%i\f1, quad=\f7%i\f1",
                        forcecanspawnitem(I_SHELLS),
                        forcecanspawnitem(I_BULLETS),
                        forcecanspawnitem(I_ROCKETS),
                        forcecanspawnitem(I_ROUNDS),
                        forcecanspawnitem(I_GRENADES),
                        forcecanspawnitem(I_CARTRIDGES),
                        forcecanspawnitem(I_HEALTH),
                        forcecanspawnitem(I_BOOST),
                        forcecanspawnitem(I_GREENARMOUR),
                        forcecanspawnitem(I_YELLOWARMOUR),
                        forcecanspawnitem(I_QUAD)
                    );
                    sendf(cn, 1, "ris", N_SERVMSG, msg);
                    return 0;
                }
                else
                {
                    int itemnum = entities::entnum(args[1]);
                    int status = forcements.inrange(itemnum) ? forcements[itemnum] : 0;
                    formatstring( msg, "\f6%s \f7spawn state is: \f1%i", args[1], status );
                    sendf(cn, 1, "ris", N_SERVMSG, msg);
                    return 0;
                }
            }
            formatstring( msg, "\f0#%s \f7takes up to two arguments: \f0item status\f7. Enable quad: \f0#%s quad 1\f7 . Disable yellow armour: \f0#%s yellowarmour 0\f7.", args[0], args[0], args[0] );
            sendf(cn, 1, "ris", N_SERVMSG, msg);
            formatstring( msg, "\f6valid items are:\f1 shells, bullets, rockets, rounds, grenades, catridges, health, boost, greenarmour, yellowarmour, quad" );
            sendf(cn, 1, "ris", N_SERVMSG, msg);
            formatstring( msg, "\f0#%s quad \f7 displays the state of quad spawn. \f0#%s state  \f7displays state of all options", args[0], args[0] );
            sendf(cn, 1, "ris", N_SERVMSG, msg);
            return 0;
        }
        else
        {
            int itemnum = setfcanspawnitem( args[1], ( 0 == strcmp("0" , args[2]) ) ? 0 : 1 );
            if(-1 == itemnum)
            {
                formatstring( msg, "\f3%s \f6is \f3not\f6 a valid item. Try one of: \f1shells, bullets, rockets, rounds, grenades, catridges, health, boost, greenarmour, yellowarmour, quad", args[1] );
                sendf(cn, 1, "ris", N_SERVMSG, msg);
                return 1;
            }
            loaditems();
            formatstring( msg, "\f6%s \f7spawning is now set to \f1%s", args[1], args[2] );
            sendf(cn, 1, "ris", N_SERVMSG, msg);
        }
        return 0;
    }

    /*
        Put commands and aliaes to the vectors
    */
    void installservcmds()
    {
        addservcmd("test", PRIV_NONE, true, srvcmd_test);
        addservcmdalias("test", "foo");

        addservcmd("resetmods", PRIV_ADMIN, true, srvcmd_resetmodifications);

        addservcmd("fcanspawnitem", PRIV_NONE, true, srvcmd_fcanspawnitem);

        addservcmd("help", PRIV_NONE, true, srvcmd_help);
        addservcmdalias("help", "commands");
    }
