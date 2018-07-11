INCDIR=include
SRCDIR=src
OBJDIR=obj
EXE_MATRIX = makematrix
EXE = horst

CPP = g++
CPPFLAGS = -O3 -Wall -Wextra -Wshadow -std=c++11 -I$(INCDIR)
ROOTFLAGS=`root-config --cflags --glibs`

_DEPS = Config.h FitFunction.h Fitter.h InputFileReader.h MakeMatrix.h Reconstructor.h Uncertainty.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS)) 

_OBJ_MATRIX = MakeMatrix.o InputFileReader.o
OBJ_MATRIX = $(patsubst %,$(OBJDIR)/%,$(_OBJ_MATRIX))

_OBJ = FitFunction.o Fitter.o horst.o InputFileReader.o Reconstructor.o Uncertainty.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS) $(ROOTFLAGS) 

all: $(EXE) $(EXE_MATRIX)

debug: CPPFLAGS += -g -Wconversion -Wsign-conversion
debug: $(EXE) $(EXE_MATRIX)


$(EXE_MATRIX): $(OBJ_MATRIX) 
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

$(EXE): $(OBJ)
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

clean:
	rm -rf obj/*
	rm -f makematrix
	rm -f horst
