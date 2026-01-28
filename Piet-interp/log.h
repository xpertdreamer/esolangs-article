//
// Created by IWOFLEUR on 28.01.2026.
//

#ifndef LOG_H
#define LOG_H

#define PIET_LOG_NONE 0
#define PIET_LOG_INFO 1
#define PIET_LOG_DEBUG 2
#define PIET_LOG_TRACE 3

extern int piet_log_level;

#define PIET_INFO(...) if (piet_log_level >= PIET_LOG_INFO) fprintf(stderr, "[PIET] " __VA_ARGS__)
#define PIET_DEBUG(...) if (piet_log_level >= PIET_LOG_DEBUG) fprintf(stderr, "[PIET-DBG] " __VA_ARGS__)
#define PIET_TRACE(...) if (piet_log_level >= PIET_LOG_TRACE) fprintf(stderr, "[PIET-TRC] " __VA_ARGS__)
#define PIET_ERROR(...) do { fprintf(stderr, "[PIET-ERR] " __VA_ARGS__); exit(EXIT_FAILURE); } while(0)

#endif //LOG_H
