CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
SRCDIR = src
TESTDIR = tests
OBJDIR = build
BINDIR = bin

# Source files
ASSEMBLER_SOURCES = $(SRCDIR)/assembler/assembler.cpp
EMULATOR_SOURCES = $(SRCDIR)/emulator/memory.cpp $(SRCDIR)/emulator/registers.cpp \
				   $(SRCDIR)/emulator/alu.cpp $(SRCDIR)/emulator/cpu.cpp \
				   $(SRCDIR)/emulator/trace_recorder.cpp
MAIN_SOURCES = $(SRCDIR)/main.cpp
TEST_EMULATOR_SOURCES = $(SRCDIR)/emulator/test_emulator.cpp

# Unit test sources
TEST_ALU_SOURCES = $(TESTDIR)/test_alu.cpp
TEST_MEMORY_SOURCES = $(TESTDIR)/test_memory.cpp
TEST_CPU_SOURCES = $(TESTDIR)/test_cpu.cpp
TEST_ASSEMBLER_SOURCES = $(TESTDIR)/test_assembler.cpp

# Object files
ASSEMBLER_OBJECTS = $(ASSEMBLER_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EMULATOR_OBJECTS = $(EMULATOR_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TEST_EMULATOR_OBJECTS = $(TEST_EMULATOR_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Targets
MAIN_TARGET = $(BINDIR)/software-cpu
TEST_EMULATOR_TARGET = $(BINDIR)/test_emulator
TEST_ALU_TARGET = $(BINDIR)/test_alu
TEST_MEMORY_TARGET = $(BINDIR)/test_memory
TEST_CPU_TARGET = $(BINDIR)/test_cpu
TEST_ASSEMBLER_TARGET = $(BINDIR)/test_assembler

.PHONY: all clean test test-all test-alu test-memory test-cpu test-assembler

all: $(MAIN_TARGET) $(TEST_EMULATOR_TARGET) $(TEST_ALU_TARGET) $(TEST_MEMORY_TARGET) $(TEST_CPU_TARGET) $(TEST_ASSEMBLER_TARGET)

$(MAIN_TARGET): $(MAIN_OBJECTS) $(ASSEMBLER_OBJECTS) $(EMULATOR_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_EMULATOR_TARGET): $(TEST_EMULATOR_OBJECTS) $(EMULATOR_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_ALU_TARGET): $(TESTDIR)/test_alu.cpp $(SRCDIR)/emulator/alu.cpp $(SRCDIR)/emulator/registers.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_MEMORY_TARGET): $(TESTDIR)/test_memory.cpp $(SRCDIR)/emulator/memory.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_CPU_TARGET): $(TESTDIR)/test_cpu.cpp $(EMULATOR_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_ASSEMBLER_TARGET): $(TESTDIR)/test_assembler.cpp $(ASSEMBLER_OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)/assembler $(OBJDIR)/emulator

$(BINDIR):
	mkdir -p $(BINDIR)

test: $(TEST_EMULATOR_TARGET)
	./$(TEST_EMULATOR_TARGET)

test-alu: $(TEST_ALU_TARGET)
	./$(TEST_ALU_TARGET)

test-memory: $(TEST_MEMORY_TARGET)
	./$(TEST_MEMORY_TARGET)

test-cpu: $(TEST_CPU_TARGET)
	./$(TEST_CPU_TARGET)

test-assembler: $(TEST_ASSEMBLER_TARGET)
	./$(TEST_ASSEMBLER_TARGET)

test-all: $(TEST_EMULATOR_TARGET) $(TEST_ALU_TARGET) $(TEST_MEMORY_TARGET) $(TEST_CPU_TARGET) $(TEST_ASSEMBLER_TARGET)
	@echo "=== Running All Unit Tests ==="
	@echo ""
	@./$(TEST_ALU_TARGET)
	@echo ""
	@./$(TEST_MEMORY_TARGET)
	@echo ""
	@./$(TEST_CPU_TARGET)
	@echo ""
	@./$(TEST_ASSEMBLER_TARGET)
	@echo ""
	@./$(TEST_EMULATOR_TARGET)
	@echo ""
	@echo "=== All Tests Completed Successfully ==="

clean:
	rm -rf $(OBJDIR) $(BINDIR)