# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

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
CMAKE_COMMAND = /home/luojiu/下载/软件杂物/clion-2022.2.5/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/luojiu/下载/软件杂物/clion-2022.2.5/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/luojiu/CLionProjects/sql_ast

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/luojiu/CLionProjects/sql_ast/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/token.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/token.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/token.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/token.dir/flags.make

CMakeFiles/token.dir/ast/node.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/ast/node.cpp.o: ../ast/node.cpp
CMakeFiles/token.dir/ast/node.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/token.dir/ast/node.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/ast/node.cpp.o -MF CMakeFiles/token.dir/ast/node.cpp.o.d -o CMakeFiles/token.dir/ast/node.cpp.o -c /home/luojiu/CLionProjects/sql_ast/ast/node.cpp

CMakeFiles/token.dir/ast/node.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/ast/node.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/ast/node.cpp > CMakeFiles/token.dir/ast/node.cpp.i

CMakeFiles/token.dir/ast/node.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/ast/node.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/ast/node.cpp -o CMakeFiles/token.dir/ast/node.cpp.s

CMakeFiles/token.dir/json/json.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/json/json.cpp.o: ../json/json.cpp
CMakeFiles/token.dir/json/json.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/token.dir/json/json.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/json/json.cpp.o -MF CMakeFiles/token.dir/json/json.cpp.o.d -o CMakeFiles/token.dir/json/json.cpp.o -c /home/luojiu/CLionProjects/sql_ast/json/json.cpp

CMakeFiles/token.dir/json/json.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/json/json.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/json/json.cpp > CMakeFiles/token.dir/json/json.cpp.i

CMakeFiles/token.dir/json/json.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/json/json.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/json/json.cpp -o CMakeFiles/token.dir/json/json.cpp.s

CMakeFiles/token.dir/json/parser.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/json/parser.cpp.o: ../json/parser.cpp
CMakeFiles/token.dir/json/parser.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/token.dir/json/parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/json/parser.cpp.o -MF CMakeFiles/token.dir/json/parser.cpp.o.d -o CMakeFiles/token.dir/json/parser.cpp.o -c /home/luojiu/CLionProjects/sql_ast/json/parser.cpp

CMakeFiles/token.dir/json/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/json/parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/json/parser.cpp > CMakeFiles/token.dir/json/parser.cpp.i

CMakeFiles/token.dir/json/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/json/parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/json/parser.cpp -o CMakeFiles/token.dir/json/parser.cpp.s

CMakeFiles/token.dir/lexer/lexer.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/lexer/lexer.cpp.o: ../lexer/lexer.cpp
CMakeFiles/token.dir/lexer/lexer.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/token.dir/lexer/lexer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/lexer/lexer.cpp.o -MF CMakeFiles/token.dir/lexer/lexer.cpp.o.d -o CMakeFiles/token.dir/lexer/lexer.cpp.o -c /home/luojiu/CLionProjects/sql_ast/lexer/lexer.cpp

CMakeFiles/token.dir/lexer/lexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/lexer/lexer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/lexer/lexer.cpp > CMakeFiles/token.dir/lexer/lexer.cpp.i

CMakeFiles/token.dir/lexer/lexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/lexer/lexer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/lexer/lexer.cpp -o CMakeFiles/token.dir/lexer/lexer.cpp.s

CMakeFiles/token.dir/parser/expression.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/expression.cpp.o: ../parser/expression.cpp
CMakeFiles/token.dir/parser/expression.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/token.dir/parser/expression.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/expression.cpp.o -MF CMakeFiles/token.dir/parser/expression.cpp.o.d -o CMakeFiles/token.dir/parser/expression.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/expression.cpp

CMakeFiles/token.dir/parser/expression.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/expression.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/expression.cpp > CMakeFiles/token.dir/parser/expression.cpp.i

CMakeFiles/token.dir/parser/expression.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/expression.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/expression.cpp -o CMakeFiles/token.dir/parser/expression.cpp.s

CMakeFiles/token.dir/parser/float.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/float.cpp.o: ../parser/float.cpp
CMakeFiles/token.dir/parser/float.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/token.dir/parser/float.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/float.cpp.o -MF CMakeFiles/token.dir/parser/float.cpp.o.d -o CMakeFiles/token.dir/parser/float.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/float.cpp

CMakeFiles/token.dir/parser/float.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/float.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/float.cpp > CMakeFiles/token.dir/parser/float.cpp.i

CMakeFiles/token.dir/parser/float.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/float.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/float.cpp -o CMakeFiles/token.dir/parser/float.cpp.s

CMakeFiles/token.dir/parser/group.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/group.cpp.o: ../parser/group.cpp
CMakeFiles/token.dir/parser/group.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/token.dir/parser/group.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/group.cpp.o -MF CMakeFiles/token.dir/parser/group.cpp.o.d -o CMakeFiles/token.dir/parser/group.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/group.cpp

CMakeFiles/token.dir/parser/group.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/group.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/group.cpp > CMakeFiles/token.dir/parser/group.cpp.i

CMakeFiles/token.dir/parser/group.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/group.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/group.cpp -o CMakeFiles/token.dir/parser/group.cpp.s

CMakeFiles/token.dir/parser/identifier.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/identifier.cpp.o: ../parser/identifier.cpp
CMakeFiles/token.dir/parser/identifier.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/token.dir/parser/identifier.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/identifier.cpp.o -MF CMakeFiles/token.dir/parser/identifier.cpp.o.d -o CMakeFiles/token.dir/parser/identifier.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/identifier.cpp

CMakeFiles/token.dir/parser/identifier.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/identifier.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/identifier.cpp > CMakeFiles/token.dir/parser/identifier.cpp.i

CMakeFiles/token.dir/parser/identifier.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/identifier.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/identifier.cpp -o CMakeFiles/token.dir/parser/identifier.cpp.s

CMakeFiles/token.dir/parser/infix.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/infix.cpp.o: ../parser/infix.cpp
CMakeFiles/token.dir/parser/infix.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/token.dir/parser/infix.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/infix.cpp.o -MF CMakeFiles/token.dir/parser/infix.cpp.o.d -o CMakeFiles/token.dir/parser/infix.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/infix.cpp

CMakeFiles/token.dir/parser/infix.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/infix.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/infix.cpp > CMakeFiles/token.dir/parser/infix.cpp.i

CMakeFiles/token.dir/parser/infix.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/infix.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/infix.cpp -o CMakeFiles/token.dir/parser/infix.cpp.s

CMakeFiles/token.dir/parser/integer.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/integer.cpp.o: ../parser/integer.cpp
CMakeFiles/token.dir/parser/integer.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/token.dir/parser/integer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/integer.cpp.o -MF CMakeFiles/token.dir/parser/integer.cpp.o.d -o CMakeFiles/token.dir/parser/integer.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/integer.cpp

CMakeFiles/token.dir/parser/integer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/integer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/integer.cpp > CMakeFiles/token.dir/parser/integer.cpp.i

CMakeFiles/token.dir/parser/integer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/integer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/integer.cpp -o CMakeFiles/token.dir/parser/integer.cpp.s

CMakeFiles/token.dir/parser/null.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/null.cpp.o: ../parser/null.cpp
CMakeFiles/token.dir/parser/null.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/token.dir/parser/null.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/null.cpp.o -MF CMakeFiles/token.dir/parser/null.cpp.o.d -o CMakeFiles/token.dir/parser/null.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/null.cpp

CMakeFiles/token.dir/parser/null.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/null.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/null.cpp > CMakeFiles/token.dir/parser/null.cpp.i

CMakeFiles/token.dir/parser/null.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/null.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/null.cpp -o CMakeFiles/token.dir/parser/null.cpp.s

CMakeFiles/token.dir/parser/parser.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/parser.cpp.o: ../parser/parser.cpp
CMakeFiles/token.dir/parser/parser.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/token.dir/parser/parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/parser.cpp.o -MF CMakeFiles/token.dir/parser/parser.cpp.o.d -o CMakeFiles/token.dir/parser/parser.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/parser.cpp

CMakeFiles/token.dir/parser/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/parser.cpp > CMakeFiles/token.dir/parser/parser.cpp.i

CMakeFiles/token.dir/parser/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/parser.cpp -o CMakeFiles/token.dir/parser/parser.cpp.s

CMakeFiles/token.dir/parser/prefix.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/prefix.cpp.o: ../parser/prefix.cpp
CMakeFiles/token.dir/parser/prefix.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/token.dir/parser/prefix.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/prefix.cpp.o -MF CMakeFiles/token.dir/parser/prefix.cpp.o.d -o CMakeFiles/token.dir/parser/prefix.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/prefix.cpp

CMakeFiles/token.dir/parser/prefix.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/prefix.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/prefix.cpp > CMakeFiles/token.dir/parser/prefix.cpp.i

CMakeFiles/token.dir/parser/prefix.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/prefix.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/prefix.cpp -o CMakeFiles/token.dir/parser/prefix.cpp.s

CMakeFiles/token.dir/parser/program.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/program.cpp.o: ../parser/program.cpp
CMakeFiles/token.dir/parser/program.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/token.dir/parser/program.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/program.cpp.o -MF CMakeFiles/token.dir/parser/program.cpp.o.d -o CMakeFiles/token.dir/parser/program.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/program.cpp

CMakeFiles/token.dir/parser/program.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/program.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/program.cpp > CMakeFiles/token.dir/parser/program.cpp.i

CMakeFiles/token.dir/parser/program.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/program.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/program.cpp -o CMakeFiles/token.dir/parser/program.cpp.s

CMakeFiles/token.dir/parser/select.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/select.cpp.o: ../parser/select.cpp
CMakeFiles/token.dir/parser/select.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/token.dir/parser/select.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/select.cpp.o -MF CMakeFiles/token.dir/parser/select.cpp.o.d -o CMakeFiles/token.dir/parser/select.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/select.cpp

CMakeFiles/token.dir/parser/select.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/select.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/select.cpp > CMakeFiles/token.dir/parser/select.cpp.i

CMakeFiles/token.dir/parser/select.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/select.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/select.cpp -o CMakeFiles/token.dir/parser/select.cpp.s

CMakeFiles/token.dir/parser/statement.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/statement.cpp.o: ../parser/statement.cpp
CMakeFiles/token.dir/parser/statement.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/token.dir/parser/statement.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/statement.cpp.o -MF CMakeFiles/token.dir/parser/statement.cpp.o.d -o CMakeFiles/token.dir/parser/statement.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/statement.cpp

CMakeFiles/token.dir/parser/statement.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/statement.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/statement.cpp > CMakeFiles/token.dir/parser/statement.cpp.i

CMakeFiles/token.dir/parser/statement.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/statement.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/statement.cpp -o CMakeFiles/token.dir/parser/statement.cpp.s

CMakeFiles/token.dir/parser/string.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/parser/string.cpp.o: ../parser/string.cpp
CMakeFiles/token.dir/parser/string.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/token.dir/parser/string.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/parser/string.cpp.o -MF CMakeFiles/token.dir/parser/string.cpp.o.d -o CMakeFiles/token.dir/parser/string.cpp.o -c /home/luojiu/CLionProjects/sql_ast/parser/string.cpp

CMakeFiles/token.dir/parser/string.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/parser/string.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/parser/string.cpp > CMakeFiles/token.dir/parser/string.cpp.i

CMakeFiles/token.dir/parser/string.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/parser/string.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/parser/string.cpp -o CMakeFiles/token.dir/parser/string.cpp.s

CMakeFiles/token.dir/token/token.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/token/token.cpp.o: ../token/token.cpp
CMakeFiles/token.dir/token/token.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/token.dir/token/token.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/token/token.cpp.o -MF CMakeFiles/token.dir/token/token.cpp.o.d -o CMakeFiles/token.dir/token/token.cpp.o -c /home/luojiu/CLionProjects/sql_ast/token/token.cpp

CMakeFiles/token.dir/token/token.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/token/token.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/token/token.cpp > CMakeFiles/token.dir/token/token.cpp.i

CMakeFiles/token.dir/token/token.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/token/token.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/token/token.cpp -o CMakeFiles/token.dir/token/token.cpp.s

CMakeFiles/token.dir/token.cpp.o: CMakeFiles/token.dir/flags.make
CMakeFiles/token.dir/token.cpp.o: ../token.cpp
CMakeFiles/token.dir/token.cpp.o: CMakeFiles/token.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building CXX object CMakeFiles/token.dir/token.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/token.dir/token.cpp.o -MF CMakeFiles/token.dir/token.cpp.o.d -o CMakeFiles/token.dir/token.cpp.o -c /home/luojiu/CLionProjects/sql_ast/token.cpp

CMakeFiles/token.dir/token.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/token.dir/token.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/luojiu/CLionProjects/sql_ast/token.cpp > CMakeFiles/token.dir/token.cpp.i

CMakeFiles/token.dir/token.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/token.dir/token.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/luojiu/CLionProjects/sql_ast/token.cpp -o CMakeFiles/token.dir/token.cpp.s

# Object files for target token
token_OBJECTS = \
"CMakeFiles/token.dir/ast/node.cpp.o" \
"CMakeFiles/token.dir/json/json.cpp.o" \
"CMakeFiles/token.dir/json/parser.cpp.o" \
"CMakeFiles/token.dir/lexer/lexer.cpp.o" \
"CMakeFiles/token.dir/parser/expression.cpp.o" \
"CMakeFiles/token.dir/parser/float.cpp.o" \
"CMakeFiles/token.dir/parser/group.cpp.o" \
"CMakeFiles/token.dir/parser/identifier.cpp.o" \
"CMakeFiles/token.dir/parser/infix.cpp.o" \
"CMakeFiles/token.dir/parser/integer.cpp.o" \
"CMakeFiles/token.dir/parser/null.cpp.o" \
"CMakeFiles/token.dir/parser/parser.cpp.o" \
"CMakeFiles/token.dir/parser/prefix.cpp.o" \
"CMakeFiles/token.dir/parser/program.cpp.o" \
"CMakeFiles/token.dir/parser/select.cpp.o" \
"CMakeFiles/token.dir/parser/statement.cpp.o" \
"CMakeFiles/token.dir/parser/string.cpp.o" \
"CMakeFiles/token.dir/token/token.cpp.o" \
"CMakeFiles/token.dir/token.cpp.o"

# External object files for target token
token_EXTERNAL_OBJECTS =

token: CMakeFiles/token.dir/ast/node.cpp.o
token: CMakeFiles/token.dir/json/json.cpp.o
token: CMakeFiles/token.dir/json/parser.cpp.o
token: CMakeFiles/token.dir/lexer/lexer.cpp.o
token: CMakeFiles/token.dir/parser/expression.cpp.o
token: CMakeFiles/token.dir/parser/float.cpp.o
token: CMakeFiles/token.dir/parser/group.cpp.o
token: CMakeFiles/token.dir/parser/identifier.cpp.o
token: CMakeFiles/token.dir/parser/infix.cpp.o
token: CMakeFiles/token.dir/parser/integer.cpp.o
token: CMakeFiles/token.dir/parser/null.cpp.o
token: CMakeFiles/token.dir/parser/parser.cpp.o
token: CMakeFiles/token.dir/parser/prefix.cpp.o
token: CMakeFiles/token.dir/parser/program.cpp.o
token: CMakeFiles/token.dir/parser/select.cpp.o
token: CMakeFiles/token.dir/parser/statement.cpp.o
token: CMakeFiles/token.dir/parser/string.cpp.o
token: CMakeFiles/token.dir/token/token.cpp.o
token: CMakeFiles/token.dir/token.cpp.o
token: CMakeFiles/token.dir/build.make
token: CMakeFiles/token.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Linking CXX executable token"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/token.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/token.dir/build: token
.PHONY : CMakeFiles/token.dir/build

CMakeFiles/token.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/token.dir/cmake_clean.cmake
.PHONY : CMakeFiles/token.dir/clean

CMakeFiles/token.dir/depend:
	cd /home/luojiu/CLionProjects/sql_ast/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/luojiu/CLionProjects/sql_ast /home/luojiu/CLionProjects/sql_ast /home/luojiu/CLionProjects/sql_ast/cmake-build-debug /home/luojiu/CLionProjects/sql_ast/cmake-build-debug /home/luojiu/CLionProjects/sql_ast/cmake-build-debug/CMakeFiles/token.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/token.dir/depend

