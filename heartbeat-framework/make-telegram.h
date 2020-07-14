//
// Created by CHM on 2020/6/22.
//

#ifndef HEARTBEAT_TCP_MAKE_TELEGRAM_H
#define HEARTBEAT_TCP_MAKE_TELEGRAM_H

#include <stdlib.h>
#include <string>

/**
 * 序列化数据
 * @param data      要发送的数据结构体
 * @param telegram_data     序列化的string 结果
 * @param t_data_size       序列化string的长度
 * @return
 */
int make_telegram(void *data, std::string &telegram_data, size_t *t_data_size);

/**
 * 反序列化数据
 * @param telegram_data     收到的序列化的string
 * @param t_data_size       收到的序列化的string的长度
 * @param data              反序列化的结果，数据结构体
 * @return
 */
int parse_telegram(std::string telegram_data, size_t t_data_size, void **data);

#endif //HEARTBEAT_TCP_MAKE_TELEGRAM_H
