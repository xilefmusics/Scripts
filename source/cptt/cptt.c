#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/16color.h"

#define BUFFER_LENGTH 512
char *chords[2][12] = {
    {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"},
    {"A", "Bb", "Cb", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab"}
};
typedef enum chord_preference {SHARP, FLAT, DYNAMIC} chordp_t;

chordp_t get_chordp(int level) {
    return level == 1 || level == 4 || level == 6 || level == 8 || level == 9 || level == 11;
}

char parse_whitespaces(FILE *in) {
    char c;
    do {
        c = getc(in);
    } while (c == ' ' || c == '\t');
    return c;
}

char parse(FILE *in, char *buffer, int buff_len, char c, char until, char *error, int *line)  {
    int buffer_index = 0;
    c = c ? c : getc(in);
    while (c != EOF) {
        if (c == until) {
            if (buffer) {
                buffer[buffer_index] = '\0';
            }
            return until;
        }
        for (int i = 0; i < strlen(error); ++i) {
            if (c == error[i]) {
                fprintf(stderr, "%d: found %c while parsing until %c\n", *line, error[i], until);
                exit(EXIT_FAILURE);
            }
        }
        if (buffer) {
            if (buffer_index >= buff_len - 1) {
                fprintf(stderr, "%d: buffer to small while parsing until %c\n", *line, until);
            }
            buffer[buffer_index++] = c;
        }
        c = getc(in);
    }
    fprintf(stderr, "%d: found EOF while parsing until %c\n", *line, until);
    exit(EXIT_FAILURE);
}

char parse_chord_level(FILE *in, char c, int *level, int *line) {
    c = c ? c : getc(in);
    switch (c) {
        case 'A': *level = 0;
                  break;
        case 'B': *level = 2;
                  break;
        case 'C': *level = 3;
                  break;
        case 'D': *level = 5;
                  break;
        case 'E': *level = 7;
                  break;
        case 'F': *level = 8;
                  break;
        case 'G': *level = 10;
                  break;
        default:
            fprintf(stderr, "%d: found %c while parsing chord_level\n", *line, c);
            exit(EXIT_FAILURE);
    }
    c = getc(in);
    if (c == '#') {
        *level = (*level + 1) % 12;
        c = getc(in);
    } else if (c == 'b') {
        *level = (*level - 1) % 12;
        c = getc(in);
    }
    return c;
}


void parse_to_plaintext(FILE *in, FILE *out, int transpose, chordp_t chordp, bool show_chords, bool show_sections, bool colors, char *prefix, int text_color, int chord_color, int section_color) {
    char c;
    int line_index = 1;
    char text_buffer[BUFFER_LENGTH];
    char chord_buffer[BUFFER_LENGTH];
    char directive_buffer[BUFFER_LENGTH];
    int text_index = 0;
    int directive_index = 0;
    int chord_index = 0;
    int chord_level = -1;
    bool first_section = true;

    while ((c = getc(in)) != EOF) {

        // skip empty lines
        if (c == 13) {
            continue;
        }

        // parse directive
        if (c == '{') {
            parse(in, directive_buffer, BUFFER_LENGTH, 0, ':', "\n}", &line_index);
            if (!strcmp(directive_buffer, "meta")) {
                parse(in, directive_buffer, BUFFER_LENGTH, parse_whitespaces(in), ' ', "\n}", &line_index);
                if (!strcmp(directive_buffer, "section") && show_sections) {
                    parse(in, directive_buffer, BUFFER_LENGTH, parse_whitespaces(in), '}', "\n", &line_index);
                    if (first_section) {
                        first_section = false;
                    } else {
                        printf("\n");
                    }
                    if (colors) {
                        fprintf(out, "%s%s%s\n", CCOLORS[section_color], directive_buffer, CRESET);
                    } else {
                        fprintf(out, "%s\n", directive_buffer);
                    }
                }
            } else if (chordp == DYNAMIC && !strcmp(directive_buffer, "key")) {
                if (parse_chord_level(in, parse_whitespaces(in), &chord_level, &line_index) == 'm') {
                    chord_level += 3;
                }
                chordp = get_chordp((chord_level + transpose) % 12);
            }
            parse(in, NULL, 0, 0, '\n', "", &line_index);
            line_index++;
            continue;
        }

        // parse line
        do {
            // finish line
            if (c == '\n') {
                if (show_chords && chord_index > 0) {
                    chord_buffer[chord_index] = '\0';
                    if (colors) {
                        fprintf(out, "%s%s%s%s\n", CCOLORS[chord_color], prefix, chord_buffer, CRESET);
                    } else {
                        fprintf(out, "%s%s\n", prefix, chord_buffer);
                    }
                    chord_index = 0;
                }
                if (text_index > 0) {
                    text_buffer[text_index] = '\0';
                    if (colors) {
                        fprintf(out, "%s%s%s%s\n", CCOLORS[text_color], prefix, text_buffer, CRESET);
                    } else {
                        fprintf(out, "%s%s\n", prefix, text_buffer);
                    }
                    text_index = 0;
                }
                ++line_index;
                break;
            }

            // parse chord
            if (c == '[' && show_chords) {
                // fill in spaces
                while (chord_index < text_index) {
                    chord_buffer[chord_index++] = ' ';
                }
                // parse chord
                while ((c = getc(in)) != EOF) {
                    // parse to transpose
                    if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' || c == 'G') {
                       c = parse_chord_level(in, c, &chord_level, &line_index);
                        chord_level = (chord_level + transpose) % 12;
                        if (chord_index >= BUFFER_LENGTH-2) {
                            fprintf(stderr, "%d: chord_buffer to small\n", line_index);
                            exit(EXIT_FAILURE);
                        }
                        if (chordp == DYNAMIC) {
                            fprintf(stderr, "%d: no key spezieied\n", line_index);
                            exit(EXIT_FAILURE);
                        }
                        chord_buffer[chord_index++] = chords[chordp][chord_level][0];
                        if (strlen(chords[chordp][chord_level]) > 1) {
                            chord_buffer[chord_index++] = chords[chordp][chord_level][1];
                        }
                        chord_level = -1;
                    }
                    // finish chord
                    if (c == ']') {
                        break;
                    }
                    // add not to transpose
                    if (chord_index >= BUFFER_LENGTH-1) {
                        fprintf(stderr, "%d: chord_buffer to small\n", line_index);
                        exit(EXIT_FAILURE);
                    }
                    chord_buffer[chord_index++] = c;
                }
                continue;
            } else if (c == '[') {
                while ((c = getc(in)) != EOF) {
                    if (c == ']') {
                        break;
                    }
                }
                continue;
            }

            // parse normal character
            if (text_index >= BUFFER_LENGTH-1) {
                fprintf(stderr, "%d: text_buffer to small\n", line_index);
                exit(EXIT_FAILURE);
            }
            if (c != 13) {
                text_buffer[text_index++] = c;
            }

        } while ((c = getc(in)) != EOF);
    }
}

int main(int argc, char *argv[]) {
    FILE *in = stdin, *out = stdout;
    chordp_t chordp = DYNAMIC;
    char *prefix ="  ";
    int transpose = 0, capo = 0, inindex = 0, outindex = 0, text_color = 7, chord_color = 7, section_color = 7;
    bool show_chords = true, show_sections = true, colors = false;
    // parse Arguments
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-nc")) {
            show_chords = false;
        } else if (!strcmp(argv[i], "-ns")) {
            show_sections = false;
        } else if (!strcmp(argv[i], "-ni")) {
            prefix = "";
        } else if (!strcmp(argv[i], "-t")) {
            if (i+1 >= argc) {
                fprintf(stderr, "-t needs argument <INTEGER>\n");
                exit(EXIT_FAILURE);
            }
            transpose = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-c")) {
            if (i+1 >= argc) {
                fprintf(stderr, "-c needs argument <INTEGER>\n");
                exit(EXIT_FAILURE);
            }
            capo = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-o")) {
            if (i+1 >= argc) {
                fprintf(stderr, "-o needs argument <STRING>\n");
                exit(EXIT_FAILURE);
            }
            outindex = ++i;
        } else if (!strcmp(argv[i], "-cp")) {
            if (i+1 >= argc) {
                fprintf(stderr, "-cp needs argument (flat or sharp)\n");
                exit(EXIT_FAILURE);
            }
            if (!strcmp(argv[++i], "flat")) {
               chordp = FLAT;
            } else if (!strcmp(argv[i], "sharp")) {
               chordp = SHARP;
            } else {
                fprintf(stderr, "-cp needs argument (flat or sharp)\n");
                exit(EXIT_FAILURE);
            }
        } else if (!strcmp(argv[i], "-Ct")) {
            if (i+1 >= argc) {
                fprintf(stderr, "-c needs argument <INTEGER>\n");
                exit(EXIT_FAILURE);
            }
            text_color = atoi(argv[++i]);
            text_color = 0 <= text_color && text_color < 16 ? text_color: 7;
            colors = true;
        } else if (!strcmp(argv[i], "-Cc")) {
            colors = true;
            if (i+1 >= argc) {
                fprintf(stderr, "-c needs argument <INTEGER>\n");
                exit(EXIT_FAILURE);
            }
            chord_color = atoi(argv[++i]);
            chord_color = 0 <= chord_color && chord_color < 16 ? chord_color: 7;
            colors = true;
        } else if (!strcmp(argv[i], "-Cs")) {
            colors = true;
            if (i+1 >= argc) {
                fprintf(stderr, "-c needs argument <INTEGER>\n");
                exit(EXIT_FAILURE);
            }
            section_color = atoi(argv[++i]);
            section_color = 0 <= section_color && section_color < 16 ? section_color: 7;
            colors = true;
        } else {
            inindex = i;
        }
    }

    // open files
    if (inindex) {
        if (!(in = fopen(argv[inindex], "r"))){
            fprintf(stderr, "can't open file: %s\n", argv[inindex]);
            exit(EXIT_FAILURE);
        }
    }
    if (outindex) {
        if (!(out = fopen(argv[outindex], "w"))){
            fprintf(stderr, "can't open file: %s\n", argv[outindex]);
            exit(EXIT_FAILURE);
        }
    }

    // parse
    parse_to_plaintext(in, out, transpose-capo, chordp, show_chords, show_sections, colors, prefix, text_color, chord_color, section_color);

    // close files
    if (inindex) {
        fclose(in);
    }
    if (outindex) {
        fclose(out);
    }

    return 0;
}
