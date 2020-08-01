#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool get_display(const char const *path, char *buffer) {
        struct dirent *dir;
        DIR *d = opendir(path);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_name[0] != '.') {
                    strcpy(buffer, dir->d_name);
                    closedir(d);
                    return true;
                }
            }
            closedir(d);
        }
        return false;
}

int get_number(const char const *path, const char const *name) {
    char file_name[128];
    strcpy(file_name, path);
    strcat(file_name, name);
    FILE *fp = fopen(file_name, "r");
    if (fp) {
        int result;
        fscanf(fp, "%d", &result);
        fclose(fp);
        return result;
    }
    return 0;
}

bool set_brightness(const char const *path, int brightness) {
    char file_name[128];
    strcpy(file_name, path);
    strcat(file_name, "brightness");
    FILE *fp = fopen(file_name, "w");
    if (!fp) {
        return false;
    }
    fprintf(fp, "%d", brightness);
    fclose(fp);
    return true;
}


int main(int argc, char *argv[]) {

    // basic path
    char path[128];
    strcpy(path, "/sys/class/backlight");

    // get display
    char d_name[64];
    if (!get_display(path, d_name)) {
        fprintf(stderr, "can't find display\n");
        return 1;
    }
    strcat(path, "/");
    strcat(path, d_name);
    strcat(path, "/");

    // get max_brightness
    int max_brightness = get_number(path, "max_brightness");
    if (!max_brightness) {
        fprintf(stderr, "can't load max_brightness\n");
        return 1;
    }

    // get actual_brightness
    int actual_brightness = get_number(path, "actual_brightness");
    if (!actual_brightness) {
        fprintf(stderr, "can't load actual_brightness\n");
        return 1;
    }

    // calculate percentage
    /* int current_percentage = (int)(100*actual_brightness/actual_brightness); */

    // calculate percentage with transformation
    int current_percentage = 1;
    while ((int)(((double)current_percentage*current_percentage)/10000*max_brightness) < actual_brightness) {
        current_percentage++;
    }

    if (argc == 1) { // return brightness

        // show percentage
        printf("%d\n", current_percentage);

    } else if (argc == 2) { // change brightness

        // get new percentage
        int percentage, brightness;
        if (argv[1][0] == '+') {
            percentage = current_percentage + atoi(argv[1]+1);
        } else if (argv[1][0] == '-') {
            percentage = current_percentage - atoi(argv[1]+1);
        } else {
            percentage = atoi(argv[1]);
        }

        /* // calculate new brightness */
        /* brightness = (int)(((double)percentage)/100*max_brightness); */

        // calculate new brightness with transformation
        brightness = (int)(((double)percentage*percentage)/10000*max_brightness);

        // normalize brightness
        if (brightness < 1) {
            brightness = 1;
        } else if (brightness > max_brightness) {
            brightness = max_brightness;
        }

        // set new brightness
        if (!set_brightness(path, brightness)) {
            fprintf(stderr, "can't set brightness\n");
            return 1;
        }
    }
    return 0;
}
