//
// Created by CHM on 2020/6/22.
//

#ifndef HEARTBEAT_TCP_MAKE_TELEGRAM_H
#define HEARTBEAT_TCP_MAKE_TELEGRAM_H

#include <stdlib.h>
#include <string>


int make_telegram(void *data, std::string &telegram_data, size_t *t_data_size);

int parse_telegram(std::string telegram_data, size_t t_data_size, void ** data);

#endif //HEARTBEAT_TCP_MAKE_TELEGRAM_H
