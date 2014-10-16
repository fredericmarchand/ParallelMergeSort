import random
import sys

n = int(sys.argv[1])
inputFile = sys.argv[2]
outputFile = sys.argv[3]
x = []

f = open(inputFile, 'w')
f.write(str(n) + '\n')
for i in range(n):
    y = random.randint(1, 100000)
    x.append(y)
    f.write(str(y) + '\n')
f.close()

f = open(outputFile, 'w')
f.write(str(n) + '\n')
x.sort()
for y in x:
    f.write(str(y) + '\n')
f.close()
