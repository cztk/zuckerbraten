    struct servcommand
    {
        const char *name;
        int minprivilege;
        bool enabled;
        int (*run)(int, vector<char *>); // cn, arguments

    };

    struct servaliascommand
    {
        const char *name;
        int commandid;
    };
