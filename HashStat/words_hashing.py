import os
import subprocess

executed_file = './mora_sponge'

line_of_start = 0
result_file = 'results.txt'

if os.path.isfile('results.txt'):
    with open(result_file, 'r') as f:
        line_of_start = sum(1 for line in f)

print(f'{line_of_start = }')

with open('words.txt', 'r') as words_file, open('results.txt', 'a') as results:
    for i, word in enumerate(words_file):
        if i > 3:
            break
        if i < line_of_start:
            continue

        p = subprocess.Popen(f'{executed_file} {word[:-1]}', stdout=subprocess.PIPE, shell=True)
        output, err = p.communicate()
        p_status = p.wait()

        print(output)
