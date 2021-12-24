#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * Initialize logger module
 *
 * @param[in] log_file filename of log file where datas will written to
 * @param[in] rotate_interval value in seconds after log file will be renamed
 * with datestamp and new file created.
 *
 * @return 0 on success, negative value otherwise
 */
int logger_init(char* log_file, int rotate_interval);

/**
 * Release logger module
 *
 * @return 0 on success, negative value otherwise
 */
int logger_release();

/**
 * Output log message
 *
 * @param[in] type 0-NOTICE,1-ERROR,2-WARNING
 * @param[in] message message string formatted in printf style, if additional
 * argument required, these submitted throw the list
 *
 * @return 0 on success, negative value otherwise
 */
int logger_printf(int type, const char* message, ...)
#ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 2, 3)))
#endif
;


int logger_dup(int f);

#define LOG_MESSAGE(LEVEL, FMT, ...) \
    logger_message_send(LEVEL,  __FILE__, __LINE__, __FUNCTION__, FMT, ##__VA_ARGS__)

#define LOG_E(FMT, ...) LOG_MESSAGE( 0, FMT, ##__VA_ARGS__)
#define LOG_N(FMT, ...) LOG_MESSAGE(10, FMT, ##__VA_ARGS__)
#define LOG_D(FMT, ...) LOG_MESSAGE(20, FMT, ##__VA_ARGS__)
#define LOG_T(FMT, ...) LOG_MESSAGE(30, FMT, ##__VA_ARGS__)

#define LOG_NP          LOG_E("%s", "NOT IMPLEMENTED")
#define LOG_TD          LOG_T("%s", "TODO")
#define LOG_PL          LOG_E("%s", "");

void logger_message_level(int level);

void logger_message_send(int level, const char* file, const int line, const char* function, const char *fmt, ...)
#ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 5, 6)))
#endif
;


#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif /* LOGGER_H */


