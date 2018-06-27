INCDIR=include
SRCDIR=src
OBJDIR=obj
EXE_MATRIX = makematrix
EXE = horst

CPP = g++
CPPFLAGS = -g -Wall -Wextra -Wconversion -Wsign-conversion -Wshadow -std=c++11 -I$(INCDIR)
ROOTFLAGS=`root-config --cflags --glibs`

_DEPS = Config.h InputFileReader.h MakeMatrix.h Fitter.h FitFunction.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS)) 

_OBJ_MATRIX = MakeMatrix.o InputFileReader.o
OBJ_MATRIX = $(patsubst %,$(OBJDIR)/%,$(_OBJ_MATRIX))

_OBJ = InputFileReader.o horst.o Fitter.o FitFunction.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS) $(ROOTFLAGS) 

all: $(EXE) $(EXE_MATRIX)

$(EXE_MATRIX): $(OBJ_MATRIX) 
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

$(EXE): $(OBJ)
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

clean:
	rm -rf obj/*
	rm -f makematrix
	rm -f horst
