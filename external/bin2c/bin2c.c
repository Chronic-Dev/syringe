/*
 * bin2c: A Program to convert binary data into C source code
 * Copyright 2004 by Adrian Prantl <adrian@f4z.org>
 *
 * Modified output for greenpois0n
 * Copyright 2010 by Joshua Hill <posixninja@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char GPL[] =                                                                 \
"/**\n"                                                                            \
"  * GreenPois0n Syringe\n"                         \
"  * Copyright (C) 2010 Chronic-Dev Team\n"                                        \
"  *\n"                                                                            \
"  * This program is free software: you can redistribute it and/or modify\n"       \
"  * it under the terms of the GNU General Public License as published by\n"       \
"  * the Free Software Foundation, either version 3 of the License, or\n"          \
"  * (at your option) any later version.\n"                                        \
"  *\n"                                                                            \
"  * This program is distributed in the hope that it will be useful,\n"            \
"  * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"             \
"  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"              \
"  * GNU General Public License for more details.\n"                               \
"  *\n"                                                                            \
"  * You should have received a copy of the GNU General Public License\n"          \
"  * along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"      \
" **/\n\n";

char* self = 0;

void usage() {
	printf("Usage:\n%s input.bin output.h name\n\n", self);
}

void bail_out(const char* s1, const char* s2) {
	fprintf(stderr, "%s: FATAL ERROR:\n%s%s\n", self, s1, s2);
	exit(1);
}

int main(int argc, char** argv) {
	FILE *fi, *fo;
	int c, i;

	self = argv[0];

	if (argc != 4) {
		usage();
		return 0;
	}

	if ((fi = fopen(argv[1], "rb")) == 0)
		bail_out("Cannot open input file ", argv[1]);

	if ((fo = fopen(argv[2], "w")) == 0)
		bail_out("Cannot open output file ", argv[2]);

	if ((c = fgetc(fi)) != EOF) {
		fprintf(fo, GPL);
		fprintf(fo, "#ifndef %s_H\n", argv[3]);
		fprintf(fo, "#define %s_H\n\n", argv[3]);
		fprintf(fo, "const unsigned char %s[] = {\n", argv[3]);
		fprintf(fo, c < 16 ? "   0x%02x" : "    0x%02x", (unsigned char) c);
	}

	i = 1;
	while ((c = fgetc(fi)) != EOF) {
		if (i < 12)
			fprintf(fo, c < 16 ? ", 0x%02x" : ", 0x%02x", (unsigned char) c);
		else {
			fprintf(fo, c < 16 ? ",\n   0x%02x" : ",\n   0x%02x", (unsigned char) c);
			i = 0;
		}
		i++;
	}
	fprintf(fo, "\n};\n\n");
	fprintf(fo, "#endif\n");

	printf("converted %s\n", argv[1]);

	return 0;
}

