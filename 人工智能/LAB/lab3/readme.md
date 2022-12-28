[TOC]

# 实验三学习笔记

## Bayes学习

Bayes基础知识 [从朴素贝叶斯分类器到贝叶斯网络（下） - 走看看 (zoukankan.com)](http://t.zoukankan.com/wzzkaifa-p-7401175.html)

wikipedia [贝叶斯网络 - 维基百科，自由的百科全书 (wikipedia.org)](https://zh.wikipedia.org/zh-my/貝氏網路#精確推論)

Bayes中概率的求解方式：

![image-20221227230717029](readme/image-20221227230717029.png)

![image-20221227230726469](readme/image-20221227230726469.png)

* **1） 利用与朴素贝叶斯相似方法来进行推理（当中相同用到贝叶斯公式），我们称其为枚举法；**

![image-20221227230730118](readme/image-20221227230730118.png)

* **2）其次是一种更为经常使用的算法。我们称之为消去法；**

![image-20221227230732979](readme/image-20221227230732979.png)

![image-20221227230737329](readme/image-20221227230737329.png)

* **3）最后另一种基于蒙特卡洛法的近似推理方法**
  * Monte Carlo是估算，计算的结果往往是错误的，但是很接近真实值，机器学习中只需要一个低精度的近似解即可。

* [ ] 利用Monte Carlo估算一元定积分(一维均匀随机抽样)

![image-20221227230740333](readme/image-20221227230740333.png)

* [ ] 利用Monte Carlo估算多元定积分（二维均匀随机抽样）

![image-20221227230744274](readme/image-20221227230744274.png)

* [ ] 利用Monte Carlo算法估算期望(按照概率密度随机抽样)

![image-20221227230746902](readme/image-20221227230746902.png)

## 函数功能查询

* **strip**

```python
Python strip() #方法用于移除字符串头尾指定的字符（默认为空格或换行符）或字符序列。

str = "00000003210Runoob01230000000"; 
print str.strip( '0' );  # 去除首尾字符 0
 
 
str2 = "   Runoob      ";   # 去除首尾空格
print str2.strip();
```



- **enumerate**

```python
>>> seasons = ['Spring', 'Summer', 'Fall', 'Winter']
>>> list(enumerate(seasons))
[(0, 'Spring'), (1, 'Summer'), (2, 'Fall'), (3, 'Winter')]
>>> list(enumerate(seasons, start=1))       # 下标从 1 开始
[(1, 'Spring'), (2, 'Summer'), (3, 'Fall'), (4, 'Winter')]
```

- **bin**

```python
>>>bin(10)
'0b1010'
>>> bin(20)
'0b10100'
```

- **insert**

```python
aList = [123, 'xyz', 'zara', 'abc']
aList.insert( 3, 2009)
print "Final List : ", aList

# 输出结果如下
Final List : [123, 'xyz', 'zara', 2009, 'abc']
```

* .jion()

**join()方法语法：**

str.join(sequence)
**参数**
sequence -- 要连接的元素序列。
**返回值**
返回通过指定字符连接序列中元素后生成的新字符串。

```python
symbol = "-";
seq = ("a", "b", "c"); # 字符串序列
print symbol.join( seq );

// 输出为
a-b-c
```

* **assert**断言函数

![image-20221227230754606](readme/image-20221227230754606.png)

```python
>>> assert 1==2, '1 不等于 2'
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AssertionError: 1 不等于 2
```





## 实现原理

**举例说明**

<font color=green size=5>目标是求出</font>
$$
P(X|A,B)
$$
**基础知识准备**

由Bayes公式
$$
P(A|B) = \frac{P(B|A)*P(A)}{P(B)}
$$
由联合概率分布公式
$$
P(A,B)= P(A|B)*P(B)
$$
推广一下

<br>
$$
P(X_1,X_2,...,X_n) = P(X_1|X_2,...,X_n)*P(X_2|X_3,..,X_n)*...*P(X_n)
$$
<br>

当$X_1,X_2,X_3,...,X_n$之间相互影响时，有上式

如果$X_1$和$X_2$独立，那么上面得公式中
$$
P(X_1|X_2,...,X_n) = P(X_1|X_3,...,X_n)
$$
<font color=red>上式是我们通过Bayes网络中的先验概率求得联合概率的关键</font>

Bayes网络就是一个有向无环图，每个节点被指向它的节点影响，而和无法相互到达的节点相互独立



![image-20221227230804902](readme/image-20221227230804902.png)

上图右一中，$X$和$Z$不是相互联系的，但是当$Y$的值确定下来后，$X$和$Z$相互独立。再Bayes网络中，每个节点的CPT都是可求的，再求到字节点的CPT之前，其父节点的CPT已经变成已知，于是可以认为每个节点只受其直接父节点的影响。

这样求解某个节点事件的发生概率就变成了，求在其直接父节点发生的情况下，该节点事件发生的概率

即Bayes网络中的事件联合概率分布可以使用如下方法求解

<br>
$$
P(A,B,C,..) = P(A|A's\space fathers)*P(B|B's\space fathers)*P(C|C's\space fathers)*...
$$
于是Bayes公式可以简化为

<br>
$$
P(A|B) = \frac{P(B|A)*P(A)}{P(B)} = \frac{P(A,B)}{P(B)} = \frac{P(A,B)}{P(B|A)+P(B|\neg A)}
$$
<font color=green>目标式可以转化为</font>

<br>
$$
P(X|A,B)=\frac{P(X,A,B)}{P(A,B)} = \frac{P(X|X's\space fathers)*P(A|A's\space fathers)*P(B|B's\space fathers)}{P(A|A's\space fathers)*P(B|B's\space fathers)}
$$
<br>

如果网络中还有事件$C、D$，计算联合概率时，带入一同计算即可（由于这里建立的CPT表是一张包含所有组合情况的表）

<br>
$$
P(X|A,B)=\frac{P(X,A,B,C,D)}{P(A,B,C,D)}+\frac{P(X,A,B,\neg C,D)}{P(A,B,\neg C,D)}+\frac{P(X,A,B,C,\neg D)}{P(A,B,C,\neg D)}+\frac{P(X,A,B,\neg C,\neg D)}{P(A,B,\neg C,\neg D)}
$$
利用上式进行Bayes网络对特定事件结果的预测

## PPT讲解

### 1. 枚举法(Enumeration)

> 将目标节点、证据节点和隐藏节点全部加入CPT表，得到一张枚举出所有情况的巨大CPT表。
>
> 预测时，直接去CPT表中找结果即可。

![image-20221227230810039](readme/image-20221227230810039.png)

![image-20221227230812759](readme/image-20221227230812759.png)

![image-20221227230816291](readme/image-20221227230816291.png)

### 2. 变量消元法(Eliminate)

![image-20221227230819361](readme/image-20221227230819361.png)

![image-20221227230821966](readme/image-20221227230821966.png)

![image-20221227230826035](readme/image-20221227230826035.png)

<font color=red size=4>把所有的事件加入CPT后再进行变量消除，无异于枚举</font>

![image-20221227230828953](readme/image-20221227230828953.png)

<font color=green size=4>尽早合并、消除变量才是正确的消元法</font>

**正确的消元法演示如下图右一**

![image-20221227230831699](readme/image-20221227230831699.png)

![image-20221227230836113](readme/image-20221227230836113.png)

**消元法使用过程演示(当起始元素是证据节点时，只需要考虑该变量的一种已知取值即可)**

![image-20221227230838636](readme/image-20221227230838636.png)

![image-20221227230841145](readme/image-20221227230841145.png)

**消元法的使用步骤总结**

![image-20221227230843767](readme/image-20221227230843767.png)

**消元法使用举例**

![image-20221227230846656](readme/image-20221227230846656.png)



### 3. 采样近似求解（Sampling）

> 之所以需要使用采样求近似解，是因为当CPT网络非常大时，求精确解的代价太高了

![image-20221227230850532](readme/image-20221227230850532.png)

#### 1. Prior Sampling（先验抽样）

从起始节点随机一个$value(True/False)$，之后逐步随机选择$CPT$中合理的值，直到得出目标值，根据目标值出现$True/False$的统计次数，求出平均概率。

![image-20221227230853945](readme/image-20221227230853945.png)

![image-20221227230857402](readme/image-20221227230857402.png)

![image-20221227230859782](readme/image-20221227230859782.png)



#### 2. Rejection Sampling（拒绝抽样）

对于不符合预期的采样结果，选择拒绝采样

![image-20221227230902330](readme/image-20221227230902330.png)

 ![image-20221227230906016](readme/image-20221227230906016.png)

#### 3. Likelihood Weighting（似然加权）

如果某个证据节点的出现概率很小，就会导致大量的拒绝采样

解决方法：固定该证据样本，随机选择其余的样本

![image-20221227230908410](readme/image-20221227230908410.png)

固定选择证据节点$S$和$W$，为+，其余的进行随机选择，得到的目标概率如下

![image-20221227230910838](readme/image-20221227230910838.png)

![image-20221227230912974](readme/image-20221227230912974.png)

>上述近似采样的伪代码如下

```python
IN: evidence instantiation
w = 1.0
for i=1, 2, …, n
    if Xi is an evidence variable
    	Xi = observation xi for Xi
    	Set w = w * P(xi | Parents(Xi))
    else
		Sample xi from P(Xi | Parents(Xi))
return (x1, x2, …, xn), w
```



似然加权虽好，但是上层的抽样结果会影响到下层的选择，但是下层的选择影响不到上层的选择，最顶端C可能抽取不到正确的选择。

![image-20221227230916629](readme/image-20221227230916629.png)

于是，我们需要吉布斯采样来解决这个问题。

#### 4. Gibbs Sampling（吉布斯采样）

 ![image-20221227230918656](readme/image-20221227230918656.png)

 ![image-20221227230921478](readme/image-20221227230921478.png)

 ![image-20221227230926697](readme/image-20221227230926697.png)



## 代码实现

见 `my_lab3`

