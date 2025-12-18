import os

# 获取当前代码文件的绝对路径
script_path = os.path.abspath(__file__)
# 获取代码文件所在的目录
script_dir = os.path.dirname(script_path)
# 切换工作目录到代码所在目录
os.chdir(script_dir)
# 1. 读取并解析数据
student_data = []
with open("data.txt", "r", encoding="utf-8") as f:
    lines = f.readlines()
    # 跳过表头，处理每一行数据
    for line in lines[1:]:
        # 去除换行符并按逗号分割
        parts = line.strip().split(",")
        name = parts[0]
        # 提取各科成绩并转换为整数
        chinese = int(parts[1])
        math = int(parts[2])
        english = int(parts[3])
        science = int(parts[4])
        # 计算总分
        total = chinese + math + english + science
        student_data.append({"name": name, "total": total})

# 2. 按总分降序排序
student_data.sort(key=lambda x: x["total"], reverse=True)

# 3. 写入结果到result.txt
with open("result.txt", "w", encoding="utf-8") as f:
    # 写入表头
    f.write("名次,姓名,总分\n")
    # 遍历排序后的数据，生成名次并写入
    for rank, student in enumerate(student_data, start=1):
        f.write(f"{rank},{student['name']},{student['total']}\n")

print("排序完成，结果已保存到result.txt")