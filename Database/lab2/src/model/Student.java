package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class Student {
    private final StringProperty student_id;
    private final StringProperty student_name;
    private final IntegerProperty apartment_id;
    private final StringProperty apartment_name;
    private final IntegerProperty class_id;
    private final StringProperty class_name;
    private final IntegerProperty college_id;
    private final StringProperty college_name;

    public Student(String student_id, String student_name,
                   Integer apartment_id, String apartment_name,
                   Integer class_id, String class_name,
                   Integer college_id, String college_name) {
        this.student_id=new SimpleStringProperty(student_id);
        this.student_name = new SimpleStringProperty(student_name);
        this.apartment_id = new SimpleIntegerProperty(apartment_id);
        this.apartment_name = new SimpleStringProperty(apartment_name);
        this.class_id = new SimpleIntegerProperty(class_id);
        this.class_name = new SimpleStringProperty(class_name);
        this.college_id = new SimpleIntegerProperty(college_id);
        this.college_name = new SimpleStringProperty(college_name);
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

    // apartment_id 的三个 method
    public int getApartment_id() {
        return apartment_id.get();
    }
    public void setApartment_id(int apartment_id) {
        this.apartment_id.set(apartment_id);
    }
    public IntegerProperty apartment_idProperty() {
        return apartment_id;
    }

    // apartment_name 的三个 method
    public String getApartment_name() {
        return apartment_name.get();
    }
    public void setApartment_name(String apartment_name) {
        this.apartment_name.set(apartment_name);
    }
    public StringProperty apartment_nameProperty() {
        return apartment_name;
    }

    // class_id 的三个 method
    public int getClass_id() {
        return class_id.get();
    }
    public void setClass_id(int class_id) {
        this.class_id.set(class_id);
    }
    public IntegerProperty class_idProperty() {
        return class_id;
    }

    // class_name 的三个 method
    public String getClass_name() {
        return class_name.get();
    }
    public void setClass_name(String class_name) {
        this.class_name.set(class_name);
    }
    public StringProperty class_nameProperty() {
        return class_name;
    }

    // college_id 的三个 method
    public int getCollege_id() { return college_id.get(); }
    public void setCollege_id(int college_id) { this.college_id.set(college_id); }
    public IntegerProperty college_idProperty() { return college_id; }

    // college_name 的三个 method
    public String getCollege_name() { return college_name.get(); }
    public void setCollege_name(String college_name) { this.college_name.set(college_name); }
    public StringProperty college_nameProperty() { return college_name; }
}
