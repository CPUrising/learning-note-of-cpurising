# 接收用户输入，去除首尾空格后按空格分割成单词列表
words = input().strip().split()

# 去重：转换为集合后转回列表
unique_words = list(set(words))

# 按字母数字排序
sorted_words = sorted(unique_words)

# 拼接成空格分隔的字符串并打印
print(' '.join(sorted_words))