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
POINT_NUM = 750

MAX_POWER = 254

def X(n):
    return 10**(((n-1)/(253/3))-1)

# POINT_NUM 个点
n_values = np.linspace(85,MAX_POWER,POINT_NUM)

X_values = X(n_values)

# 创建图形
plt.figure(figsize=(10, 6))

line = list(n_values)
val = list(X_values)

print("*"*80)
print(list(map(int,val)))

#写入序列到文件
with open("py_DALI_Wave.c",'w',encoding= 'UTF-8') as f:
    # 先将每个值取整，再转换为字符串并组合为一个字符串
    int_values = ', '.join(map(lambda x: str(round(x)), val))  # 对每个值取整并转换为字符串
    f.write(int_values)  # 将结果写入文件
with open("py_DALI_Wave_hex.c",'w',encoding= 'UTF-8') as f:
    # 转换为十六进制并将其格式化为字符串，不带逗号、方括号和引号
    hex_values = ', '.join(map(lambda x: hex(int(x)), val))  # 生成格式化的字符串
    f.write(hex_values)  # 写入文件

# 绘制 X(n) 曲线
plt.plot(n_values, X_values, label=r'$X(n) = 10^{\left(\frac{n-1}{\frac{253}{3}} - 1\right)}$', color='b')
plt.title(r'Plot of the Function $X(n) = 10^{\left(\frac{n-1}{\frac{253}{3}} - 1\right)}$')
plt.xlabel('n')
plt.ylabel('X(n)')

# 添加图例
plt.legend()

# 显示网格
plt.grid(True)

# 显示图形
plt.show()