CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
SRCDIR = src
OBJDIR = build
BINDIR = bin

# Source files
ASSEMBLER_SOURCES = $(SRCDIR)/assembler/assembler.cpp
EMULATOR_SOURCES = $(SRCDIR)/emulator/memory.cpp $(SRCDIR)/emulator/registers.cpp \
                   $(SRCDIR)/emulator/alu.cpp $(SRCDIR)/emulator/cpu.cpp
MAIN_SOURCES = $(SRCDIR)/main.cpp
TEST_SOURCES = $(SRCDIR)/emulator/test_emulator.cpp

# Object files
ASSEMBLER_OBJECTS = $(ASSEMBLER_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EMULATOR_OBJECTS = $(EMULATOR_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Targets
MAIN_TARGET = $(BINDIR)/software-cpu
TEST_TARGET = $(BINDIR)/test_emulator

.PHONY: all clean test

all: $(MAIN_TARGET) $(TEST_TARGET)

$(MAIN_TARGET): $(MAIN_OBJECTS) $(ASSEMBLER_OBJECTS) $(EMULATOR_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJECTS) $(EMULATOR_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)/assembler $(OBJDIR)/emulator

$(BINDIR):
	mkdir -p $(BINDIR)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR)