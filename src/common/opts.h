#define DEFAULT_INSTANCE_SET_OPTS                                               \
            case '6':   /* --log-file-name              */                      \
                ST_STR(log_file_name, PATH_MAX);                                \
            case '7':   /* --log-rotate-interval        */                      \
                ST_INT(log_rotate_interval);                                    \
                                                                                \
            case 'd':   /* --demonize                   */                      \
                ST_FLAG(f_demonize);                                            \
                                                                                \
            case 'n':   /* --nodemonize                 */                      \
                ST_FLAG(f_nodemonize);                                          \
                                                                                \
            case 250:   /* --realtime-io                */                      \
                ST_FLAG(f_realtime_io);                                         \
                                                                                \
            case 249:   /* --cpu-affinity               */                      \
                ST_INT(cpu_affinity);                                           \
                                                                                \
            case 248:   /* --debug                      */                      \
                instance->f_debug++; break;                                     \
                                                                                \
            case 247:   /* --realtime-cpu               */                      \
                ST_INT(realtime_cpu);                                           \
                                                                                \
            case 'h':                                                           \
                instance_usage(NULL);                                           \
                return -1;                                                      \
                break;                                                          \
                                                                                \
            default:                                                            \
                fprintf(stderr,                                                 \
                    "ERROR: Incorrect argument! [%s]\n", argv[optind - 1]);     \
                instance_usage(NULL);                                           \
                return -1;                                                      \
                break;

#define DEFAULT_INSTANCE_USAGE                                                  \
        "    --demonize                         Become daemon on executing\n"   \
        "\n"                                                                    \
        "                                       Log file operation\n"           \
        "    --log-file-name=<STR>\n"                                           \
        "    --log-rotate-interval=<INT>\n"                                     \
        "\n"                                                                    \
        "    -h, --help                         Show this help and usage\n"

#define DEFAULT_INSTANCE_LONG_OPTS                                              \
    { "log-file-name",                  required_argument, NULL, '6'},          \
    { "log-rotate-interval",            required_argument, NULL, '7'},          \
    { "demonize",                       no_argument,       NULL, 'd'},          \
    { "nodemonize",                     no_argument,       NULL, 'n'},          \
    { "help",                           no_argument,       NULL, 'h'},          \
    { "realtime-io",                    no_argument,       NULL, 250},          \
    { "cpu-affinity",                   required_argument, NULL, 249},          \
    { "debug",                          no_argument,       NULL, 248},          \
    { "realtime-cpu",                   required_argument, NULL, 247},          \
    { 0, 0, 0, 0}

/* program arguments processing */
#define ST_STR(VAR, MAX_LEN) strncpy(instance->VAR, optarg, MAX_LEN); break;
#define ST_INT(VAR) instance->VAR = atoi(optarg); break;
#define ST_FLAG(VAR) instance->VAR = 1; break;

#define REQ_STR(ARG, VAL) if(0 == instance->VAL[0]) req_arg = ARG; else
#define REQ_INT(ARG, VAL) if(0 == instance->VAL) req_arg = ARG; else

#define OPTS_FROM_FILE_CB(PROC) \
static int opts_from_file_cb(int argc, char** argv, void* app) \
{ \
    return PROC((instance_t*)app, argc, argv); \
}

#define OPTS_FROM_FILE_PROCESS(APP) \
    for(c = 0; optind < argc; optind++) \
    { \
        const char* f = argv[optind]; \
        if(!opts_from_file(f, APP, opts_from_file_cb)) \
            strlcpy(instance->last_opts_file, f, PATH_MAX); \
        else \
            c++; \
    };


static int opts_from_file(const char* filename, void* instance, int (*cb)(int, char**, void*))
{
    FILE *f;
    int args_count = 0, r = 0, l;
    char *buf;
    char** args_list;

    f = fopen(filename, "rt");
    if(!f)
    {
        fprintf(stderr, "%s: failed to open [%s]\n", __FUNCTION__, filename);
        return 1;
    };

    buf = (char*)malloc(PATH_MAX);
    args_list = (char**)malloc(sizeof(char*) * PATH_MAX);
    memset(args_list, 0, sizeof(char*) * PATH_MAX);

    args_list[0] = strdup(filename);
    args_count = 1;

    while(!feof(f))
    {
        char *c;

        buf[0] = 0;
        fgets(buf, PATH_MAX, f);

        /* trim from end */
        for(l = strlen(buf); l > 0;)
        {
            if(strchr("\n\t\r ", buf[l - 1]))
            {
                buf[l - 1] = 0;
                l--;
            }
            else
                break;
        };

        /* trim from start */
        for(c = buf; c[0];)
        {
            if(strchr("\n\t\r ", c[0]))
                c++;
            else
                break;
        };

        /* non-empty */
        if(!c[0] || c[0] == '#')
            continue;

        args_list[args_count++] = strdup(c);
    };

    free(buf);

    if(args_count > 1)
        r = cb(args_count, args_list, instance);

    for(l = 0; l < args_count; l++)
        free(args_list[l]);
    free(args_list);

    fclose(f);

    return r;
};
