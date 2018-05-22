OBJS = draw.o output.o matrix.o parser.o shapes.o rcs.o lighting.o vmath.o symtab.o print_pcode.o my_main.o
CC = gcc
OUTPUT = picture.ppm
EXEC = exec
MATH_LIB = -lm
SCRIPT=robot.mdl

all: parser
	./$(EXEC) $(SCRIPT)

parser: lex.yy.c y.tab.c y.tab.h $(OBJS)
	$(CC) -o $(EXEC) lex.yy.c y.tab.h y.tab.c $(OBJS) $(MATH_LIB)

lex.yy.c:
	flex -I compiler/mdl.l

y.tab.c:
	bison -d -y compiler/mdl.y

y.tab.h:
	bison -d -y compiler/mdl.y

symtab.o:
	$(CC) $(DBG) -Wall -c compiler/symtab.c compiler/symtab.h

print_pcode.o:
	$(CC) $(DBG) -Wall -c compiler/print_pcode.c compiler/parser.h include/matrix.h

my_main.o:
	$(CC) $(DBG) -Wall -c my_main.c

main.o:
	$(CC) $(DBG) -Wall -c main.c include/draw.h include/output.h include/rcs.h

draw.o:
	$(CC) $(DBG) -Wall -c draw.c include/draw.h

output.o:
	$(CC) $(DBG) -Wall -c output.c include/output.h

matrix.o:
	$(CC) $(DBG) -Wall -c matrix.c include/matrix.h

parser.o:
	$(CC) $(DBG) -Wall -c parser.c include/parser.h 

shapes.o:
	$(CC) $(DBG) -Wall -c shapes.c include/shapes.h

rcs.o:
	$(CC) $(DBG) -Wall -c rcs.c include/rcs.h

lighting.o:
	$(CC) $(DBG) -Wall -c lighting.c include/lighting.h

vmath.o:
	$(CC) $(DBG) -Wall -c vmath.c include/vmath.h

clean:
	rm -rf *.o $(OUTPUT)
	rm -rf y.tab.*
	rm -rf lex.yy.c
	rm -rf mdl.dSYM

