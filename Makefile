INCDIR=include
SRCDIR=src
OBJDIR=obj
EXE_CONVERTER = convert_to_txt
EXE_MATRIX = makematrix
EXE_TSROH = tsroh
EXE = horst

CPP = g++
CPPFLAGS = -O3 -Wall -Wextra -Wshadow -std=c++11 -I$(INCDIR)
ROOTFLAGS=`root-config --cflags --glibs`

_DEPS = Config.h FitFunction.h Fitter.h InputFileReader.h MakeMatrix.h Reconstructor.h Uncertainty.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS)) 

_OBJ_CONVERTER = HistogramToTxt.o 
OBJ_CONVERTER = $(patsubst %,$(OBJDIR)/%,$(_OBJ_CONVERTER))

_OBJ_MATRIX = MakeMatrix.o InputFileReader.o
OBJ_MATRIX = $(patsubst %,$(OBJDIR)/%,$(_OBJ_MATRIX))

_OBJ_TSROH = InputFileReader.o Reconstructor.o tsroh.o
OBJ_TSROH = $(patsubst %,$(OBJDIR)/%,$(_OBJ_TSROH))

_OBJ = FitFunction.o Fitter.o horst.o InputFileReader.o Reconstructor.o Uncertainty.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS) $(ROOTFLAGS) 

all: $(EXE) $(EXE_MATRIX) $(EXE_CONVERTER) $(EXE_TSROH)

debug: CPPFLAGS += -g -Wconversion -Wsign-conversion
debug: $(EXE) $(EXE_MATRIX) $(EXE_CONVERTER)

$(EXE_CONVERTER): $(OBJ_CONVERTER) 
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

$(EXE_MATRIX): $(OBJ_MATRIX) 
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

$(EXE_TSROH): $(OBJ_TSROH) 
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

$(EXE): $(OBJ)
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

clean:
	rm -rf obj/*
	rm -f makematrix
	rm -f horst
	rm -f convert_to_txt
	rm -f tsroh
