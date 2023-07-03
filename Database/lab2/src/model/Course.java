package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class Course {
    private final StringProperty course_id;
    private final StringProperty course_name;
    private final IntegerProperty teacher_id;
    private final StringProperty teacher_name;
    private final StringProperty credit;

    public Course(String course_id, String course_name, Integer teacher_id, String teacher_name, String credit){
        this.course_id=new SimpleStringProperty(course_id);
        this.course_name = new SimpleStringProperty(course_name);
        this.teacher_id=new SimpleIntegerProperty(teacher_id);
        this.teacher_name = new SimpleStringProperty(teacher_name);
        this.credit=new SimpleStringProperty(credit);
    }

    // course_id 的三个 method
    public String getCourse_id() { return course_id.get(); }
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

    // teacher_id 的三个 method
    public int getTeacher_id() {
        return teacher_id.get();
    }
    public void setTeacher_id(int teacher_id) {
        this.teacher_id.set(teacher_id);
    }
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

    // credit 的三个 method
    public String getCredit() {
        return credit.get();
    }
    public void setCredit(String credit) {
        this.credit.set(credit);
    }
    public StringProperty creditProperty() {
        return credit;
    }
}
