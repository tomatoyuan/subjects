import numpy as np
import matplotlib.pyplot as plt
import random

# 符号函数
def sign(v):
    if v >= 0:
        return 1
    else:
        return -1

def train(train_num, train_data, lr=0.5):  # 原始训练部分
    w = [1., 1.]  # 初始化权重向量和偏置
    b = -1
    for i in range(train_num):
        #x = random.choice(train_data)
        for x in train_data:
            x1, x2, y = x
            if y * sign(x1 * w[0] + x2 * w[1] + b) <= 0:
                w[0] += lr * y * x1
                w[1] += lr * y * x2
                b += lr * y
                print(w,b)
    return w, b

# 画散点图
def plot_points(train_data, w, b):
    plt.figure()
    x1 = np.linspace(-1.5, 1.5, 100)
    x2 = (-b - w[0] * x1) / w[1]  # 化简 w1*x1 + w2*x2 + b =0 此时x2相当于竖轴坐标
    plt.plot(x1, x2, color='r', label='y1 data')
    datas_len = len(train_data)
    for i in range(datas_len):
        if train_data[i][-1] == 1:
            plt.scatter(train_data[i][0], train_data[i][1], s=50)
        else:
            plt.scatter(train_data[i][0], train_data[i][1], marker='x', s=50)
    plt.show()

if __name__ == '__main__':
    # train_data1 = [[-0.30,0.96,1],[1.49,0.25,1],[0.78,0.96,1]]  # 正样本
    # train_data2 = [[-0.89,-1.62,-1], [0.06,-0.68,-1],[-1.13,0.14,-1]]  # 负样本
    train_data1 = [[3,3,1],[4,3,1]]  # 正样本
    train_data2 = [[1,1,-1]]  # 负样本
    data_set = train_data1 + train_data2  # 样本集
    w, b = train(100, data_set, 0.5)  # lr 学习率
    plot_points(data_set, w, b)
