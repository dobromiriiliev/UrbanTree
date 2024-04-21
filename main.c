#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef struct {
    char* data;
    size_t length;
} Buffer;

size_t write_response(void* ptr, size_t size, size_t nmemb, void* stream) {
    size_t real_size = size * nmemb;
    Buffer* buffer = (Buffer*)stream;

    char* ptr_new = realloc(buffer->data, buffer->length + real_size + 1);
    if (ptr_new == NULL) return 0;  // Out of memory!

    buffer->data = ptr_new;
    memcpy(&(buffer->data[buffer->length]), ptr, real_size);
    buffer->length += real_size;
    buffer->data[buffer->length] = 0;

    return real_size;
}

void geocode_address(const char* address, const char* api_key) {
    CURL* curl;
    CURLcode res;
    Buffer buffer;

    buffer.data = malloc(1);  // Will be grown as needed by realloc
    buffer.length = 0;        // No data at this point

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        char* url_encoded_address = curl_easy_escape(curl, address, 0);
        if(url_encoded_address) {
            char url[512];
            sprintf(url, "https://maps.googleapis.com/maps/api/geocode/json?address=%s&key=%s", url_encoded_address, api_key);
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                printf("Response: %s\n", buffer.data);
                cJSON* root = cJSON_Parse(buffer.data);
                cJSON* results = cJSON_GetObjectItem(root, "results");
                cJSON* first_result = cJSON_GetArrayItem(results, 0);
                cJSON* geometry = cJSON_GetObjectItem(first_result, "geometry");
                cJSON* location = cJSON_GetObjectItem(geometry, "location");
                cJSON* lat = cJSON_GetObjectItem(location, "lat");
                cJSON* lng = cJSON_GetObjectItem(location, "lng");

                printf("Latitude: %f\n", lat->valuedouble);
                printf("Longitude: %f\n", lng->valuedouble);

                cJSON_Delete(root);
            }

            curl_free(url_encoded_address);
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    free(buffer.data);
}

int main(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <address> <API key>\n", argv[0]);
        return 1;
    }

    geocode_address(argv[1], argv[2]);

    return 0;
}
