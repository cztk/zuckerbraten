    struct servcommand
    {
        char *name;
        int minprivilege;
        bool enabled;
        int (*run)(int, vector<char *>); // cn, arguments

    };

    struct servaliascommand
    {
        char *name;
        int commandid;
    };
