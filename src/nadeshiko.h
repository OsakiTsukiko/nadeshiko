/**
 * Nadeshiko
 * A library born from my need of a fast to develop ui library (i dislike gtk and imgui needs too much boilerplate) 
 * (mainly for giving a ui to my university homeworks...) thus its not very modular..
 * 
 * for not the scope is small and i'm too lazy + don't have much time to implement stuff like multiple windows, or 
 * ability to send packets from server to client (without which multiple windows would be kinda useless), still, i'll try
 * to make the design so that they could be implemented in the future witout much hussle
 * 
 * - Osaki
**/

/**
 * TODO: 
 * - ADD COMMENTS 
 * - make docs
 * - build system (prob cmake)
 * - multiple windows? (look into window return)
**/

#ifndef NADESHIKO_H
#define NADESHIKO_H

#include <webview.h>
#include <microhttpd.h>
#include <stdlib.h>

#define STR(VAR) #VAR
#define VAR_TO_STRING(VAR) STR(VAR)
// macro witchcraft

#define PORT 25160
// #define ROOT_PATH "./web"
#define ROOT_URL "http://127.0.0.1:" VAR_TO_STRING(PORT) "/"
#define MIMETYPE "none" // let the browser figure it out cuz im lazy

typedef struct {
    char* url;
    char* title;
    char* entry_point;
    int width;
    int height;
    webview_t w;
} Nadeshiko;

typedef struct {
    struct MHD_Daemon *daemon;
} WebServerData;

WebServerData server_data; // variable definition in headers :skull:
char *root_path = NULL;

/**
 * Server Stuff
**/

void nadeshiko_init(const char *p_root_path);
void nadeshiko_stop() {MHD_stop_daemon(server_data.daemon); free(root_path);}

/**
 * Nadeshiko Stuff
**/

Nadeshiko* nadeshiko_create_window(const char* p_title, const char* p_entry_point, int p_width, int p_height);

void nadeshiko_set_min_window_size(Nadeshiko* nadeshiko, int p_width, int p_height);
void nadeshiko_set_max_window_size(Nadeshiko* nadeshiko, int p_width, int p_height);

// just wrappers for convenience
void nadeshiko_bind(
    Nadeshiko* nadeshiko, 
    const char* p_name,
    void (*fn)(const char *seq, const char *req, void *arg),
    void *arg
) { webview_bind(nadeshiko->w, p_name, fn, arg); }

void nadeshiko_unbind(Nadeshiko* nadeshiko, const char *name) {
    webview_unbind(nadeshiko->w, name);
};

void nadeshiko_return(
    Nadeshiko* nadeshiko, 
    const char *seq, 
    int status,
    const char *result
) {
    webview_return(nadeshiko->w, seq, status, result);
}

void nadeshiko_run(Nadeshiko* nadeshiko);
void nadeshiko_destroy(Nadeshiko* nadeshiko);

#endif