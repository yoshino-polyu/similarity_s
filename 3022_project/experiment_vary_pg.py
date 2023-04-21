import subprocess
import os 
import time
import logging

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)
# logger.propagate = False

TAKE_AVERAGE_ON_TRIALS = True # whether to take average value of indicators to aggregate trials with same levels

# compile the c programs
def compileCPrograms(run_which):
    if run_which == 1:
        subprocess.call(["llvm-g++", "-DMY", "-Wall", "-Werror", "-std=c++17", "-I.", "main.cpp", "-o", "main"])
    else:
        subprocess.call(["llvm-g++", "-DOG", "-Wall", "-Werror", "-std=c++17", "-I.", "main.cpp", "-o", "main"])
    print("Compile finished...")

# run similarity search algorithm on input.txt. Append the result (response time of 10 queries) to appendFile.
def runMain(Q, query_points, K, appendFile=os.path.join("graphdata", "result_default.txt")):
    with open(appendFile, "a") as f:
        subprocess.call(["./main", "-g", str(Q), "-k", str(K), "-b", str(10), "-q", str(query_points)], stdout=f)    
    print("finish run runMain Q = {}, query_points = {}, K = {}".format(Q, query_points, K))

# main method to conduct the experimental design plan in Lab1 4.1
# for each trial, generate a statistic of performance in the format like "Co_OUTPUT_5_1_1.txt"
def NMI_TIME_EXP(run_which):
    K = [10]
    # K = [5, 15, 20]
    Q = [1, 2, 3]
    query_points = [1]
    # Q = [1]
    # query_points = [1]
    for k in K:
        for i in Q:
            for j in query_points:
                if run_which == 1:
                    output_file = os.path.join("{}_OUTPUT_{}_{}_{}.txt".format("My", k, i, j));
                else:
                    output_file = os.path.join("{}_OUTPUT_{}_{}_{}.txt".format("Co", k, i, j));
                runMain(i, j, k, appendFile=output_file)

if __name__ == "__main__":
    run_which = int(input()) # 1 -> my 2 -> competitor
    if run_which == 1:
        print("Run My")
    else:
        print("Run competitor")
    compileCPrograms(run_which)
    NMI_TIME_EXP(run_which)