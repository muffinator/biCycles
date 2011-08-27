#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#       fontgen.py
#       
#       Copyright 2011 Josh <muffin@josh-laptop>
#       
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.
import Image as im
graphicsLib = open('graphics.h', 'w')
graphicsLib.write('#include <avr/pgmspace.h>\n\n')
graphicsLib.write('unsigned char big_one[10][6][32] PROGMEM = { \n')
disp = im.open('screen.jpg')
sdisp= im.open('screen_s.jpg')
pix = disp.load()
spix= sdisp.load()
count = 0
for num in range(10):
	graphicsLib.write('{')
	for y in range(6):
		graphicsLib.write('{')
		page = ""
		for x in range(disp.size[0]/10):
			byte = 0
			for p in range(8):
				byte += int(pix[num*32+x,(y*8+p)]<=(100,100,100))*2**p
			page += str(byte) + ", "
			count += 1
		graphicsLib.write(page + '}, ')
	graphicsLib.write('}, \n')
graphicsLib.write('}; \n \n')
graphicsLib.write('unsigned char lil_one[10][3][16] PROGMEM = { \n')
print count
count = 0
for num in range(10):
	graphicsLib.write('{')
	for y in range(3):
		graphicsLib.write('{')
		page = ""
		for x in range(sdisp.size[0]/10):
			byte = 0
			for p in range(8):
				byte += int(spix[num*16+x, (y*8+p)]<=(100,100,100))*2**p
			page += str(byte) + ","
			count += 1
		graphicsLib.write(page + '}, ')
	graphicsLib.write('}, \n')
graphicsLib.write('};')
sp= [0b0000000, 0b0000000, 0b0000000, 0b0000000]
wa= [0b1000000, 0b1110000, 0b1111111, 0b0000000]
tw= [0b0000000, 0b0000000]
a = [0b1111111, 0b0001001, 0b1111111, 0b0000000]
c = [0b0111110, 0b1000001, 0b0100010, 0b0000000]
d = [0b1111111, 0b1000001, 0b0111110, 0b0000000]
e = [0b1111111, 0b1001001, 0b1000001, 0b0000000]
i = [0b1000001, 0b1111111, 0b1000001, 0b0000000]
l = [0b1111111, 0b1000000, 0b1100000, 0b0000000]
p = [0b1111111, 0b0001001, 0b0001111, 0b0000000]
s = [0b1101111, 0b1001001, 0b1111011, 0b0000000]
t = [0b0000001, 0b1111111, 0b0000001, 0b0000000]
graphicsLib.write('\n')
graphicsLib.write('unsigned char menu[118] PROGMEM = { \n')
fir = [sp, s, p, e, e, d,sp,wa]
sec = [sp, d, i, s, t,sp,wa]
thr = [sp, a, c, c, e, l,sp,wa]
fou = [sp,wa,tw,wa,wa,wa,wa]
let = ""
count = 0
menu = [fir, sec, thr, fou]
for line in menu:
	for l in line:
		for col in l:
			let+= str(col+0x80)+", "
			count+=1
	graphicsLib.write(let + '\n')
	let = ""
graphicsLib.write('};')
graphicsLib.close();
