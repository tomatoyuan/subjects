package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class Teacher {
    private final IntegerProperty teacher_id;
    private final StringProperty teacher_name;
    private final IntegerProperty teacher_salary;
    public Teacher(Integer teacher_id, String teacher_name, Integer teacher_salary) {
        this.teacher_id = new SimpleIntegerProperty(teacher_id);
        this.teacher_name = new SimpleStringProperty(teacher_name);
        this.teacher_salary = new SimpleIntegerProperty(teacher_salary);
    }

    // teacher_id 的三个 method
    public int getTeacher_id() {
        return teacher_id.get();
    }
    public void setTeacher_id(int teacher_id) {this.teacher_id.set(teacher_id); }
    public IntegerProperty teacher_idProperty() {
        return teacher_id;
    }

    // teacher_name 的三个 method
    public String getTeacher_name() {
        return teacher_name.get();
    }
    public void setTeacher_name(String teacher_name) {
        this.teacher_name.set(teacher_name);
    }
    public StringProperty teacher_nameProperty() {
        return teacher_name;
    }

    // teacher_salary 的三个 method
    public int getTeacher_salary() {
        return teacher_salary.get();
    }
    public void setTeacher_salary(int teacher_salary) {
        this.teacher_salary.set(teacher_salary);
    }
    public IntegerProperty teacher_salaryProperty() {
        return teacher_salary;
    }
}
