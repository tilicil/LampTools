##########################################################
# 1.将Excel表格中的 ARR 行复制到 map
# 2.将Excel表格中的 step 行复制到 anime_input_arr
# 3.右键run code运行，复制生成的动画数组 
##########################################################

map_arr = '''
{	21,	20,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17,	18,	19,	1,	0,	22,	23,}

'''

anime_input_carr = '''{
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},
{	0,	0,	0,	0,	0,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0xff,	0,	0,},

}'''

def carr_transto_pyarr(c_arr):
    """
    将C风格的数组转换为Python列表
    :param c_arr: C风格的数组
    :return: Python列表
    """
    import ast
    cleaned = c_arr.strip()
    formatted = cleaned.replace("{", "[").replace("}", "]")
    py_arr = ast.literal_eval(formatted)
    return py_arr

def reorder_anime_input(anime_input_arr, map_arr):
    """
    重新排序LED动画输入数据
    :param anime_input_arr: 原始动画数据数组[ANIME_STEP_NUM][LED_NUM]
    :param map_arr: LED映射数组,定义新的LED顺序
    :return: 重新排序后的动画数据
    """
    ANIME_STEP_NUM = len(anime_input_arr)
    LED_NUM = len(anime_input_arr[0]) if ANIME_STEP_NUM > 0 else 0
    
    reordered_anime_input_arr = [[0] * LED_NUM for _ in range(ANIME_STEP_NUM)]
    
    for step in range(ANIME_STEP_NUM):
        for i in range(LED_NUM):
            reordered_anime_input_arr[step][i] = anime_input_arr[step][map_arr[i]]
    
    return reordered_anime_input_arr

if __name__ == "__main__":
    # 转换为Python列表
    py_anime_arr = carr_transto_pyarr(anime_input_carr)
    py_map_arr = carr_transto_pyarr(map_arr)
    result = reorder_anime_input(py_anime_arr, py_map_arr)
    
    # 输出到.h文件
    with open("anime_output_dec.h", "w") as f:
        f.write("#ifndef ANIME_OUTPUT_H\n")
        f.write("#define ANIME_OUTPUT_H\n\n")
        f.write("const unsigned char anime_output_arr[][] = {\n")
        for step in result:
            f.write(f"    {{{', '.join(map(str, step))}}},\n")
        f.write("};\n\n")
        f.write("#endif // ANIME_OUTPUT_H\n")
    
    # 输出到.h文件
    with open("anime_output_hex.h", "w") as f:
        f.write("#ifndef ANIME_OUTPUT_H\n")
        f.write("#define ANIME_OUTPUT_H\n\n")
        f.write("const unsigned char anime_output_arr[][] = {\n")
        for step in result:
            # 将每个值转换为16进制格式 (0xXX)
            hex_values = [f"0x{val:02X}" for val in step]
            f.write(f"    {{{', '.join(hex_values)}}},\n")
        f.write("};\n\n")
        f.write("#endif // ANIME_OUTPUT_H\n")

    print("success")