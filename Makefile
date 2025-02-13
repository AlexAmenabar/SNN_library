# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alejo/Documentos/SNN_codigo/snn_library

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alejo/Documentos/SNN_codigo/snn_library

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/alejo/Documentos/SNN_codigo/snn_library/CMakeFiles /home/alejo/Documentos/SNN_codigo/snn_library//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/alejo/Documentos/SNN_codigo/snn_library/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named main

# Build rule for target.
main: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 main
.PHONY : main

# fast build rule for target.
main/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/build
.PHONY : main/fast

src/helpers.o: src/helpers.c.o
.PHONY : src/helpers.o

# target to build an object file
src/helpers.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/helpers.c.o
.PHONY : src/helpers.c.o

src/helpers.i: src/helpers.c.i
.PHONY : src/helpers.i

# target to preprocess a source file
src/helpers.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/helpers.c.i
.PHONY : src/helpers.c.i

src/helpers.s: src/helpers.c.s
.PHONY : src/helpers.s

# target to generate assembly for a file
src/helpers.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/helpers.c.s
.PHONY : src/helpers.c.s

src/load_data.o: src/load_data.c.o
.PHONY : src/load_data.o

# target to build an object file
src/load_data.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/load_data.c.o
.PHONY : src/load_data.c.o

src/load_data.i: src/load_data.c.i
.PHONY : src/load_data.i

# target to preprocess a source file
src/load_data.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/load_data.c.i
.PHONY : src/load_data.c.i

src/load_data.s: src/load_data.c.s
.PHONY : src/load_data.s

# target to generate assembly for a file
src/load_data.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/load_data.c.s
.PHONY : src/load_data.c.s

src/main.o: src/main.c.o
.PHONY : src/main.o

# target to build an object file
src/main.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.c.o
.PHONY : src/main.c.o

src/main.i: src/main.c.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.c.i
.PHONY : src/main.c.i

src/main.s: src/main.c.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.c.s
.PHONY : src/main.c.s

src/memory_manager.o: src/memory_manager.c.o
.PHONY : src/memory_manager.o

# target to build an object file
src/memory_manager.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/memory_manager.c.o
.PHONY : src/memory_manager.c.o

src/memory_manager.i: src/memory_manager.c.i
.PHONY : src/memory_manager.i

# target to preprocess a source file
src/memory_manager.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/memory_manager.c.i
.PHONY : src/memory_manager.c.i

src/memory_manager.s: src/memory_manager.c.s
.PHONY : src/memory_manager.s

# target to generate assembly for a file
src/memory_manager.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/memory_manager.c.s
.PHONY : src/memory_manager.c.s

src/neuron_models/lif_neuron.o: src/neuron_models/lif_neuron.c.o
.PHONY : src/neuron_models/lif_neuron.o

# target to build an object file
src/neuron_models/lif_neuron.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/neuron_models/lif_neuron.c.o
.PHONY : src/neuron_models/lif_neuron.c.o

src/neuron_models/lif_neuron.i: src/neuron_models/lif_neuron.c.i
.PHONY : src/neuron_models/lif_neuron.i

# target to preprocess a source file
src/neuron_models/lif_neuron.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/neuron_models/lif_neuron.c.i
.PHONY : src/neuron_models/lif_neuron.c.i

src/neuron_models/lif_neuron.s: src/neuron_models/lif_neuron.c.s
.PHONY : src/neuron_models/lif_neuron.s

# target to generate assembly for a file
src/neuron_models/lif_neuron.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/neuron_models/lif_neuron.c.s
.PHONY : src/neuron_models/lif_neuron.c.s

src/neuron_models/neuron_models.o: src/neuron_models/neuron_models.c.o
.PHONY : src/neuron_models/neuron_models.o

# target to build an object file
src/neuron_models/neuron_models.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/neuron_models/neuron_models.c.o
.PHONY : src/neuron_models/neuron_models.c.o

src/neuron_models/neuron_models.i: src/neuron_models/neuron_models.c.i
.PHONY : src/neuron_models/neuron_models.i

# target to preprocess a source file
src/neuron_models/neuron_models.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/neuron_models/neuron_models.c.i
.PHONY : src/neuron_models/neuron_models.c.i

src/neuron_models/neuron_models.s: src/neuron_models/neuron_models.c.s
.PHONY : src/neuron_models/neuron_models.s

# target to generate assembly for a file
src/neuron_models/neuron_models.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/neuron_models/neuron_models.c.s
.PHONY : src/neuron_models/neuron_models.c.s

src/snn_library.o: src/snn_library.c.o
.PHONY : src/snn_library.o

# target to build an object file
src/snn_library.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/snn_library.c.o
.PHONY : src/snn_library.c.o

src/snn_library.i: src/snn_library.c.i
.PHONY : src/snn_library.i

# target to preprocess a source file
src/snn_library.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/snn_library.c.i
.PHONY : src/snn_library.c.i

src/snn_library.s: src/snn_library.c.s
.PHONY : src/snn_library.s

# target to generate assembly for a file
src/snn_library.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/snn_library.c.s
.PHONY : src/snn_library.c.s

src/training_rules/stdp.o: src/training_rules/stdp.c.o
.PHONY : src/training_rules/stdp.o

# target to build an object file
src/training_rules/stdp.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/training_rules/stdp.c.o
.PHONY : src/training_rules/stdp.c.o

src/training_rules/stdp.i: src/training_rules/stdp.c.i
.PHONY : src/training_rules/stdp.i

# target to preprocess a source file
src/training_rules/stdp.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/training_rules/stdp.c.i
.PHONY : src/training_rules/stdp.c.i

src/training_rules/stdp.s: src/training_rules/stdp.c.s
.PHONY : src/training_rules/stdp.s

# target to generate assembly for a file
src/training_rules/stdp.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/training_rules/stdp.c.s
.PHONY : src/training_rules/stdp.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... main"
	@echo "... src/helpers.o"
	@echo "... src/helpers.i"
	@echo "... src/helpers.s"
	@echo "... src/load_data.o"
	@echo "... src/load_data.i"
	@echo "... src/load_data.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/memory_manager.o"
	@echo "... src/memory_manager.i"
	@echo "... src/memory_manager.s"
	@echo "... src/neuron_models/lif_neuron.o"
	@echo "... src/neuron_models/lif_neuron.i"
	@echo "... src/neuron_models/lif_neuron.s"
	@echo "... src/neuron_models/neuron_models.o"
	@echo "... src/neuron_models/neuron_models.i"
	@echo "... src/neuron_models/neuron_models.s"
	@echo "... src/snn_library.o"
	@echo "... src/snn_library.i"
	@echo "... src/snn_library.s"
	@echo "... src/training_rules/stdp.o"
	@echo "... src/training_rules/stdp.i"
	@echo "... src/training_rules/stdp.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

