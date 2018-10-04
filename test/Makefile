INCDIR=include
SRCDIR=src
OBJDIR=obj

EXE=create_test_data

CPP = g++
CPPFLAGS = -O3 -Wall -Wextra -Wshadow -std=c++11 -I$(INCDIR)
ROOTFLAGS = -isystem$(shell root-config --incdir) -L$(shell root-config --libdir) -lCore -lRIO -lHist -lMathCore

_DEPS = ConfigTest.h ResponseMatrixCreator.h SpectrumCreator.h 
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = ResponseMatrixCreator.o SpectrumCreator.o create_test_data.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS) $(ROOTFLAGS)

$(EXE): $(OBJ)
	$(CPP) -o $@ $^ $(CPPFLAGS) $(ROOTFLAGS)

clean:
	rm -rf obj/*
	rm -f create_test_data
