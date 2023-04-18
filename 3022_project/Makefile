#
# for C++ 
#
CC = llvm-g++
# CC is the path of compiler
CFLAGS = -Wall -g -Werror -DMY -std=c++17 -stdlib=libc++ -Wno-unused-function
# -Werror: Make all warnings into errors.
# -Wall: enables all the warnings about constructions that some users consider questionable
# -g: tells the compiler to embed debugging information for the debugger to use.
# -O2: O2 optimisation
# -Wno-unused-function: turns off gcc warnings about unused function.

SRC_DIR = .
EXECUTABLE = $(SRC_DIR)/main
# main
MAIN_PROGRAM = $(SRC_DIR)/main.cpp

# CODE = COMMON + CPU + MEMORY + MAIN_HARDWARE

.PHONY:cli
cli:
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(MAIN_PROGRAM) -o $(EXECUTABLE)
# -I means set path of #include to the path of specified (SRC) working directory so that we could use relative path when writing #include
# -o means generate an executable named EXE. 

clean:
	rm -f *.o *~ $(EXECUTABLE)
# rm -f means delete a directory recursively. 
# *~ means all files ending in ~. Many Unix/Linux systems programs create backup files that end in ~.
run:
	$(EXECUTABLE) > stats.txt
	

#
# for generating query point series
#
# CC = llvm-g++
# # CC is the path of compiler
# CFLAGS = -Wall -g -Werror -std=c++17 -stdlib=libc++ -Wno-unused-function
# # -Werror: Make all warnings into errors.
# # -Wall: enables all the warnings about constructions that some users consider questionable
# # -g: tells the compiler to embed debugging information for the debugger to use.
# # -O2: O2 optimisation
# # -Wno-unused-function: turns off gcc warnings about unused function.


# SRC_DIR = .
# EXECUTABLE = $(SRC_DIR)/generate_q
# # main
# MAIN_PROGRAM = $(SRC_DIR)/generate_q.cpp

# # CODE = COMMON + CPU + MEMORY + MAIN_HARDWARE

# .PHONY:cli
# cli:
# 	$(CC) $(CFLAGS) -I$(SRC_DIR) $(MAIN_PROGRAM) -o $(EXECUTABLE)
# # -I means set path of #include to the path of specified (SRC) working directory so that we could use relative path when writing #include
# # -o means generate an executable named EXE. 

# clean:
# 	rm -f *.o *~ $(EXECUTABLE)
# # rm -f means delete a directory recursively. 
# # *~ means all files ending in ~. Many Unix/Linux systems programs create backup files that end in ~.
# run:
# 	$(EXECUTABLE) -g 1 -k 10 -b 10 > query_points_01.txt
# 	$(EXECUTABLE) -g 2 -k 10 -b 10 > query_points_02.txt
# 	$(EXECUTABLE) -g 3 -k 10 -b 10 > query_points_03.txt
	
	


#
# for C
#
# CC = llvm-g++
# # CC is the path of compiler
# CFLAGS = -Wall -g -Werror -std=c++17 -stdlib=libc++ -Wno-unused-function
# # -Werror: Make all warnings into errors.
# # -Wall: enables all the warnings about constructions that some users consider questionable
# # -g: tells the compiler to embed debugging information for the debugger to use.
# # -O2: O2 optimisation
# # -Wno-unused-function: turns off gcc warnings about unused function.

# SRC_DIR = .
# EXECUTABLE = $(SRC_DIR)/randomkeyword
# # main
# MAIN_PROGRAM = $(SRC_DIR)/randomkeyword.cpp

# # CODE = COMMON + CPU + MEMORY + MAIN_HARDWARE

# .PHONY:cli
# cli:
# 	$(CC) $(CFLAGS) -I$(SRC_DIR) $(MAIN_PROGRAM) -o $(EXECUTABLE)
# # -I means set path of #include to the path of specified (SRC) working directory so that we could use relative path when writing #include
# # -o means generate an executable named EXE. 

# clean:
# 	rm -f *.o *~ $(EXECUTABLE)
# # rm -f means delete a directory recursively. 
# # *~ means all files ending in ~. Many Unix/Linux systems programs create backup files that end in ~.
# run:
# 	$(EXECUTABLE) > input_01.txt