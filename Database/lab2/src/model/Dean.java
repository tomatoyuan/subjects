package model;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class Dean {
    private final IntegerProperty dean_id;
    private final StringProperty dean_name;
    private final IntegerProperty dean_salary;
    public Dean(Integer dean_id, String dean_name, Integer dean_salary) {
        this.dean_id = new SimpleIntegerProperty(dean_id);
        this.dean_name = new SimpleStringProperty(dean_name);
        this.dean_salary = new SimpleIntegerProperty(dean_salary);
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

    // dean_salary 的三个 method
    public int getDean_salary() {
        return dean_salary.get();
    }
    public void setDean_salary(int dean_salary) {
        this.dean_salary.set(dean_salary);
    }
    public IntegerProperty dean_salaryProperty() {
        return dean_salary;
    }
}
