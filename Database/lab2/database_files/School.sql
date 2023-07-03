CREATE DATABASE School;

USE School;

-- 创建实体
CREATE TABLE Teacher(
    teacher_id      INT NOT NULL AUTO_INCREMENT,
    teacher_name    text,
    teacher_salary  int,
    PRIMARY KEY(teacher_id)
);

CREATE TABLE Dean(
    dean_id       	INT NOT NULL AUTO_INCREMENT,
    dean_name     	text,
    dean_salary   	int,
    PRIMARY KEY(dean_id)
);

CREATE TABLE Class(
    class_id        INT NOT NULL AUTO_INCREMENT,
    class_name      text,
    PRIMARY KEY(class_id)
);

CREATE TABLE Apartment(
    apartment_id    INT NOT NULL AUTO_INCREMENT,
    apartment_name  text,
    PRIMARY KEY(apartment_id)
);

CREATE TABLE Course(
    course_id       varchar(12),
    course_name     text,
    teacher_id		int,
    teacher_name    text,
    credit          text,
    PRIMARY KEY(course_id),
    FOREIGN KEY(teacher_id) REFERENCES Teacher(teacher_id) on delete cascade -- 外键约束，级联删除
);

CREATE TABLE College(
    college_id      INT NOT NULL AUTO_INCREMENT,
    college_name    text,
    dean_id       	int,
    dean_name     	text,
    PRIMARY KEY(college_id),
    FOREIGN KEY(dean_id) REFERENCES Dean(dean_id) on delete cascade -- 外键约束，级联删除
);

CREATE TABLE Major(
    major_id        INT NOT NULL AUTO_INCREMENT,
    major_name      text,
    PRIMARY KEY(major_id)
);

CREATE TABLE Student(
    student_id      varchar(12),
    student_name    text,
    apartment_id    int,
    apartment_name  text,
    class_id		int,
    class_name      text,
    college_id		int,
    college_name    text,
    PRIMARY KEY(student_id),
    FOREIGN KEY(apartment_id) REFERENCES Apartment(apartment_id) on delete cascade, -- 外键约束，级联删除
    FOREIGN KEY(class_id) REFERENCES Class(class_id) on delete cascade, -- 外键约束，级联删除
    FOREIGN KEY(college_id) REFERENCES College(college_id) on delete cascade -- 外键约束，级联删除
);

CREATE TABLE Dependent(
    dependent_name  varchar(12),
    student_id      varchar(12),
    student_name    text,
    relationship    text,
    PRIMARY KEY(student_id, dependent_name),
    FOREIGN KEY(student_id) REFERENCES Student(student_id) on delete cascade -- 外键约束，级联删除
);

CREATE TABLE SC(
    student_id      varchar(12),
    student_name    text,
    course_id       varchar(12),
    course_name     text,
    grade           text,
    PRIMARY KEY(student_id, course_id),
    FOREIGN KEY(student_id) REFERENCES Student(student_id) on delete cascade, -- 外键约束，级联删除
    FOREIGN KEY(course_id) REFERENCES Course(course_id) on delete cascade -- 外键约束，级联删除
);


-- 创建视图
-- ANY_VALUE() 用于忽略 ONLY_FULL_GROUP_BY 错误
CREATE VIEW ClassInfo AS
SELECT class_id AS cid, ANY_VALUE(class_name) AS cname, COUNT(class_id) AS tot_num
FROM Student
GROUP BY class_id;

    -- 为 cid 来源 Student 表中的 class_id 属性建立索引，加速检索
    CREATE INDEX class_id ON Student(class_id);

-- 创建视图，内连接查询学生选课与对应授课教师
CREATE VIEW StudentTeacher AS
SELECT S.student_id, S.student_name, S.course_name, C.teacher_name
FROM SC S INNER JOIN Course C
ON S.course_id = C.course_id;


-- 创建触发器
-- 建立回收站，将删除的学生信息放入 StuRecycleBin
CREATE TABLE StuRecycleBin(
    student_id      varchar(12),
    student_name    text,
    apartment_id    int,
    apartment_name  text,
    class_id		int,
    class_name      text,
    college_id		int,
    college_name    text,
    PRIMARY KEY(student_id)
);

-- 创建触发器，当学生被删除时，将包含该学生信息的元组放入回收站 StuRecycleBin
DELIMITER //
CREATE TRIGGER deleteStudent BEFORE DELETE ON Student
FOR EACH ROW
BEGIN
    INSERT INTO StuRecycleBin(student_id, student_name, apartment_id, apartment_name, class_id, class_name, college_id, college_name)
    VALUES(OLD.student_id, OLD.student_name, OLD.apartment_id, OLD.apartment_name,
           OLD.class_id, OLD.class_name, OLD.college_id, OLD.college_name
    );
END //
DELIMITER ;