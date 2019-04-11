TARGET_NAME = painter
BUILD_DIR     = build
SRC_DIR       = src
INCLUDE_DIR   = include
BINARY  ?= $(BUILD_DIR)/$(TARGET_NAME)

CXX     = g++
LD      = g++
TEX	= xelatex
CXXFLAGS  += -std=gnu++14 -Wall -ggdb -MMD -O1
CXXFLAGS  += -fsanitize=undefined -fsanitize=address
CXXFLAGS  += -iquote ./$(INCLUDE_DIR)
LDFLAGS = $(CXXFLAGS)

SRCS = $(shell find $(SRC_DIR)/ -name "*.cpp")
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

.DEFAULT_GOAL = $(BINARY)
.PHONY : clean run doc

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo + [CXX] $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(OBJS:.o=.d)

$(BINARY) : $(OBJS)
	@mkdir -p $(dir $@)
	@echo + [LD] $@
	@$(LD) $(LDFLAGS) -o $@ $^

run : $(BINARY)
	@./$(BINARY)

$(BUILD_DIR)/report.pdf : doc/report.tex
	@echo + [TEX] $@
	@$(TEX) -output-directory=$(BUILD_DIR) $<
	@$(TEX) -output-directory=$(BUILD_DIR) $<

doc : $(BUILD_DIR)/report.pdf
	cp $< .

clean :
	@echo - [RM] $(BUILD_DIR)
	@rm -rf $(BUILD_DIR)


