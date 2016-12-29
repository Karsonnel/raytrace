
 
from PIL import Image
import random

def readfile(filename):
    pixsels = []
    f = open(filename, 'r', encoding='utf-8')
    while True:
        line = f.readline()[:-2]
        if not line:
            break
        line = line.split('\t')
        pixsels.append([int(float(x) * 255) for x in line])
    return pixsels, len(pixsels), len(pixsels[0])

r = readfile('newralpha.txt')
g = readfile('newgalpha.txt')
b = readfile('newbalpha.txt')
x , y = r[1], r[2]

c = Image.new("RGB",(y,x))
for i in range (0,x):
    for j in range (0,y):
        c.putpixel([j,i], (r[0][i][j], g[0][i][j], b[0][i][j]))

c.show()

c.save('test4.png')


