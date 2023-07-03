package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/31
 */
public class StudentTeacher {
    private final StringProperty student_id;
    private final StringProperty student_name;
    private final StringProperty course_name;
    private final StringProperty teacher_name;

    public StudentTeacher(String student_id, String student_name, String course_name, String teacher_name){
        this.student_id = new SimpleStringProperty(student_id);
        this.student_name = new SimpleStringProperty(student_name);
        this.course_name = new SimpleStringProperty(course_name);
        this.teacher_name = new SimpleStringProperty(teacher_name);
    }

    // student_id 的三个 method
    public String getStudent_id() {
        return student_id.get();
    }
    public void setStudent_id(String student_id) {
        this.student_id.set(student_id);
    }
    public StringProperty student_idProperty() {
        return student_id;
    }

    // student_name 的三个 method
    public String getStudent_name() {
        return student_name.get();
    }
    public void setStudent_name(String student_name) {
        this.student_name.set(student_name);
    }
    public StringProperty student_nameProperty() {
        return student_name;
    }

    // student_id 的三个 method
    public String getCourse_name() {
        return course_name.get();
    }
    public void setCourse_name(String course_name) {
        this.course_name.set(course_name);
    }
    public StringProperty course_nameProperty() {
        return course_name;
    }

    // student_name 的三个 method
    public String getTeacher_name() {
        return teacher_name.get();
    }
    public void setTeacher_name(String teacher_name) {
        this.teacher_name.set(teacher_name);
    }
    public StringProperty teacher_nameProperty() {
        return teacher_name;
    }

}
