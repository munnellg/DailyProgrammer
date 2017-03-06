#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_LEN_STAVE   2048 /* length of the stave at start of program */
#define STAVE_GROWTH_RATE   2    /* rate at which dynamic stave array expands */
#define NUM_STRINGS         6    /* number of strings on the guitar */
#define NUM_ARGS            2    /* number of arguments this program expects */
#define NUM_NOTES           12   /* number of musical notes. Basically A-G with all sharps included */
#define BUFFER_SIZE         3    /* buffer size for the fret number input string */

/* convert an integer code to its note equivalent */
char * num2note(int num) {
	static char *notes[] = {
		"C","C#","D", "D#","E","F",
		"F#","G","G#", "A","A#","B"
	};

	return notes[num%NUM_NOTES];
}

/* get the octave of the note referred to by the input number */
int num2octave(int num) {
	return num/NUM_NOTES;
}

/* convert an input note and octave to its integer code */
int note2num( char* note, int octave ) {
	int code=0;
	static char *notes[] = {
		"C","C#","D", "D#","E","F",
		"F#","G","G#", "A","A#","B"
	};

	/* linear search to find the code at zero octave */
	for(code=0; code<NUM_NOTES; code++) {
		if(strcmp(note, notes[code]) == 0) {
			break;
		}
	}

	/* return code plus requested octave */
	return code + NUM_NOTES*octave;
}

int main( int argc, char *argv[] ) {
	FILE *f;
	int  string, i,j, stave_len;	
	char *stave;
	char fret_num[BUFFER_SIZE] = {0};
	char note;

	/* initialize guitar string tunings. Could be read from file if  */
	/* we really wanted to                                           */
	int  tuning[NUM_STRINGS] = {
		note2num("E", 4),
		note2num("B", 3),
		note2num("G", 3),
		note2num("D", 3),
		note2num("A", 2),
		note2num("E", 2)
	};

	i = j = 0;

	/* start on the highest E string and work up */
	string = 0;

	/* allocate and initialize memory for the stave */
	stave_len = DEFAULT_LEN_STAVE;
	stave = malloc(sizeof(*stave)*stave_len);
	memset( stave, -1, stave_len);

	/* test for valid inputs */
	if( argc < NUM_ARGS ) {
		printf("usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	f = fopen(argv[1], "r");

	if(!f) {
		printf("unable to open file %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	/* while we are not at the end of the file */
	while( !feof(f) ) {

		/* load the next character from the file */
		note = getc(f);
		
		if(note == '\n') {
			/* if it's a newline character, move to the next string */
			string++;
			i=0;
		} else {			
			if(isdigit(note)) {
				/* if it's a digit, update our fret number buffer */
				fret_num[j++] = note;
				fret_num[j] = 0;	
			} else if (strlen(fret_num) != 0) {

				/* if it's not a digit, but the buffer isn't empty then add a note to our stave */
				stave[i] = atoi(fret_num) + tuning[string];

				/* reset the buffer */
				j=0;
				fret_num[j] = 0;
			}
			
			i++;

			/* if we are out of memory, expand the array */
			if(i >= stave_len) {				
				stave = realloc(stave, stave_len*STAVE_GROWTH_RATE);
				memset( &stave[stave_len], -1, stave_len*STAVE_GROWTH_RATE - stave_len);
				stave_len *= STAVE_GROWTH_RATE;
			}		
		}
	}

	/* close the file */
	fclose(f);

	/* print our output */
	for(i=0; i<stave_len; i++) {
		if (stave[i] > -1) {
			printf("%s%i ", num2note(stave[i]), num2octave(stave[i]));
		}
	}
	printf("\n");

	/* release allocated memory */
	free(stave);

	/* all done */
	return EXIT_SUCCESS;
}