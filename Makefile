TARGET_NAME = painter
TARGET_NAME_GUI = painter-gui
STUID         = 161240004
MONTH         = $(shell date +%m | sed 's/^0*//')
PACKAGE       = $(STUID)_$(MONTH)月报告.zip
BUILD_DIR     = build
SRC_DIR       = source/src
UI_DIR        = source/cli
INCLUDE_DIR   = source/include
BINARY_DIR    = binary
BINARY  ?= $(BUILD_DIR)/$(TARGET_NAME)
BINARY_GUI ?= $(BUILD_DIR)/$(TARGET_NAME_GUI)

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

.DEFAULT_GOAL = $(BINARY_GUI)
.PHONY : clean run doc package targets

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
	@echo - [RM] $(BUILD_DIR) $(BINARY_DIR)
	@rm -rf $(BUILD_DIR) $(BINARY_DIR)
	-@cd source/gui/Paint-GUI && make clean

$(BINARY_GUI) : 
	@cd source/gui/Paint-GUI && qmake && make
	@echo + [CP] $@
	@mkdir -p $(dir $@)
	@cp source/gui/Paint-GUI/Paint-GUI $@

targets : $(BINARY) $(BINARY_GUI)
	mkdir -p $(BINARY_DIR)
	cp $(BINARY) $(BINARY_GUI) $(BINARY_DIR)

package : doc targets $(BINARY) $(BINARY_GUI)
	rm -f $(STUID)_*月报告.zip
	-@cd source/gui/Paint-GUI && make clean
	zip -r $(PACKAGE) $(BINARY_DIR) picture source README.md report.pdf LICENSE Makefile
	printf "@ report.pdf\n@=$(STUID)_$(MONTH)月报告.pdf\n" | zipnote -w $(PACKAGE)
	printf "@ README.md\n@=$(STUID)_系统使用说明.md\n" | zipnote -w $(PACKAGE)

.DELETE_ON_ERROR : 

