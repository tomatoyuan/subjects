package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class College {
    private final IntegerProperty college_id;
    private final StringProperty college_name;
    private final IntegerProperty dean_id;
    private final StringProperty dean_name;

    public College(Integer college_id, String college_name, Integer dean_id, String dean_name) {
        this.college_id = new SimpleIntegerProperty(college_id);
        this.college_name = new SimpleStringProperty(college_name);
        this.dean_id = new SimpleIntegerProperty(dean_id);
        this.dean_name = new SimpleStringProperty(dean_name);
    }

    // college_id 的三个 method
    public int getCollege_id() { return college_id.get(); }
    public void setCollege_id(int college_id) { this.college_id.set(college_id); }
    public IntegerProperty college_idProperty() { return college_id; }

    // college_name 的三个 method
    public String getCollege_name() {
        return college_name.get();
    }
    public void setCollege_name(String college_name) {
        this.college_name.set(college_name);
    }
    public StringProperty college_nameProperty() {
        return college_name;
    }

    // dean_id 的三个 method
    public int getDean_id() {
        return dean_id.get();
    }
    public void setDean_id(int dean_id) {
        this.dean_id.set(dean_id);
    }
    public IntegerProperty dean_idProperty() {
        return dean_id;
    }

    // dean_name 的三个 method
    public String getDean_name() {
        return dean_name.get();
    }
    public void setDean_name(String dean_name) {
        this.dean_name.set(dean_name);
    }
    public StringProperty dean_nameProperty() {
        return dean_name;
    }
}
