#! python3
#coding=utf-8

"""
Python版本：3.x
外部库：matplotlib1.5.3、numpy1.11.2

运行方式：
在命令行中输入：python index_wave.py 

运行结果：
命令行中会打印计算得的各点数据，
在当前目录下会生成py_index_wave.c文件，包含上述数据，
并且会弹出描绘曲线的对话框。
"""

import matplotlib.pyplot as plt 
import numpy as np
import math

#修改本变量可以更改点数，如16、32、64等
POINT_NUM = 40

#指数曲线最大为2的MAX_POWER次方
MAX_POWER = 16


# POINT_NUM 个点
x1 = np.linspace(0,MAX_POWER,int(POINT_NUM/2))

#f = 2^(x)
up =[]
for i in x1:
    temp = round(2**i)
    #得到升序列
    up.append( temp )
   


x2 = np.linspace(MAX_POWER,2*MAX_POWER,int(POINT_NUM/2))

#f = 2^(2*MAX_POWER-x)
down=[]
for i in x2:
    temp = round(2**(MAX_POWER*2-i))

    #得到降序列
    down.append( temp )


# 合并升序和降序的点
line = list(x1)+list(x2)
val = list(up)+list(down)

#最大值减一防止溢出
max_val = max(val)
for i in range(len(val)):
    if val[i] == max_val:
        val[i] = val[i] - 1

print(line)
print("*"*80)
print(list(map(int,val)))

#写入序列到文件
with open("py_exponent_Wave.c",'w',encoding= 'UTF-8') as f:
    print(list(map(int,val)),file= f)
with open("py_exponent_Wave_hex.c",'w',encoding= 'UTF-8') as f:
    print(list(map(lambda x: hex(int(x)), val)),file= f)

#绘图
plt.plot(line,val,"-o")
plt.show()



