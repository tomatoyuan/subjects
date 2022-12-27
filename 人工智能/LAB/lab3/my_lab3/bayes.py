# @time:2022/10/16

from copy import copy

class Node(object):
    def __init__(self, name):
        '''
        初始化函数
            name: 节点名称
            fathers：父节点列表
            CPT：该节点的CPT表
                CPT表存储格式为:{
                    ((self.name, 'true/false'), (farher1, 'true/false'), ...): probality,
                    ...
                }
        '''
        self.name = name
        self.fathers = []
        self.CPT = {}

    def add_father(self, father):
        '''
        添加父节点
            注：这里添加的父节点是有顺序的，在之后计算 P((self.name, 'true/false)|(father1.name, 'true/false)...)概率的时候
            按照存储顺序，否则在CPT表中索引不到
        '''
        self.fathers.append(father)
    
    def self_parents_multi(self, node_val_tuple_list):
        '''
        计算 P((self.name, 'true/false)|parent(self.name))
            node_val_tuple_list: 是 **所有节点** 的 (节点,值) 列表
            注：参数的节点——值对列表是可无固定顺序的，因为会按照父节点列表的顺序取父节点，查找得到对应值，组成((节点，值),...)作为CPT的键进行索引
        '''
        _ , self_val = Node.search_node(node_val_tuple_list, self.name)
        # 如果父节点数量为0。可直接返回对应True or False的概率
        if len(self.fathers) == 0:
            # 注意这里必须为tuple([(self.name, self_val)])因为这样才会把单独的元组转换为列表再转换为元组
            # 如果不采取这种方式，那么索引的键会是((self.name, self_val))，但是索引的键应当是((self.name. self_val),)
            return self.CPT[tuple([(self.name, self_val)])]

        # 节点数量不为0，先按名称的字典序过滤出
        temp_lst = [(self.name, self_val)]
        for father in self.fathers:
            father, val = Node.search_node(node_val_tuple_list, father)
            temp_lst.append((father, val))
        return self.CPT[tuple(temp_lst)]

    @staticmethod
    def search_node(node_val_tuple_list, name):
        '''
        从节点——值对来索引name节点对应的值("true/false")
        参数是
            node_val_tuple_list:[(node, val(ture or false)), ...]
            name:要查找的节点名字
        '''
        for (node, val) in node_val_tuple_list:
            if node == name:
                return (node, val)


class Bayes_Net(object):
    '''
    贝叶斯网络
    '''
    def __init__(self, file):
        '''
        初始化，构建贝叶斯网络
        '''
        self.nodes = [] #定义节点列表，其内部存储的每个节点元素都是一个节点类变量
        self.num_nodes = 0  #记录节点个数


        with open(file, 'r', encoding='utf-8') as f:
            lines = [line.strip() for line in f.readlines()]
            lines = [line for line in lines if line != '']
        
        # 读入节点个数  
        self.num_nodes = int(lines[0])

        # 读入各个节点名称并添加至节点列表
        temp_index = 1
        names = []
        while lines[temp_index][0] < '0' or lines[temp_index][0] > '9':
            names += lines[temp_index].split()
            temp_index += 1
        # print("names = {}".format(names))
        assert len(names) == self.num_nodes
        for name in names:
            self.nodes.append(Node(name))
        del names

        # 解析节点网络
        line_index = temp_index
        for i in range(self.num_nodes):
            for j, val in enumerate(lines[line_index].split()):
                if val == '1':
                    self.nodes[j].add_father(self.nodes[i].name)
            line_index += 1

        # 为每个节点建立CPT表（仅关于每个节点与其父节点）
        for node in self.nodes:
            # print(node.name)
            father_num = len(node.fathers)
            # print(father_num)
            line_num = 2 ** father_num
            # print(line_num)
            for i in range(0, line_num):
                # print(line_index + i)
                b_str = bin(i).replace('0b','') #用二进制记录每个节点的父节点的组合
                # print(b_str)
                tmp_b_lst = list(b_str)
                while len(tmp_b_lst) < father_num: #将组合由0，1，10，11补全为00，01，10，11，方便下一步统计使用
                    tmp_b_lst.insert(0, '0')
                # print(tmp_b_lst)
                b_str = ''.join(tmp_b_lst)
                # print(b_str)
                tmp_lst = []
                for j, father in enumerate(node.fathers):
                    if b_str[j] == '0':
                        tmp_lst.append((father, 'false'))
                    else:
                        tmp_lst.append((father, 'true'))
                f_tmp_lst = copy(tmp_lst)
                # print(f_tmp_lst)
                tmp_lst.insert(0, (node.name, 'true'))
                f_tmp_lst.insert(0, (node.name, 'false'))
                # print('tmp_list = {}'.format(tmp_lst))
                # print('f_temp_list = {}'.format(f_tmp_lst))
                #从读入中取出该节点为true/false时，其与父节点当前组合方式的概率，加入CPT表
                node.CPT[tuple(tmp_lst)] = float(lines[line_index + i].split()[0])
                node.CPT[tuple(f_tmp_lst)] = float(lines[line_index + i].split()[1])
                # print(node.CPT)
            line_index += line_num
        
        # debug: 输出所有节点的父节点及CPT表
        # for node in self.nodes:
        #     print(node.name)
        #     print(node.fathers)
        #     for key, value in node.CPT.items():
        #         print(key,'   ',value)
        #     print()


    def _parse_query(self, query_file):
        '''
        解析query文件，获得queries
            queries格式为((node1, 'true/false'), (node2, 'true/false'), ...)，其中元组的第一项为非条件部分的节点，剩余部分为条件部分的节点
        '''
        with open(query_file, 'r') as f:
            lines = f.readlines()
        queries = []
        for line in lines:
            line = line.strip().replace('P(', '')
            line = line.replace(')', '')
            line = line.replace('|', '')
            line = line.replace(',', ' ')
            line = line.split()
            # print(line)
            if not len(line):
                continue
            q1 = []
            q2 = []
            q1.append((line[0],'false'))
            q2.append((line[0],'true'))
            for i in range(1, len(line)):
                tmp = line[i].split('=')
                q1.append((tmp[0],tmp[1]))
                q2.append((tmp[0],tmp[1]))
            queries.append(tuple(q1))
            queries.append(tuple(q2))
        return queries

    def compute_queries(self, query_file, mode=0, times=10):
        '''
        计算query_file中所有query对应的值
        mode==0 : 求精确解
        mode==1 : 求近似解
        times   : 求近似解时的迭代次数
        '''
        queries = self._parse_query(query_file)
        
        if mode == 1:
            query_flag = 1
            for query in queries:
                if query_flag == 0:
                    query_flag = 1
                    continue
                # print(query)
                query_ans_true = 0
                query_ans_false = 0
                vaild_ans_tot = 0
                from tqdm import tqdm
                for i in tqdm(range(times)):
                    ans = self.approximate_queries(query)
                    if ans == 1:
                        query_ans_true += 1
                    elif ans == 2:
                        query_ans_false += 1
                    vaild_ans_tot = query_ans_true + query_ans_false
                # print(ans)
                if vaild_ans_tot == 0:
                    query_ans_true = 0
                    query_ans_false = 1
                    vaild_ans_tot = 1
                print('P(',query[0][0],'true','|',query[1:],')=',str(query_ans_true / vaild_ans_tot))
                print('P(',query[0][0],'flase','|',query[1:],')=',str(query_ans_false / vaild_ans_tot))
                query_flag = 0
        elif mode == 0:
            for query in queries:
                print('P(',query[0],'|',query[1:],')=',str(self._compute_one(query)))
                # print(query, str(self._compute_one(query)))

    def approximate_queries(self, query):
        """
        求解近似解
        """
        from queue import Queue
        import numpy as np
        my_rate = 1.0
        node_queue = Queue(100)
        ind = [0 for i in range(self.num_nodes)] # 入度
        vis = [0 for i in range(self.num_nodes)] # 记录是否在队列中
        # 记录一轮拓扑排序获得的结果，即每个节点对应的true/false最后用于与询问的条件比对
        now_topological_sort = {}
        # 统计入度
        for i, node in enumerate(self.nodes):
            ind[i] = len(node.fathers)
            if ind[i] == 0:
                node_queue.put(node)
                vis[i] = 1
        # 获得拓扑序
    
        while not node_queue.empty():
            node = node_queue.get()
            # print("now_node = {0} fathers = {1}".format(node.name, node.fathers))
            
            temp_order_list = [(node.name, 'true')]
            for node_f in node.fathers:
                if now_topological_sort[node_f] == 'false':
                    temp_order_list.append((node_f, 'false'))
                else:
                    temp_order_list.append((node_f, 'true'))
            # print(now_topological_sort)
            # 字典中添加拓扑序中根据随机概率获得的每个节点的true/false
            # print(temp_order_list)
            # print('rate:{}'.format(self._compute_one((temp_order_list))))
            
            # print("new")
            # print(temp_order_list)
            # print(node.CPT[tuple(temp_order_list)])

            # if np.random.random() <= self._compute_one((temp_order_list)):
            if np.random.random() <= node.CPT[tuple(temp_order_list)]:
                now_topological_sort[node.name] = 'true'
            else:
                now_topological_sort[node.name] = 'false'

            # print(node.CPT)
            # 扩展下一个节点
            for i, next_node in enumerate(self.nodes):
                if not vis[i]:
                    if node.name in next_node.fathers:
                        ind[i] -= 1
                    if ind[i] == 0:
                        node_queue.put(next_node)
                        vis[i] = 1
        # print(query)
        # print(query[1:])
        # print(now_topological_sort[query[0][0]], query[0][1])
        # print('re = {}'.format(now_topological_sort))
        for q_node, val in query[1:]:
            if now_topological_sort[q_node] != val:
                return 0
        if now_topological_sort[query[0][0]] == query[0][1]:
            return 1
        else:
            return 2

    def _compute_one(self, query):
        '''
        计算单个query，计算方法为通过贝叶斯公式将条件概率转换为联合概率的除法，然后调用计算联合概率的方法求解
        P(A|BC) = P(A,B,C)/P(B,C)
        '''
        # print(len(query))
        # print(query[1:])
        if len(query) == 1:
            return self._compute_lack(list(query))
        query = list(query)
        return self._compute_lack(query) / self._compute_lack(query[1:])
        
    
    def _compute_lack(self, node_val_tuple_list):
        '''
        计算联合概率，扩展为所有节点全联合概率的和
        '''
        lack_nodes = [node.name for node in self.nodes \
                        if (node.name, 'true') not in node_val_tuple_list \
                        and (node.name, 'false') not in node_val_tuple_list]
        # print('lack{}'.format(lack_nodes))
        result = []
        result.append(node_val_tuple_list)
        tmp_len = len(result)

        for node in lack_nodes:
            while tmp_len > 0:
                tmp_len -= 1
                # print("result_before = {}".format(result))
                a_tmp = copy(result.pop(0)) # 每次从节点的CPT表中取出第一行，添加没有涉及到的节点，将
                # print("a_tmp = {}".format(a_tmp))
                b_tmp = copy(a_tmp)
                a_tmp.append((node, 'true'))
                b_tmp.append((node, 'false'))
                result.append(a_tmp)
                result.append(b_tmp)
                # print("result_after = {}".format(result))
            tmp_len = len(result)
        assert len(result) == (2 ** len(lack_nodes))
        # print('result : \n{}'.format(result))
        return sum([self._compute_filled(tmp) for tmp in result])
    
    def _compute_filled(self, node_val_tuple_list):
        '''
        计算所有节点全联合概率，利用公式P(x1, x2, x3, ... ,xn) = ∏(i:1->n)P(xi|Parent(xi))
        '''
        value = 1.0
        for node_name, val in node_val_tuple_list:
            node = self._search_node_by_name(node_name)
            tmp = node.self_parents_multi(node_val_tuple_list)
            value = value * tmp
        return value
            
    def _search_node_by_name(self, node_name):
        '''
        通过节点名称查找节点
        '''
        for node in self.nodes:
            if node.name == node_name:
                return node


if __name__ == '__main__':
    network_file = './burglarnetwork.txt'
    query_file = './burglarqueries.txt'
    network_file = './carnetwork.txt'
    query_file = './carqueries.txt'
    b = Bayes_Net(network_file)
    b.compute_queries(query_file, mode=1, times=100000)