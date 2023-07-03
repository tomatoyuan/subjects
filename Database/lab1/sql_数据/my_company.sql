-- 创建数据库
CREATE DATABASE company;

-- 显示数据库
SHOW databases;

USE company;

CREATE TABLE employee(
    ename       varchar(12),
    essn        char(18),
    address     text,
    salary      int,
    superssn    char(18),
    dno         char(3),
    primary key(essn)
);

SHOW tables;

DESCRIBE employee;

CREATE TABLE department(
    dname        varchar(15),
    dno          char(3),
    mgrssn       char(18),
    mgrstartdate date,
    primary key(dno)
);

CREATE TABLE project(
    pname       varchar(3),
    pno         char(2),
    plocation   text,
    dno         char(3),
    primary key(pno)    
);

CREATE TABLE works_on(
    essn        char(18),
    pno         char(2),
    hours       int,
    primary key(essn, pno)
);

SHOW TABLES;

-- 导入数据之前要重新进入数据库，以如下方式
mysql --local-infile=1 -u root -p

SHOW GLOBAL VARIABLES LIKE "local_infile";

SET GLOBAL local_infile=true;

SHOW GLOBAL VARIABLES LIKE "local_infile";

USE company;

-- 导入数据前先清空
DELETE FROM employee;
DELETE FROM department;
DELETE FROM project;
DELETE FROM works_on;

LOAD DATA LOCAL INFILE "F:\\Awellfrog\\subjects\\Database\\lab1\\lab1_data\\employee.txt" 
                INTO TABLE employee;
LOAD DATA LOCAL INFILE "F:\\Awellfrog\\subjects\\Database\\lab1\\lab1_data\\department.txt"
                INTO TABLE department;
LOAD DATA LOCAL INFILE "F:\\Awellfrog\\subjects\\Database\\lab1\\lab1_data\\project.txt"
                INTO TABLE project;
LOAD DATA LOCAL INFILE "F:\\Awellfrog\\subjects\\Database\\lab1\\lab1_data\\works_on.txt"
                INTO TABLE works_on;

-- （1）参加了项目名为“SQL Project”的员工名字
SELECT ename FROM employee
WHERE essn IN (
    SELECT essn FROM works_on AS wo, project AS pj
    WHERE (
        wo.pno = pj.pno AND pj.pname = "SQL" AND wo.hours > 0
    )
);

-- （2）在“Research Department”工作且工资低于3000元的员工名字和地址
SELECT ename, address FROM employee AS e, department AS dp
WHERE (
    dp.dname = "研发部" AND e.dno = dp.dno AND e.salary < 3000
);

-- （3）没有参加项目编号为P1的项目的员工姓名；
SELECT ename FROM employee AS e
WHERE essn NOT IN (
    SELECT essn FROM works_on AS wo
    WHERE (
        wo.pno = "P1" AND wo.hours > 0 -- 参加了的
    )
);

-- （4）由张红领导的工作人员的姓名和所在部门的名字；
SELECT ename, dname FROM employee AS e, department AS dp
WHERE dp.dno = e.dno AND e.superssn IN(
    SELECT essn FROM employee
    WHERE (ename = "张红")
);

-- （5）至少参加了项目编号为P1和P2的项目的员工号；
-- 方法一
SELECT w1.essn FROM works_on AS w1, works_on AS w2
WHERE (
    w1.essn = w2.essn AND (w1.pno = "P1" AND w1.hours > 0) AND (w2.pno = "P2" AND w2.hours > 0)
);
-- 方法二
SELECT essn FROM works_on
WHERE pno = "P1" AND hours > 0 AND essn IN(
    SELECT essn FROM works_on
    WHERE pno = "P2" AND hours > 0
);

-- （6）参加了全部项目的员工号码和姓名；
-- 甚至连答案都看不懂，让我来一步一步试试

SELECT essn, ename from employee -- 如果该员工在补集中没有出现过，则说明他参加了全部的项目
-- 遍历 employee 的所有成员做如下的筛选
WHERE not exists ( -- 只要有一个没参与过的项目，这个员工就不会被取出
    SELECT pno FROM project -- 选出每个员工没参与的项目，相当于取补集
    WHERE not exists(
        SELECT * FROM works_on -- 选出每个员工参与的项目
        WHERE   works_on.pno = project.pno
                AND works_on.essn = employee.essn
    )
);
-- 利用伪代码来理解一下这段
for (employee):
    for (pno):
        for (works_on):
            if works_on.pno = project.pno AND works_on.essn = employee.essn:
                ans.append(works_on.pno)
        if (pno not in ans) result.append(pno)
    if (result == NULL) return true

-- 这个还好理解点，某个员工参与的项目数等于项目总数
SELECT essn, ename FROM employee
WHERE essn IN (
    SELECT essn FROM works_on
    GROUP BY essn HAVING count(*) IN (
        SELECT COUNT(*) FROM project
    )
);

-- （7）员工平均工资低于3000元的部门名称；
SELECT dname FROM department
WHERE dno IN (
    SELECT dno FROM employee
    GROUP BY dno HAVING AVG(salary) < 3000
);

-- （8）至少参与了3个项目且工作总时间不超过8小时的员工名字；
SELECT ename FROM employee
WHERE essn IN (
    SELECT essn FROM works_on
    GROUP BY essn HAVING COUNT(pno) >= 3 AND SUM(hours) <= 8
);

-- （9）每个部门的员工小时平均工资；
SELECT sums.dno, sumsalary / sumhours AS hoursavgsalary
FROM (
    SELECT dno, SUM(salary) AS sumsalary
    FROM employee GROUP BY dno
) AS sums, (
    SELECT dno, SUM(hours) AS sumhours
    FROM works_on JOIN employee ON works_on.essn = employee.essn
    GROUP BY dno
) AS sumh
WHERE sums.dno = sumh.dno;