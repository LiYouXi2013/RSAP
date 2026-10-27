# ============ 平台检测 ============
UNAME_S := $(shell uname -s 2>/dev/null)

ifeq ($(OS),Windows_NT)
    MKFILE := Makefile.win
else ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME_S)))
    MKFILE := Makefile.win
else ifeq ($(UNAME_S),Linux)
    MKFILE := Makefile.linux
else
    $(error Unsupported platform: OS=$(OS) UNAME_S=$(UNAME_S))
endif

# ============ 默认目标：跳转到对应平台的 all ============
.PHONY: all clean run info FORCE
FORCE:

all:
	@echo ">>> $(MKFILE) (all)"
	@$(MAKE) --no-print-directory -f $(MKFILE) all

# 其它目标也一并转发，方便使用
clean run info:
	@$(MAKE) --no-print-directory -f $(MKFILE) $@