#include "nadeshiko.h"

#include <webview.h>

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Server Stuff
**/

static enum MHD_Result
answer_to_connection (void *cls, struct MHD_Connection *connection,
            const char *url, const char *method,
            const char *version, const char *upload_data,
            size_t *upload_data_size, void **con_cls)
{
    struct MHD_Response *response;
    int fd;
    enum MHD_Result ret;
    struct stat sbuf;
    (void) cls;               /* Unused. Silent compiler warning. */
    // (void) url;               /* Unused. Silent compiler warning. */
    (void) version;           /* Unused. Silent compiler warning. */
    (void) upload_data;       /* Unused. Silent compiler warning. */
    (void) upload_data_size;  /* Unused. Silent compiler warning. */
    (void) con_cls;           /* Unused. Silent compiler warning. */

    char* filename = strdup(ROOT_PATH);
    strcat(filename, url);

    if (0 != strcmp (method, "GET"))
        return MHD_NO;

    if ( (-1 == (fd = open (filename, O_RDONLY))) ||
        (0 != fstat (fd, &sbuf)) )
    {
        free(filename);
        const char *errorstr =
        "<html><body>404</body></html>";
        /* error accessing file */
        if (fd != -1)
        (void) close (fd);
        response =
        MHD_create_response_from_buffer (strlen (errorstr),
                                        (void *) errorstr,
                                        MHD_RESPMEM_PERSISTENT);
        if (NULL != response)
        {
        ret =
            MHD_queue_response (connection, MHD_HTTP_INTERNAL_SERVER_ERROR,
                                response);
        MHD_destroy_response (response);

        return ret;
        }
        else
        return MHD_NO;
    }
    free(filename);
    response =
        MHD_create_response_from_fd_at_offset64 (sbuf.st_size, fd, 0);
    // MHD_add_response_header (response, "Content-Type", MIMETYPE);
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);

    return ret;
}


void nadeshiko_init() {
    server_data.daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                                &answer_to_connection, NULL, MHD_OPTION_END);
}

/**
 * Nadeshiko Stuff
**/

Nadeshiko* nadeshiko_create_window(const char* p_title, const char* p_entry_point, int p_width, int p_height) {
    Nadeshiko* window = (Nadeshiko *)malloc(sizeof(Nadeshiko));
    
    window->url = (char *)malloc(sizeof(char) * (strlen(ROOT_URL) + strlen(p_entry_point) + 1));
    // dont really have to multiply by sizeof char but eh...
    strcpy(window->url, ROOT_URL);
    window->title = strdup(p_title);
    window->entry_point = strdup(p_entry_point);
    window->width = p_width;
    window->height = p_height;

    strcat(window->url, window->entry_point);

    window->w = webview_create(0, NULL);
    webview_set_title(window->w, window->title);
    webview_set_size(window->w, window->width, window->height, WEBVIEW_HINT_NONE);
    webview_navigate(window->w, window->url);
    
    return window;
}

void nadeshiko_set_min_window_size(Nadeshiko* nadeshiko, int p_width, int p_height) {
    webview_set_size(nadeshiko->w, p_width, p_height, WEBVIEW_HINT_MIN);
}

void nadeshiko_set_max_window_size(Nadeshiko* nadeshiko, int p_width, int p_height) {
    webview_set_size(nadeshiko->w, p_width, p_height, WEBVIEW_HINT_MAX);
}

void nadeshiko_run(Nadeshiko* nadeshiko) {
    webview_run(nadeshiko->w);
}

void nadeshiko_destroy(Nadeshiko* nadeshiko) {
    free(nadeshiko->url);
    free(nadeshiko->title);
    free(nadeshiko->entry_point);
    webview_destroy(nadeshiko->w); // should i also free w?
    free(nadeshiko);
}






typedef struct {
  Nadeshiko* instance;
  unsigned int count;
} context_t;

void increment(const char *seq, const char *req, void *arg) {
  (void)req;
  context_t *context = (context_t *)arg;
  char count_string[10] = {0};
  sprintf(count_string, "%u", ++context->count);
  char result[21] = {0};
  strcat(result, "{\"count\": ");
  strcat(result, count_string);
  strcat(result, "}");
  nadeshiko_return(context->instance, seq, 0, result);
}

int main() {
    nadeshiko_init();

    Nadeshiko* instance = (Nadeshiko *)malloc(sizeof(Nadeshiko));
    instance = nadeshiko_create_window(
        "Test",
        "index.html",
        500,
        500
    );

    context_t context = {.instance = instance, .count = 0};
    nadeshiko_bind(instance, "increment", increment, &context);

    nadeshiko_run(instance);
    nadeshiko_destroy(instance);

    nadeshiko_stop();

    return 0;
}