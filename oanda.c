#include "oanda.h"
#include "secrets.h"

// See https://curl.haxx.se/libcurl/c/getinmemory.html
static size_t
http_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct CurlData *mem = (struct CurlData *)userp;

    mem->data = realloc(mem->data, mem->size + realsize + 1);
    if (mem->data == NULL)
    {
	printf("not enough data (realloc returned NULL)\n");
	return 0;
    }

    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

struct json_object *
curl(const char *url)
{
    CURL *curl_handle;
    char auth_header[100];
    struct json_object *result = NULL;

    if (snprintf(auth_header, 100, "Authorization: Bearer %s", access_token) >= 100)
    {
	printf("Auth header overflow.");
	exit(EXIT_FAILURE);
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    CURLcode response_code;
    struct curl_slist *slist = NULL;
    struct CurlData chunk;

    chunk.data = malloc(1);
    chunk.size = 0;

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, http_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "console/1.0");

    slist = curl_slist_append(slist, auth_header);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);

    response_code = curl_easy_perform(curl_handle);

    if (response_code)
    {
	fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(response_code));
	exit(EXIT_FAILURE);
    }
    else
    {
	result = json_tokener_parse(chunk.data);
    }

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(slist);
    free(chunk.data);
    curl_global_cleanup();

    return result;
}

int
oanda_prices(price_map sparkline_prices[], int count)
{
    char *url =
	"https://api-fxtrade.oanda.com/v1/prices?instruments=EUR_USD%2CGBP_USD%2CUSD_JPY%2CUSD_CAD%2CUSD_CHF%2CSPX500_USD%2CXCU_USD%2CUSB30Y_USD%2CSOYBN_USD%2CNATGAS_USD";
    struct json_object *parse_result = curl(url);
    if (parse_result) {
	json_object_object_foreach(parse_result, key, val) {
	    json_object *arr = NULL;
	    json_object *price_object = NULL;
	    json_object *instrument = NULL;
	    json_object *price = NULL;
	    json_object_object_get_ex(parse_result, key, &arr);
	    // int arrlen = json_object_array_length(arr);

	    for (int i = 0; i < count; i++) {
		price_object = json_object_array_get_idx(arr, i);
		json_object_object_get_ex(price_object, "instrument", &instrument);

		json_object_object_get_ex(price_object, "bid", &price);
		sparkline_prices[i].key = json_object_get_string(instrument);
		sparkline_prices[i].value = json_object_get_double(price);
	    }
	}
    }
    else
    {
	for (int i = 0; i < count; i++) {
	    sparkline_prices[i].key = "INSTRUMENT";
	    sparkline_prices[i].value = 0;
	}
    }
    return 0;
}

double
oanda_balance()
{
    double account_balance = 0.0;
    char url[51];
    if (snprintf(url, 51, "https://api-fxtrade.oanda.com/v1/accounts/%s", account_id) >= 51)
    {
	fprintf(stdout, "URL overflow.");
	exit(EXIT_FAILURE);
    }

    json_object *balance_obj = NULL;
    struct json_object *parse_result = curl(url);
    if (parse_result)
    {
	json_object_object_get_ex(parse_result, "balance", &balance_obj);
	account_balance = json_object_get_double(balance_obj);
    }
    return account_balance;
}
