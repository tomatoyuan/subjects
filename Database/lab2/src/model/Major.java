package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class Major {
    private final IntegerProperty major_id;
    private final StringProperty major_name;
    public Major(Integer major_id, String major_name) {
        this.major_id = new SimpleIntegerProperty(major_id);
        this.major_name = new SimpleStringProperty(major_name);
    }

    // major_id 的三个 method
    public int getMajor_id() { return major_id.get(); }
    public void setMajor_id(int major_id) {
        this.major_id.set(major_id);
    }
    public IntegerProperty major_idProperty() {
        return major_id;
    }

    // major_name 的三个 method
    public String getMajor_name() {
        return major_name.get();
    }
    public void setMajor_name(String major_name) {
        this.major_name.set(major_name);
    }
    public StringProperty major_nameProperty() {
        return major_name;
    }
}
