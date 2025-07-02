#! python3
#coding=utf-8

"""
Python版本：3.x
外部库：matplotlib1.5.3、numpy1.11.2

运行方式：
在命令行中输入：python sinWave.py 

运行结果：
命令行中会打印计算得的各点数据，
在当前目录下会生成py_dac_sinWav.c文件，包含上述数据，
并且会弹出描绘曲线的对话框。
"""

import matplotlib.pyplot as plt 
import numpy as np
import math

#修改本变量可以更改点数，如16、32、64等
POINT_NUM =  12

#正弦函数放大倍数
MAX_POWER = 8
amplitude = 2**MAX_POWER

#正弦函数X取值范围
X_VALUE = (np.pi)/2

# 计算升序的正弦波
x1 = np.linspace(0, X_VALUE, int(POINT_NUM/2))
sin_up = np.sin(x1) * amplitude
sin_up_rounded = np.round(sin_up).astype(int)
sin_up_list = sin_up_rounded.tolist()

# 计算降序的正弦波
x2 = np.linspace(X_VALUE, X_VALUE*2, int(POINT_NUM/2))
sin_down = np.sin(x2) * amplitude
sin_down_rounded = np.round(sin_down).astype(int)
sin_down_list = sin_down_rounded.tolist()

# 合并升序和降序的点
line = list(x1)+list(x2)
val = sin_up_list + sin_down_list

#最大值减一防止溢出
max_val = max(val)
for i in range(len(val)):
    if val[i] == max_val:
        val[i] = val[i] - 1


print(line)
print("*"*80)
print(list(map(int,val)))


#写入序列到文件
with open("py_pwm_sinWave.c",'w',encoding= 'UTF-8') as f:
    # 先将每个值取整，再转换为字符串并组合为一个字符串
    int_values = ', '.join(map(lambda x: str(round(x)), val))  # 对每个值取整并转换为字符串
    f.write(int_values)  # 将结果写入文件
with open("py_pwm_sinWave_hex.c",'w',encoding= 'UTF-8') as f:
    # 转换为十六进制并将其格式化为字符串，不带逗号、方括号和引号
    hex_values = ', '.join(map(lambda x: hex(int(x)), val))  # 生成格式化的字符串
    f.write(hex_values)  # 写入文件

#绘图
plt.plot(line,val,"-o")
plt.show()