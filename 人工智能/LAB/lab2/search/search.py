# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def expand(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (child,
        action, stepCost), where 'child' is a child to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that child.
        """
        util.raiseNotDefined()

    def getActions(self, state):
        """
          state: Search state

        For a given state, this should return a list of possible actions.
        """
        util.raiseNotDefined()

    def getActionCost(self, state, action, next_state):
        """
          state: Search state
          action: action taken at state.
          next_state: next Search state after taking action.

        For a given state, this should return the cost of the (s, a, s') transition.
        """
        util.raiseNotDefined()

    def getNextState(self, state, action):
        """
          state: Search state
          action: action taken at state

        For a given state, this should return the next state after taking action from state.
        """
        util.raiseNotDefined()

    def getCostOfActionSequence(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print("Start:", problem.getStartState())
    print("Is the start a goal?", problem.isGoalState(problem.getStartState()))
    """
    "*** YOUR CODE HERE ***"
    # from game import Directions
    # s = Directions.SOUTH
    # w = Directions.WEST
    # e = Directions.EAST
    # n = Directions.NORTH

    # print("Start:", problem.getStartState())
    # print("Is the start a goal?", problem.isGoalState(problem.getStartState()))
    # print("expand:", problem.expand(problem.getStartState())) # 返回(可以走的下一个位置坐标，方向，距离)
    # print("get_Action:", problem.getActions(problem.getStartState()))

    open_table = util.Stack()
    
    # return general_search(problem, open_table)
    return general_search(problem, open_table)
    # util.raiseNotDefined()

def my_dfs(problem, open_table):
    vis = []
    open_table.push([problem.getStartState(), []])
    # vis.append(problem.getStartState())
    # print(len(open_table))
    while not open_table.isEmpty():
        now_state, road_records = open_table.pop()

        if problem.isGoalState(now_state):
            # print(road_records)
            return road_records
        if now_state not in vis:
            vis.append(now_state)
            for next_state in problem.expand(now_state):
                print(next_state)
    
                open_table.push([next_state[0], road_records + [next_state[1]]])
        
        # next_states = problem.expand(now_state)
        # for next_state in next_states:
        #     if next_state[0] in vis:
        #         continue
        #     vis.append(next_state[0])

        #     # 正确写法
        #     open_table.push([next_state[0], road_records + [next_state[1]]])

            # 错误写法:可能是因为编译器理解为将road_records的实体存进去了，外部做修改，则存在open_table内部的实体也发生变化
            # road_records.append(next_state[1])
            # print('old:', road_records)
            # open_table.push([next_state[0], road_records])
            # road_records.pop()
            # print('new', road_records)
    return []


def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"
    open_table = util.Queue()
    return general_search(problem, open_table)
    # util.raiseNotDefined()

def my_bfs(problem, open_table):
    vis = []
    open_table.push([problem.getStartState(), []])
    vis.append(problem.getStartState())
    
    while not open_table.isEmpty():
        now_state, new_road_records = open_table.pop()
        if problem.isGoalState(now_state):
            return new_road_records
        next_states = problem.expand(now_state)
        for next_state in next_states:
            if next_state[0] in vis:
                continue
            vis.append(next_state[0])
            open_table.push([next_state[0], new_road_records + [next_state[1]]])

    return []

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"
    # 此处的估计函数已经给出是 heuristic，默认是没有估值(相当于没有启发式，直接就是爆搜)
    # open_table_test = util.PriorityQueue() # 由于要统一接口，原始的优先队列就不用了

    # lambda函数，用于给出优先队列排序的判据为 综合估计值f(n)，这样三种搜索的接口就统一了
    priorityFunction = lambda item: problem.getCostOfActionSequence(item[1]) + heuristic(item[0], problem)
    open_table = util.PriorityQueueWithFunction(priorityFunction)
    return general_search(problem, open_table)


def my_Astar(problem, open_table):
    vis = []
    open_table.push([problem.getStartState(), []])
    vis.append(problem.getStartState())
    
    while not open_table.isEmpty():
        now_state, new_road_records = open_table.pop()
        if problem.isGoalState(now_state):
            return new_road_records
        next_states = problem.expand(now_state)
        for next_state in next_states:
            if next_state[0] in vis:
                continue
            vis.append(next_state[0])
            open_table.push([next_state[0], new_road_records + [next_state[1]]])
    return []

def general_search(problem, open_table):
    vis = []
    open_table.push([problem.getStartState(), []])
    # vis.append(problem.getStartState())
    
    while not open_table.isEmpty():
        now_state, new_road_records = open_table.pop()
        if problem.isGoalState(now_state):
            return new_road_records

        if now_state not in vis:
            vis.append(now_state)
            for next_state in problem.expand(now_state):
                open_table.push([next_state[0], new_road_records + [next_state[1]]])

        # if now_state in vis:
        #     continue
        # vis.append(now_state)
        # next_states = problem.expand(now_state)
        # for next_state in next_states:
        #     if next_state[0] in vis:
        #         continue
        #     vis.append(next_state[0])
        #     open_table.push([next_state[0], new_road_records + [next_state[1]]])
    return []

# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
