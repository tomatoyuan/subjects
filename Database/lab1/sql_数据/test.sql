SELECT ename, sumsalary / sumhours AS hoursavgsalary
FROM (
    SELECT ename, salary AS sumsalary
    FROM employee, works_on, project
    WHERE (
        works_on.essn = employee.essn AND works_on.pno = project.pno AND project.dno != employee.dno
    )
) AS sums, (
    SELECT dno, SUM(hours) AS sumhours
    FROM works_on JOIN employee ON works_on.essn = employee.essn
    GROUP BY dno
) AS sumh
WHERE sum.dno = sumh.dno;

-- 作业1
SELECT SET1.ename, employeeavgsalary.hoursavgsalary FROM
    (SELECT DISTINCT ename
    FROM employee, works_on, project
    WHERE (
        works_on.essn = employee.essn AND works_on.pno = project.pno AND project.dno != employee.dno
    )) AS SET1, (
        SELECT sums.ename, sumsalary / sumhours AS hoursavgsalary
        FROM (
            SELECT ename, SUM(salary) AS sumsalary
            FROM employee
            GROUP BY ename
        ) AS sums, (
            SELECT ename, SUM(hours) AS sumhours
            FROM works_on JOIN employee ON works_on.essn = employee.essn
            GROUP BY ename
        ) AS sumh
        WHERE sums.ename = sumh.ename
    ) AS employeeavgsalary
WHERE SET1.ename = employeeavgsalary.ename;




-- 作业2
SELECT ename, hoursavgsalary
FROM(
    SELECT sums.ename, sumsalary / sumhours AS hoursavgsalary
    FROM (
        SELECT ename, SUM(salary) AS sumsalary
        FROM employee
        GROUP BY ename
    ) AS sums, (
        SELECT ename, SUM(hours) AS sumhours
        FROM works_on JOIN employee ON works_on.essn = employee.essn
        GROUP BY ename
    ) AS sumh
    WHERE sums.ename = sumh.ename
) AS employeeavgsalary
WHERE (
    employeeavgsalary.hoursavgsalary > 1000
);
