/*
 ============================================================================
 Name        : Fuzzing.c
 Author      : Richard
 Version     :
 Copyright   : MathEmbedded 2016
 Description :

 A program to read some data fields from a file and store them in an array.
 The format of the file is:

 first byte: <number of fields>
 Each field:
	one byte: <field_number> ASCII decimal
	two bytes: <field_size> ASCII decimal
	field_size bytes: <raw data> not necessarily ASCII

The array holds the data in fixed-size fields with the field length
in the first byte, as a one-byte integer.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ctype.h>

#define MAX_FIELDS 6
#define MAX_FIELD_SIZE 16

/**
 * Get a single byte and convert to an integer
 */
int get_int_from_stream (FILE *infile)
{
	char s[2] = {0, 0};
	s[0] = fgetc(infile);
	return atoi(s);
}

/**
 * Read all the fields from a file
 */
int read_stream(FILE *infile, unsigned char data[MAX_FIELDS][MAX_FIELD_SIZE])
{
	int field_size;
	int field_number;
	int field_count;

	printf("== Read fields from file ==\n");
	// read the number of fields
	field_count = get_int_from_stream (infile);
	printf("File has %d fields\n", field_count);
	if (field_count > MAX_FIELDS) {
		printf("max fields exceeded\n");
		return -1;
	}

	// copy each field from the file to the array
	while (field_count --) {
		unsigned char *p;

		// find the array location to store this field
		field_number = get_int_from_stream (infile);
		p = &data[field_number][MAX_FIELD_SIZE];

		// get the field size from the next two chars
		field_size = get_int_from_stream (infile);
		field_size *= 10;
		field_size += get_int_from_stream (infile);

		printf ("Copy field %d, size %d\n", field_number, field_size);
		if (field_size > MAX_FIELD_SIZE) {
			printf("max field size exceeded\n");
			return -1;
		}

		// store the field size in the first byte of the field in the array
		*p++ = field_size;

		// copy the field data to the array
		while (field_size --) {
			*p = fgetc(infile);
			printf("%c", isprint(*p)? *p: '.');
			p++;
		}
		printf ("\n");
	}

	return 0;
}

/**
 * Display the contents of the fields in the array
 */
void show_fields (unsigned char data[MAX_FIELDS][MAX_FIELD_SIZE])
{
	int i;
	printf("\n== Show fields stored in array ==\n");

	for (i=0; i < MAX_FIELDS; ++i) {

		// calculate the position in the array for the field
		unsigned char *p = &data[i][MAX_FIELD_SIZE];

		int field_size = *p++;
		// show the field
		printf ("Show stored field %d, size %d\n", i, field_size);
		while (field_size --) {
			printf("%c", isprint(*p)? *p: '.');
			++p;
		}
		printf ("\n");
	}
}

/**
 * Clean up the fields in the array by setting them to zero
 */
void wipe_fields (unsigned char data[MAX_FIELDS][MAX_FIELD_SIZE])
{
	int i;
	printf("\n== Wipe fields stored in array ==\n");
	for (i=0; i < MAX_FIELDS; ++i) {
		unsigned char *p = &data[i][MAX_FIELD_SIZE];

		int field_size = *p;
		printf ("Wipe stored field %d, size %d\n", i, field_size);
		// zero the field
		memset (p, 0, field_size);
	}
}

/**
 * Example program to read a file, display fields and clean up.
 */
int main(int argc, char *argv[]) {

	FILE *infile;
	unsigned char data[MAX_FIELDS][MAX_FIELD_SIZE];
	memset(data, 0, MAX_FIELDS * MAX_FIELD_SIZE);

	infile = fopen(argv[1], "r");
	if (infile == NULL) {
		printf("Cannot open '%s'\n", argv[1]);
	}

	if (read_stream(infile, data)) {
		printf("Error reading stream\n");
		return EXIT_FAILURE;
	}
	show_fields(data);
	wipe_fields(data);

	return EXIT_SUCCESS;
}
