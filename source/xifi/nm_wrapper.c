#include "nm_wrapper.h"

bool nm_get_wifi_card(char *name) {
    char buffer[256];
    FILE *fp = popen("nmcli -t dev | grep wifi", "r");
    fgets(buffer, sizeof(buffer)-1, fp);
    fclose(fp);
    char *colon = strchr(buffer, ':');
    if (!colon) {
        return false;
    }
    (*colon) = '\0';
    strcpy(name, buffer);
    return true;
}

int nm_load_connections(connection *connections, int length) {
    // init variables
    FILE *fp;
    char connection_line[80];
    int connections_loaded = 0;
    char *start;
    char *next_colon;
    bool found = false;
    // call nmcli load available connections
    fp = popen("nmcli -t -c no -f SSID,SECURITY,RATE,SIGNAL,ACTIVE dev wifi list", "r");
    if (fp == NULL) {
        return -1;
    }
    // go trough lines of output from nmcli
    while ((fgets(connection_line, sizeof(connection_line)-1, fp) != NULL) && connections_loaded<length) {
        // parse ssid
        start = connection_line;
        next_colon = strchr(start, ':');
        (*next_colon) = '\0';
        strcpy(connections[connections_loaded].ssid, start);
        // parse security
        start = next_colon + 1;
        next_colon = strchr(start, ':');
        (*next_colon) = '\0';
        char *next_space = strchr(start, ' ');
        if (next_space != NULL) {
            (*next_space) = '\0';
        }
        strcpy(connections[connections_loaded].security, start);
        // parse rate
        start = next_colon + 1;
        next_colon = strchr(start, ':');
        (*next_colon) = '\0';
        connections[connections_loaded].rate = atoi(start);
        // parse signal
        start = next_colon + 1;
        next_colon = strchr(start, ':');
        (*next_colon) = '\0';
        connections[connections_loaded].signal = atoi(start);
        // set bools
        connections[connections_loaded].active = false;
        connections[connections_loaded].available = true;
        connections[connections_loaded].configured = false;
        // parse active
        start = next_colon + 1;
        if (strncmp(start, "no", 2) && strncmp(start, "nein", 4)) {
            connections[connections_loaded].active = true;
        }
        // increment connections_loaded
        connections_loaded++;
    }
    // call nmcli load configured connections
    fp = popen("nmcli -t -c no -f NAME connection", "r");
    if (fp == NULL) {
        return -1;
    }
    // go through lines of output from nmcli
    while (fgets(connection_line, sizeof(connection_line)-1, fp) != NULL) {
        // try to find connection
        found = false;
        (*strchr(connection_line, '\n')) = '\0';
        for (int i = 0; i < connections_loaded; i++) {
            if (strcmp(connection_line, connections[i].ssid) == 0) {
                connections[i].configured = true;
                found = true;
            }
        }
        // if connection is not available add new connection
        if (!found && connections_loaded<length) {
            strcpy(connections[connections_loaded].ssid, connection_line);
            strcpy(connections[connections_loaded].security, "-");
            connections[connections_loaded].rate = 0;
            connections[connections_loaded].signal = 0;
            connections[connections_loaded].active = false;
            connections[connections_loaded].available = false;
            connections[connections_loaded].configured = true;
            connections_loaded++;
        }
    }
    // return number of connections loaded
    return connections_loaded;
}

bool nm_enabled() {
    FILE *fp;
    char line[80];
    fp = popen("nmcli device | grep wifi", "r");
    fgets(line, sizeof(line)-1, fp);
    return strstr(line, "unmanaged") == NULL;
}

void nm_enable() {
    system("nmcli n on");
}

void nm_disable() {
    system("nmcli n off");
}

bool nm_connect(const connection *con) {
    FILE *fp;
    char line[80];
    char command[80];
    strcpy(command, "nmcli c up ");
    strcat(command, con->ssid);
    fp = popen(command, "r");
    fgets(line, sizeof(line)-1, fp);
    return strstr(line, "Error") == NULL;
}

bool nm_disconnect(const connection *con) {
    FILE *fp;
    char line[80];
    char command[80];
    strcpy(command, "nmcli c down ");
    strcat(command, con->ssid);
    fp = popen(command, "r");
    fgets(line, sizeof(line)-1, fp);
    return strstr(line, "Error") == NULL;
}

bool nm_add_connection(const char *ssid, const char *password) {
    FILE *fp;
    char line[80];
    char command[256];
    strcpy(command, "nmcli device wifi connect ");
    strcat(command, ssid);
    strcat(command, " password ");
    strcat(command, password);
    fp = popen(command, "r");
    fgets(line, sizeof(line)-1, fp);
    return strstr(line, "Error") == NULL;
}

bool nm_add_connection_eduroam(const char *ssid, const char *identity, const char *password) {
    FILE *fp;
    char line[80];
    char command[2048];
    char wifi_card[256];

    // get wifi card
    if (! nm_get_wifi_card(wifi_card)) {
        return false;
    }

    // build command
    strcpy(command, "nmcli connection add type wifi");
    // ifname
    strcat(command, " ifname ");
    strcat(command, wifi_card);
    // con-name
    strcat(command, " con-name ");
    strcat(command, ssid);
    // ssid
    strcat(command, " ssid ");
    strcat(command, ssid);
    // ipv4.method
    strcat(command, " ipv4.method auto");
    // wifi-sec.key-mgmt
    strcat(command, " wifi-sec.key-mgmt wpa-eap");
    // 802-1x.eap
    strcat(command, " 802-1x.eap peap");
    // 802-1x.phase2-auth
    strcat(command, " 802-1x.phase2-auth mschapv2");
    // 802-1x.identity
    strcat(command, " 802-1x.identity ");
    strcat(command, identity);
    // 802-1x.password
    strcat(command, " 802-1x.password ");
    strcat(command, password);
    // 802-1x.anonymous-identity
    strcat(command, " 802-1x.anonymous-identity eduroam@stud.uni-stuttgart.de");
    // 802-1x.ca-cert
    strcat(command, " 802-1x.ca-cert ");
    strcat(command, getenv("HOME"));
    strcat(command, "/.certs/ca.pem");

    // systemcall
    fp = popen(command, "r");
    fgets(line, sizeof(line)-1, fp);
    return strstr(line, "Error") == NULL;
}

bool nm_delete_connection(const connection *con) {
    FILE *fp;
    char line[80];
    char command[80];
    strcpy(command, "nmcli c delete ");
    strcat(command, con->ssid);
    fp = popen(command, "r");
    fgets(line, sizeof(line)-1, fp);
    return strstr(line, "Error") == NULL;
}
