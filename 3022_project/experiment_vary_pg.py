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

# generate the graph with n vertices and m edges, the graph is put in the output_file.
def generateRandomGraph(n, m, seed="", output_file=os.path.join("graphdata", "input_default.txt")):
    logger.debug(f"generating random graph with n = {n} m = {m} seed = {seed} output_file with path = {output_file}")
    with open(output_file, "w") as f:
        logger.debug("type of seed = %s", type(seed))
        if seed:   
            subprocess.call(["./randomgraph", str(n), str(m), seed], stdout=f)
        else:
            subprocess.call(["./randomgraph", str(n), str(m)], stdout=f)
    
    print("Random graph has been written into {}".format(output_file))

def runMain(Q, query_points, K, appendFile=os.path.join("graphdata", "result_default.txt")):
    with open(appendFile, "a") as f:
        subprocess.call(["./main", "-g", str(Q), "-k", str(K), "-b", str(10), "-q", str(query_points)], stdout=f)    
    print("finish run runMain Q = {}, query_points = {}, K = {}".format(Q, query_points, K))

# run RandomGreedy algorithm on graph in inputfile. Append the result to appendFile.
# the output format is "n m iterations #OfColorsChecked usedColorNumber"
def runRandomGreedyToFile(inputfile, iterations, seed="", appendFile=os.path.join("graphdata", "result_default.txt")):
    with open(appendFile, "a") as f:
        if seed:
            subprocess.call(["./randomgreedy", "-f", inputfile, "-i", str(iterations), "-s", seed, "-b"], stdout=f)
        else:
            subprocess.call(["./randomgreedy", "-f", inputfile, "-i", str(iterations), "-b"], stdout=f)
    print("Run random greedy ({},i={}) complete...".format(inputfile, iterations))

# same with runRandomGreedyToFile but return the output but not to file.
def runRandomGreedyReturnResults(inputfile, iterations, seed=""):
    if seed:
        result = subprocess.check_output(["./randomgreedy", "-f", inputfile, "-i", str(iterations), "-s", seed, "-b"]).decode("utf-8")
    else:
        result = subprocess.check_output(["./randomgreedy", "-f", inputfile, "-i", str(iterations), "-b"]).decode("utf-8")
    print("Run random greedy ({},i={}) complete...".format(inputfile, iterations))
    return result

# main method to conduct the experimental design plan in Lab1 4.1
# for each trial, generate a random graph in the format like "input_n_10_m_40_s_12345.txt" in graphdata
# use RandomGreedy algorithm calculate the result and append the output in file "RANDOM_NMI_TIME_RES_{time}.txt" in resdata folder
def NMI_TIME_EXP(run_which):
    # K = [10]
    K = [5, 15, 20]
    # Q = [1, 2, 3]
    # query_points = [1, 2, 3]
    Q = [1]
    query_points = [1]
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