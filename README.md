			 ___  ___ __  _ ___  ___  __ __ 
			/ __>| __>\ \/ | . |/ __>|  \  \
			\__ \| _>  \ \ |   |\__ \|     |
			<___/|_|  _/\_\|_|_|<___/|_|_|_|
                        
			"A Nintendo SuperFX Code compiler"
			(c) 2015 mohhingman


Project from https://sourceforge.net/projects/sfxasm/ <br>
The vast majority of this code is not mine, I am just cleaning it up a bit and making it compile properly <br>

Revision History

1.1 11/17/2021 

- Use `sprintf` instead of `itoa` (why?)
- delete all the binaries and libs that were in the sourceforge repo (why? x2)
- reformatted with `clang-format`
- Compiles great with GCC, but clang gives 11 warnings I should probably fix

1.0	03/09/2015 "First release"

- Support for all instructions
- Compile to binary file
- Supports static constants and automatic calculating labels

Reference

http://en.wikibooks.org/wiki/Super_NES_Programming/Super_FX_tutorial

Table of Contents

1. Instruction set
2. Assembler directives
2.1 Label
2.2 Define
2.3 Comment
3. Numeric Expressions
4. Constraints
5. Integration  



This program compiles SuperFX assembly language into binary form, intended as an include file in a standard snes assembly file.

usage: sfxasm [asmfile] [outputfile]

If no errors are detected, it will produce a binary file "outputfile". eg scale.sfx, plot.sfx etc
   
1. Instruction Set

All instrucitons are supported. There are two addressing modes for some instructions - Register and Immediate. The instructions which support immediate mode have an alternate
form which ends with a # suffix.  

adc
adc#
add
add
alt1
alt2
alt3
and
and
asr
bcc
bcs
beq
bge
bic
bic#
blt
bmi
bne,
bpl,
bra
bvc
bvs
cache
cmode
cmp
color
dec
div2
fmult
from
getb
getbh
getbl
getbs
getc
hib
ibt
inc
iwt
jmp
ldb
ldw
lea
link
ljmp
lm
lms
lmult
lob
loop
lsr
merge
move
moves
mult
mult#
nop
not
or
or
plot
ramb
rol
romb
ror
rpix
sbc
sbk
sex
sm
sms
stb
stop
stw
sub
sub#
swap
to
umult
umult#
with
xor
xor#

For information on the instructions, please visit the SuperFX Tutorial page.
http://en.wikibooks.org/wiki/Super_NES_Programming/Super_FX_tutorial

2. Assembler directives.

The directives are quite minimal.

2.1 Label

.LABEL name

name can be up to 32 characters long, can use any combination of characters except space, and the first character must be a letter. Case sensitive.

For all branch instructions, this specifies where to jump the program counter to. 
Note, the jump must be within -128 to +127 bytes relative to the branch instruciton. The compiler will give an error if a particular jump is too far.

2.2 Define

.DEF name,number

This creates a static constant which can be defined anywhere in the code. 

name can be up to 32 characters long, can use any combination of characters except space, and the first character must be a letter. Case sensitive.
the number can be expressed as decimal, binary or hex. 

2.3 Comment

Comments can be placed on it's own line or after a code statement with the semicolon symbol.

eg

; Comment test 1 2
and $F ;commenting on this code

3. Numeric Expressions

Numbers can be expressed as decimal, binary and hex.

Decminal - eg 1023, 1, 276
Binary - eg %1001, %0101, #1001010010101111
Hex - eg $FA, $0A, $FE34
        
4.Constraints

- All code must start at the first position in line, i.e. no indentation.
- There must be a single space between the instruction name and the first argument
- THere must be no spaces between arguments and their comma separator eg 

4. Integration
To include the compiled superFX binary file in your SNES assembly file, use the incbin statement.

eg.
BANK 2 SLOT 0
.ORG 0
.incbin "..\plotfunction.sfx"

5.Calling SuperFX code in snes program

This is the part I haven't quite figured out yet. The plan is to use the mvn or mvp 65C816 instruction to move the included binary into SuperFX Cache ram.
Please see the tutorial page for more info.

Regards
mohhingman
