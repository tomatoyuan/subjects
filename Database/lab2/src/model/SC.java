package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/** 学生课程成绩
 * @author 番茄元
 * @date 2023/3/29
 */
public class SC {
    private final StringProperty student_id;
    private final StringProperty student_name;
    private final StringProperty course_id;
    private final StringProperty course_name;
    private final StringProperty grade;

    public SC(String student_id, String student_name, String course_id, String course_name, String grade){
        this.student_id=new SimpleStringProperty(student_id);
        this.student_name = new SimpleStringProperty(student_name);
        this.course_id=new SimpleStringProperty(course_id);
        this.course_name = new SimpleStringProperty(course_name);
        this.grade=new SimpleStringProperty(grade);
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

    // course_id 的三个 method
    public String getCourse_id() {
        return course_id.get();
    }
    public void setCourse_id(String course_id) {
        this.course_id.set(course_id);
    }
    public StringProperty course_idProperty() {
        return course_id;
    }

    // course_name 的三个 method
    public String getCourse_name() {
        return course_name.get();
    }
    public void setCourse_name(String course_name) {
        this.course_name.set(course_name);
    }
    public StringProperty course_nameProperty() {
        return course_name;
    }

    // grade 的三个 method
    public String getGrade() {
        return grade.get();
    }
    public void setGrade(String grade) {
        this.grade.set(grade);
    }
    public StringProperty gradeProperty() {
        return grade;
    }
}
