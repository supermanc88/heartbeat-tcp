//
// Created by CHM on 2020/6/22.
//

#ifndef HEARTBEAT_TCP_MAKE_TELEGRAM_H
#define HEARTBEAT_TCP_MAKE_TELEGRAM_H

#include <stdlib.h>


int make_telegram(void *data, void **telegram_data, size_t *t_data_size);

int parse_telegram(void * telegram_data, size_t t_data_size, void ** data);

#endif //HEARTBEAT_TCP_MAKE_TELEGRAM_H
