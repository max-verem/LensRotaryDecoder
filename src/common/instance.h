#ifndef COMMON_INSTANCE_H
#define COMMON_INSTANCE_H

#define DEFAULT_INSTANCE_PARAMS         \
    /* affinity */                      \
    int cpu_affinity;                   \
                                        \
    /* realtime io */                   \
    int f_realtime_io;                  \
                                        \
    /* realtime cpu */                  \
    int realtime_cpu;                   \
                                        \
    /* default flags */                 \
    int* p_exit;                        \
    int f_debug;                        \
    int f_demonize;                     \
    int f_nodemonize;                   \
                                        \
    /* last config file */              \
    char last_opts_file[PATH_MAX];      \
                                        \
    /* logger */                        \
    char log_file_name[PATH_MAX];       \
    int log_rotate_interval;

#endif /* COMMON_INSTANCE_H */
