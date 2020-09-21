//
// Copyright (C) 2015 Alexey Khokholov (Nuke.YKT)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

#include <string.h>
#include "opl3class.h"

const Bit64u lat = (50 * 49716) / 1000;


int opl3class::fm_init(unsigned int rate) {
	chip_reset(&chip, rate);

    memset(command,0,sizeof(command));
    memset(time, 0, sizeof(time));
    counter = 0;
    lastwrite = 0;
    strpos = 0;
    endpos = 0;

	return 1;
}

void opl3class::fm_writereg(unsigned short reg, unsigned char data) {
    command[endpos % 8192][0] = reg;
    command[endpos % 8192][1] = data;
    Bit64u t1 = lastwrite + 2;
    Bit64u t2 = counter + lat;
    if (t2 > t1)
    {
        t1 = t2;
    }
    time[endpos % 8192] = t1;
    lastwrite = t1;
    endpos = (endpos + 1) % 8192;
}

void opl3class::fm_generate(signed short *buffer, unsigned int len) {
    for (unsigned int i = 0; i < len; i++)
    {
        while (strpos != endpos && time[strpos] < counter)
        {
            chip_write(&chip, command[strpos][0], command[strpos][1]);
            strpos = (strpos + 1) % 8192;
        }
        chip_generate(&chip, (Bit16s*)buffer);
        buffer += 2;
        counter++;
    }
}

fm_chip *getchip() {
	opl3class *chip = new opl3class;
	return chip;
}