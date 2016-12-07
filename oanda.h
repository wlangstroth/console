#ifndef OANDA_H
#define OANDA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

typedef struct PriceMap {
    const char *key;
    double value;
} price_map;

typedef struct CurlData {
    char *data;
    size_t size;
} curl_data;

struct json_object *curl(const char *);
int oanda_prices(price_map [], int);
double oanda_balance();

#endif
