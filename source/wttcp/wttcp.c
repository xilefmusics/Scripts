#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_LENGTH 512

int NUM_OF_KEYWORDS = 19;
char *KEYWORDS[] = {
    "Intro",
    "Verse",
    "Verse 1",
    "Verse 2",
    "Verse 3",
    "Verse 4",
    "Verse 5",
    "Verse 6",
    "Verse 7",
    "Verse 8",
    "Verse 9",
    "Verse 10",
    "Pre-Chorus",
    "Chorus",
    "Chorus 1",
    "Chorus 2",
    "Bridge 1",
    "Bridge 2",
    "Bridge"
};

int parse(FILE *in, FILE *out, char *buffer, int buffer_length, char *c, char *until, int *line) {
    int buffer_index = 0, html_index = -1;
    while (*c != EOF) {
        if (*c == '\n') {
            ++(*line);
        }
        // goal check
        for (int i = 0; i < strlen(until); ++i) {
            if (*c == until[i]) {
                if (buffer) {
                    if (buffer_index < buffer_length) {
                        buffer[buffer_index] = '\0';
                    } else {
                        return -2;
                    }
                }
                return 0;
            }
        }
        // write to out
        if (out) { putc(*c, out);
        }
        if (buffer) {
            if (buffer_index < buffer_length) {
                buffer[buffer_index++] = *c;
            } else {
                return -2;
            }
            // html support
            if (*c == '&') {
                html_index = buffer_index - 1;
            } else if (html_index >= 0 && *c == ';') {
                if (buffer_index < buffer_length) {
                    buffer[buffer_index++] = '\0';
                    if (buffer[html_index + 1] == '#') {
                        buffer[buffer_index -2] = '\0';
                        buffer[html_index] = (char) atoi(buffer+html_index+2);
                        buffer_index = html_index + 1;
                        html_index = -1;
                    } else {
                        // TODO html_entitys
                    }
                } else {
                    return -2;
                }
            }
        }
        *c = getc(in);
    }
    return -1;
}

int parse_until_tag(FILE *in, char *buffer, int buffer_length, char *c, int *line) {
    int error;
    char class_buffer[6];

    // find beginning
    if (error = parse(in, NULL, NULL, 0, c, "<", line)) { return error; };
    *c = getc(in);

    // parse tag
    if (error = parse(in, NULL, buffer, buffer_length, c, " >", line)) { return error; };
    if (*c == '>') { return 0; };

    // find class attribut
    while (1) {
        if (error = parse(in, NULL, NULL, 0, c, "c>", line)) { return error; };
        if (*c == '>') { return 0; };
        if (error = parse(in, NULL, class_buffer, 6, c, " =>", line)) { if (error == -2) { continue; } else if (error != 0) { return error; } };
        if (*c == '>') { return 0; };
        if (strcmp(class_buffer, "class")) { continue; };
        if (error = parse(in, NULL, NULL, 0, c, "\"\'>", line)) { return error; };
        if (*c == '>') { return 0; };
        *c = getc(in);
        buffer_length -= strlen(buffer) - 1;
        buffer = strchr(buffer, '\0');
        buffer[0] = '.';
        ++buffer;
        if (error = parse(in, NULL, buffer, buffer_length, c, "\"\'>", line)) { return error; };
        if (*c == '>') { return 0; };
        return 0;
    }
}

void parse_to_chord_pro(FILE *in, FILE *out) {
    char c = getc(in);
    int line_index = 1;
    int error;
    char buffer[BUFFER_LENGTH];
    char line_buffer[BUFFER_LENGTH];
    int line_buffer_index = 0;
    bool is_keyword = false;

    // parse title
    do {
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
    } while (!error && strcmp(buffer, "h2.t-song-details__marquee__headline"));
    c = getc(in);
    c = getc(in);
    fputs("{title: ", out);
    error = parse(in, out, NULL, 0, &c, "<", &line_index);
    fputs("}\n", out);

    // parse artist
    do {
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
    } while (!error && strcmp(buffer, "p.large"));
    error = parse(in, NULL, NULL, 0, &c, "<", &line_index);
    error = parse(in, NULL, NULL, 0, &c, ">", &line_index);
    c = getc(in);
    fputs("{artist: ", out);
    error = parse(in, out, NULL, 0, &c, "<", &line_index);
    fputs("}\n", out);




    // find div.chordProContainer
    do {
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
    } while (!error && strcmp(buffer, "div.chordProContainer"));
    if (error) { fprintf(stderr, "%d: couldn't find div.chordProContainer buffer=%s\n", line_index, buffer); exit(EXIT_FAILURE); }

    // parse key
    do {
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
    } while (!error && strcmp(buffer, "select.select select--inline"));
    error = parse(in, NULL, NULL, 0, &c, "<", &line_index);
    error = parse(in, NULL, NULL, 0, &c, "\"", &line_index);
    c = getc(in);
    fputs("{key: ", out);
    error = parse(in, out, NULL, 0, &c, "\"", &line_index);
    fputs("}\n\n", out);

    // find first div.chord-pro-line
    do {
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
    } while (!error && strcmp(buffer, "div.chord-pro-line"));

    // parse all lines
    while (!error && !strcmp(buffer, "div.chord-pro-line")) {

        // parse div.chord-pro-segment
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);

        // parse all segments of line
        while (!error && !strcmp(buffer, "div.chord-pro-segment")) {
            // parse div.chord-pro-note
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
            c = getc(in);
            c = getc(in);
            if (c != '<') {
                putc('[', out);
                error = parse(in, out, NULL, 0, &c, "<", &line_index);
                putc(']', out);
            }
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);

            // parse div.chord-pro-lyric
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
            c = getc(in);
            c = getc(in);
            error = parse(in, NULL, buffer, BUFFER_LENGTH, &c, "<", &line_index);
            for (int i = 0; i < NUM_OF_KEYWORDS; ++i) {
                if (!strcmp(KEYWORDS[i], buffer)) {
                    is_keyword = true;
                }
            }
            if (is_keyword) {
                fputs("{meta: section ", out);
                fputs(buffer, out);
                putc('}', out);
                is_keyword = false;
            } else {
                fputs(buffer, out);
            }
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);

            // parse closing div.chord-pro-segment
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);

            // parse next (closing div.chord-pro-line or new div.chord-pro-segment)
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
        }

        // do linebreak
        putc('\n', out);

        // parse next (closing div.chordProContainer or new div.chord-pro-line)
        error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
        while (!error && !strcmp(buffer, "div.chord-pro-br")) {
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
            error = parse_until_tag(in, buffer, BUFFER_LENGTH, &c, &line_index);
        }

    }
}

int main(int argc, char *argv[]) {
    parse_to_chord_pro(stdin, stdout);
    return 0;
}
