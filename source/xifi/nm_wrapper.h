#ifndef NM_WRAPPER
#define NM_WRAPPER
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * a connection
 */
typedef struct connection {
    char ssid[32];
    char security[8];
    int rate;
    int signal;
    bool active;
    bool available;
    bool configured;
} connection;

/**
 * sets the name of the wifi card
 */
bool nm_get_wifi_card(char *name);
/**
 * loads all conections in buffer connections
 * returns number of loaded connections
 * returns -1 if an error occurs
 */
int nm_load_connections(connection *connections, int length);
/**
 * is NetworkmanagerEnabled
 */
bool nm_enabled();
/**
 * enables NetworkManager
 */
void nm_enable();
/**
 * disables NetworkManager
 */
void nm_disable();
/**
 * connects to network
 */
bool nm_connect(const connection *con);
/**
 * disconnects from network
 */
bool nm_disconnect(const connection *con);
/**
 * adds a connection
 */
bool nm_add_connection(const char *ssid, const char *password);
/**
 * adds a connection
 */
bool nm_add_connection_eduroam(const char *ssid, const char *identity, const char *password);
/**
 * deletes a connection
 */
bool nm_delete_connection(const connection *con);

#endif
