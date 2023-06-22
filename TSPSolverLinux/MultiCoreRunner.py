import multiprocessing as mp
import os
import platform
import random
import shutil
import subprocess
import time

end_file = "exe"
start_file = ""


def sub_process(i):
    # Compile the C++ program
    # handle errors with same time access to file
    time.sleep(random.uniform(0, 1.2))
    # Run the program
    subprocess.call(start_file + 'c.' + end_file + ' ' + str(i), shell=True)


def create_sub_process():
    processes = []
    n_cores = mp.cpu_count()
    with open('communicate/algo_input.txt') as f:
        lines = f.readlines()
        f.close()
    # max cores is
    if int(lines[10]) < n_cores:
        n_cores = int(lines[10])

    # Copy file for each core
    for i in range(n_cores):
        shutil.copyfile("communicate/algo_input.txt", "communicate/algo_input" + str(i) + ".txt")
    for i in range(n_cores):
        p = mp.Process(target=sub_process, args=(i,))
        p.start()
        processes.append(p)

    for p in processes:
        p.join()
    min_dist = float("inf")
    min_path = ""
    # read all best_pathN.txt files and find best distance with path and save it into best_path.txt
    for f in os.listdir("communicate"):
        if f.startswith("best_path") and f != "best_path.txt":
            with open("communicate/" + f) as file:
                lines = file.readlines()
                distance = float(lines[1])
                if distance < min_dist:
                    min_dist = distance
                    min_path = lines[0]
                file.close()
    with open('communicate/best_path.txt', 'w') as f:
        f.write(min_path)
        f.write(str(min_dist))
        f.close()


if __name__ == '__main__':
    if platform.system() == "Linux":
        end_file = "out"
        start_file = "./"
    with open('communicate/multi_core_arg.txt', 'r') as f:
        lines = f.readlines()
        f.close()
    script = str(lines[0])
    print(script)
    if script == "myalgo\n":
        # subprocess.call(
        #   'g++ -std=c++11  ../algorithms/threeOpt.cpp ../algorithms/tools.cpp ../algorithms/myAlgo.cpp  -pthread -O3 -march=native -o ../GUI/a.' + end_file,
        #  shell=True)
        subprocess.call(start_file + 'a.' + end_file + ' 1', shell=True)
    else:
        # subprocess.call(
        #   'g++ -std=c++11 ../algorithms/colony.cpp ../algorithms/threeOpt.cpp ../algorithms/tools.cpp   -pthread -O3 -march=native -o ../GUI/c.' + end_file,
        #  shell=True)
        create_sub_process()
        with open('communicate/update.txt', 'w') as f:
            f.write("400")
