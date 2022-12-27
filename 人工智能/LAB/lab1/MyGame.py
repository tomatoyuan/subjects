
from audioop import mul


class MyGame:
    '''
    类的功能:
        添加猴子的状态，动作
        添加香蕉的状态
        添加箱子的状态
    '''
    def __init__(self, monkey_start_pos, box_start_pos, banana_start_pos, on_bool=0, hang_bool=1):
        '''
        传入参数：
            monkey_start_pos    :   (x1, y1) 猴子初始位置
            box_start_pos       :   (x2, y2) 箱子初始位置
            banana_start_pos    :   (x3, y3) 香蕉初始位置
        初始化状态: [(x1, y1), (x2, y2), (x3, y3), on_bool, hang_bool]
            (x, y)  :   坐标
            on_bool :   0代表在地上，1代表在箱子上
            hang_bool:  0代表香蕉被猴子摘下，1代表香蕉悬挂
        '''
        self.system_start_state = [monkey_start_pos, box_start_pos, banana_start_pos, on_bool, hang_bool]
        self.system_now_state = [monkey_start_pos, box_start_pos, banana_start_pos, on_bool, hang_bool]
        self.system_end_state = [banana_start_pos, banana_start_pos, banana_start_pos, 1, 0]

    def is_end(self):
        '''
        函数功能：
            判断游戏是否结束(即猴子成功摘到香蕉)
        '''
        if self.system_now_state == self.system_end_state:
            print('摘桃成功！')
            return True
        else:
            print('革命尚未成功哦！')
            return False

    def monkey_move(self, to_pos):
        '''
        函数功能：
            猴子移动
        '''
        now_monkey_pos = self.system_now_state[0]
        self.system_now_state[0] = to_pos
        print("猴子   :{0}->{1}".format(now_monkey_pos, to_pos))

    def monkey_push(self, to_pos):
        '''
        函数功能：
            猴子推箱子移动
        '''
        if self.system_now_state[0] != self.system_now_state[1]:
            print('猴子推不着箱子！！！')
        elif self.system_now_state[3] == 1:
            print('在箱子上是没办法推箱子的哦~')
        else:
            now_pos = self.system_now_state[0]
            self.system_now_state[0] = to_pos
            self.system_now_state[1] = to_pos
            print("猴子推箱子：{0}->{1}".format(now_pos, to_pos))

    def jump_on(self):
        '''
        函数功能：
            猴子跳上箱子
        '''
        if self.system_now_state[0] == self.system_now_state[1] and self.system_now_state[3] == 0:
            self.system_now_state[3] = 1
            print("猴子跳上了箱子~")
        elif self.system_now_state[0] == self.system_now_state[1] and self.system_now_state[3] == 1:
            print('别闹~ 你想大闹天宫吗？')
        else:
            print('猴子周围没有箱子喔*.*')

    def jump_down(self):
        '''
        函数功能：
            猴子跳下箱子
        '''
        if self.system_now_state[3] == 1:
            self.system_now_state[3] = 0
            print("猴子从箱子上掉了下来-.-")
        else:
            print('你不会想钻到地里去吧……')

    def pick(self):
        '''
        函数功能：
            猴子摘下桃子
        '''
        if self.system_now_state[3] == 0:
            print('跳的不够高啊！')
        elif self.system_now_state[2] == self.system_now_state[0] and self.system_now_state[4] == 1:
            # 成功摘到桃子
            self.system_now_state[4] = 0
            # self.is_end()
        elif self.system_now_state[2] != self.system_now_state[0]:
            print('箱子都推到你姥姥家了，请推到正确的位置！')
        elif self.system_now_state[2] == self.system_now_state[0] and self.system_now_state[4] == 0:
            print('已经没有桃子啦，来年见。')

def run_game(my_game):
    '''
    函数功能：
        从键盘获取指令，运行猴子摘香蕉的过程
    '''
    print('游戏开始：当前的游戏状态如下')
    print('[monkey{0},box{1},banana{2},monkey_on={3},banana_hang={4}'.format(
        my_game.system_start_state[0],  my_game.system_start_state[1],  my_game.system_start_state[2], 
        my_game.system_start_state[3],  my_game.system_start_state[4],  
    ))
    while True:
        action = input('请输入操作方式: \n'
                        '1. monkey_move\n'
                        '2. monkey_push\n'
                        '3. jump_on\n'
                        '4. jump_down\n'
                        '5. pick\n')
        # print(action)
        if action == '1':
            print('请输入猴子要去的位置：(x, y):')
            s = input('不需要输入括号，x和y之间用逗号(英文半角)隔开\n')
            s_list = list(s.split(','))
            to_pos = [int(s_list[0]), int(s_list[1])]
            # print(to_pos)
            my_game.monkey_move(to_pos)

        elif action == '2':
            if my_game.system_now_state[3] == 1:
                print('在箱子上是没办法推箱子的哦~')
                continue
            
            print('请输入猴子要把箱子推去哪里：(x, y):')
            s = input('不需要输入括号，x和y之间用逗号(英文半角)隔开\n')
            s_list = list(s.split(','))
            to_pos = [int(s_list[0]), int(s_list[1])]

            my_game.monkey_push(to_pos)

        elif action == '3':
            my_game.jump_on()

        elif action == '4':
            my_game.jump_down()

        elif action == '5':
            my_game.pick()
            
        else:
            print('可不敢胡乱操作*_*')
            continue
        # 每次操作完检查是否完成摘桃任务
        if my_game.is_end():
            break

if __name__ == "__main__":
    print('输入提示：')
    print('进入游戏，输入时不需要加括号，需要几个变量就输入几个，变量之间用;隔开')
    print('输入示例：       10;1;2;2;3;3;0;1')
    print('相当于输入了：   [[10, 100], [2, 2], [3, 3], 0, 1]')
    print('\n')
    s = input('请输入状态: 猴子位置(x, y),箱子位置(x, y),香蕉位置(x, y), 猴子是否在箱子上(是1/否0),香蕉是否悬挂(是1/否0)\n')
    system_state = list(s.split(';'))
    monkey_start = [int(system_state[0]), int(system_state[1])]
    box_start = [int(system_state[2]), int(system_state[3])]
    banana_start = [int(system_state[4]), int(system_state[5])]
    on_start = int(system_state[6])
    hang_start = int(system_state[7])
    my_game = MyGame(monkey_start, box_start, banana_start, on_start, hang_start)
    run_game(my_game)
