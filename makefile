#
# Copyright (C) 2025 愛子あゆみ <ayumi.aiko@outlook.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# build methylish when no target is provided
ifeq ($(MAKECMDGOALS),)
	.DEFAULT_GOAL := methylish
endif

# compiler and it's arguments.
CC = clang
CFLAGS = -O3 -static -I./src/include
ERR_LOG = ./local_build/compilerErrors.log
INCLUDE = ./src/include
SOURCES = ./src/include/methylish.c
MAIN = ./src/shellMain/main.c
OUTPUT = ./local_build/methylish

# Checks if the compiler exists or not.
check_compiler:
	@if [ ! -f "$(CC)" ]; then \
		echo -e "\e[0;31mmake: Error: Clang is not found. Please install it or edit the makefile to proceed.\e[0;37m"; \
		exit 1; \
	fi

# builds the shell binary.
methylish: check_compiler
	@rm -f $(ERR_LOG)
	@echo "\e[0;33mmake: Info: Building Methylish\e[0;37m"
	@$(CC) $(CFLAGS) -I$(INCLUDE) $(SOURCES) $(MAIN) -o $(OUTPUT) >$(ERR_LOG) 2>&1 && \
	echo "\e[0;33mmake: Info: Build finished successfully\e[0;37m" || echo "\e[0;31mmake: Error: Compilation failed. Check $(ERR_LOG) for details.\e[0;37m";

# cleans the build artifacts.
clean:
	@echo "\e[0;33mmake: Info: Cleaning build artifacts...\e[0;37m"
	@rm -f $(OUTPUT) $(ERR_LOG)
	@echo "\e[0;33mmake: Info: Clean finished.\e[0;37m"

.PHONY: check_compiler methylish clean