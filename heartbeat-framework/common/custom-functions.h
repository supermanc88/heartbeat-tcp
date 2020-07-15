#ifndef HEARTBEAT_TCP_CUSTOM_FUNCTIONS_H
#define HEARTBEAT_TCP_CUSTOM_FUNCTIONS_H

/**
 * 将system的执行结果写到文件中
 * @param cmd_string
 * @param file_path
 * @return
 */
int system_to_file(const char *cmd_string, const char *file_path);

#endif //HEARTBEAT_TCP_CUSTOM_FUNCTIONS_H
