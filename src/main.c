#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ncurses.h>

int cpuload;

typedef struct line_ {
	char * character;
} line;

struct screen {
	line * line_;
};

struct winsize winsize_;
struct screen screen_;

void get_winsize() {
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize_);
}

int get_cpuload() {
	char str[100];
	char * token;
	FILE * fp = fopen("/proc/loadavg", "r");
	fgets(str, 100, fp);
	token = strtok(str, " ");
	fclose(fp);
	int load = atof(token) * 100 / sysconf(_SC_NPROCESSORS_ONLN);
	return load;
}

char * first_line (int width, int load, char * tuple) {
	char sload[2];
    
	sprintf(sload, "%d", load);
	
	for (int i = 0; i < width; i++) {
		tuple[i] = '-';
	}
	for (int i = width - 13; i < width - 3; i++) {
		tuple[i] = ' ';
	}

	tuple[width - 12] = sload[0];

	if (sload[1]) {
		tuple[width - 11] = sload[1];
	}

	tuple[width - 9] = '/';
	tuple[width - 7] = '1';
	tuple[width - 6] = '0';
	tuple[width - 5] = '0';
	tuple[0] = '|'; tuple[width - 1] = '|';
	return tuple;
}

char * last_line (int width, char * tuple) {

	for (int i = 0; i < width; i++) {
		if (!(i % 5) && i + 6 < width) {
			tuple[i] = '|';
		} else {
			tuple[i] = '-';
		}
	}

	tuple[width - 1] = '|';
	return tuple;
}

char * legend (int width, char * tuple) {

    for (int i = 0; i < width; i++) {
		tuple[i] = ' ';
	}
    
	tuple[width - 1] = 'o';
	tuple[width - 2] = 'g';
	tuple[width - 3] = 'a';
	tuple[width - 5] = '.';
	tuple[width - 6] = 's';
	return tuple;
}

void body_update (int width, int height, int step, int load, char * buffer) {
    
    int load_h = (height - 3) * load / 100;
    int counter = 0;
    
    for (int i = 0; i < height - 3; i++) {
        for (int j = 0; j < width; j++) {
            if (j == step) {
                buffer[counter] = ' ';
                if (i > (height - 3) - load_h) {
                    buffer[counter] = '#';
                    counter++;
                } else {
                    counter++;
                }
            } else {
                counter++;
            }
        }
    }
}

void body_populate (int width, int height, char * buffer) {
    
    int counter = 0;
    for (int i = 0; i < height - 3; i++) {
        for (int j = 0; j < width; j++) {
            buffer[counter] = ' ';
            counter++;
        }
    }
    
}

void push (int value, int size, int * pointer) {
    for (int i = 1; i < size; i++) {
        pointer[i-1] = pointer[i];
    }
    pointer[size] = value;
}

void draw (int width, int height) {
    
    int counter = 0;
    int * cpuloads = malloc(width * sizeof(int));
    
    char * tuple = malloc(width * sizeof(char));
    char * buffer = malloc((height - 3) * width * sizeof(char));
    
    body_populate(width, height, buffer);
    
    for (;;) {
        cpuloads[counter] = get_cpuload();
        
        first_line(width, get_cpuload(), tuple);
        for (int i = 0; i < width; i++) {
            printw("%c", tuple[i]);
        }
        
        if (counter < width) {
            body_update(width, height, counter, cpuloads[counter], buffer);
            for (int i = 0; i < width * (height - 3); i++) {
                printw("%c", buffer[i]);
            }
        } else {
            push(get_cpuload(), width, cpuloads);
            for (int i = 0; i < width; i++) {
                body_update(width, height, i, cpuloads[i], buffer);
            }
            for (int i = 0; i < width * (height - 3); i++) {
                printw("%c", buffer[i]);
            }
            counter--;
        }
        
        last_line(width, tuple);
        for (int i = 0; i < width; i++) {
            printw("%c", tuple[i]);
        }
	
        legend(width, tuple);
        for (int i = 0; i < width; i++) {
            printw("%c", tuple[i]);
        }
	
        counter++;
        refresh();
        sleep(1);
        clear();
    }
    
    /*
    first_line(width, get_cpuload(), tuple);
    for (int i = 0; i < width; i++) {
		printw("%c", tuple[i]);
	}
	
    body_update(width, height, 0, get_cpuload(), buffer);
    
    for (int i = 0; i < width * (height - 3); i++) {
		printw("%c", buffer[i]);
	}
	
	last_line(width, tuple);
	for (int i = 0; i < width; i++) {
        printw("%c", tuple[i]);
	}
	
	legend(width, tuple);
	for (int i = 0; i < width; i++) {
		printw("%c", tuple[i]);
	}
	
	free(tuple);
    refresh();
    */
}

int main (int argc, char **argv) {
	get_winsize();
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    draw(winsize_.ws_col, winsize_.ws_row);
    return 0;
}
