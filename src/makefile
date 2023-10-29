
OUT = disco
OBJ = 3dmath.o add.o blur.o cubic.o feedback.o flash.o flowers.o font.o generator.o greets.o main.o overlay.o render.o settings.o soap.o spiral.o text.o tunnel.o weight.o wobble.o
LIB = 

CPPFLAGS = -Wall -g
CPP = g++

$(OUT): $(OBJ)
	$(CPP) -o $(OUT) $(OBJ) -framework OpenGL -framework GLUT

-include $(OBJ:.o=.d)

%.d: %.cc
	@set -e; $(CPP) -MM $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@

clean: 
	-rm $(OUT) $(OBJ)
