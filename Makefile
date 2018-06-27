INCDIR=include
SRCDIR=src
OBJDIR=obj
EXE_MATRIX = makematrix

CPP = g++
CPPFLAGS = -g -Wall -Wextra -Wconversion -Wsign-conversion -Wshadow -std=c++11 -I$(INCDIR)
ROOTFLAGS=`root-config --cflags --glibs`

_DEPS = Config.h InputFileReader.h MakeMatrix.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS)) 

_OBJ = MakeMatrix.o InputFileReader.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS) $(ROOTFLAGS) 

$(EXE_MATRIX): $(OBJ)
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

clean:
	rm -rf obj/*
	rm -f makematrix
