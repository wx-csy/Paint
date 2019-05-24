TARGET_NAME = painter
BUILD_DIR     = build
SRC_DIR       = src
UI_DIR        = cli
INCLUDE_DIR   = include
BINARY  ?= $(BUILD_DIR)/$(TARGET_NAME)

CXX     = g++
LD      = g++
TEX	= xelatex
BIBTEX	= bibtex
CXXFLAGS  += -std=gnu++14 -Wall -ggdb -MMD -O1
CXXFLAGS  += -fsanitize=undefined -fsanitize=address
CXXFLAGS  += -I ./$(INCLUDE_DIR)
LDFLAGS = $(CXXFLAGS)

SRCS = $(shell find $(SRC_DIR)/ $(UI_DIR)/ -name "*.cpp")
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

.DEFAULT_GOAL = $(BINARY)
.PHONY : clean run doc

$(BUILD_DIR)/%.o : %.cpp
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

$(BUILD_DIR)/report.pdf : doc/report.tex doc/report.bib
	@mkdir -p $(dir $@)
	@cp doc/report.bib $(BUILD_DIR)
	@echo + [TEX] $@
	@cd doc && $(TEX) -output-directory=../$(BUILD_DIR) report.tex
	@cd $(BUILD_DIR) && $(BIBTEX) report
	@cd doc && $(TEX) -output-directory=../$(BUILD_DIR) report.tex
	@cd doc && $(TEX) -output-directory=../$(BUILD_DIR) report.tex

doc : $(BUILD_DIR)/report.pdf
	cp $< .

clean :
	@echo - [RM] $(BUILD_DIR)
	@rm -rf $(BUILD_DIR)

.DELETE_ON_ERROR : $(BUILD_DIR)/report.pdf 

