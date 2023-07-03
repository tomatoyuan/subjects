package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class ClassInfo {
    private final IntegerProperty class_id;
    private final StringProperty class_name;
    private final IntegerProperty tot_num;

    public ClassInfo(Integer class_id, String class_name, Integer tot_num){
        this.class_id=new SimpleIntegerProperty(class_id);
        this.class_name = new SimpleStringProperty(class_name);
        this.tot_num=new SimpleIntegerProperty(tot_num);
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

    // tot_num 的三个 method
    public int getTot_num() {
        return tot_num.get();
    }
    public void setTot_num(int tot_num) {
        this.tot_num.set(tot_num);
    }
    public IntegerProperty tot_numProperty() {
        return tot_num;
    }
}
