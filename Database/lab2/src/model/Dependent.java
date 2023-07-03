package model;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class Dependent {
    private final StringProperty dependent_name;
    private final StringProperty student_id;
    private final StringProperty student_name;
    private final StringProperty relationship;
    public Dependent(String dependent_name, String student_id, String student_name, String relationship) {
        this.dependent_name = new SimpleStringProperty(dependent_name);
        this.student_id = new SimpleStringProperty(student_id);
        this.student_name = new SimpleStringProperty(student_name);
        this.relationship = new SimpleStringProperty(relationship);
    }

    // dependent_name 的三个 method
    public String getDependent_name() {
        return dependent_name.get();
    }
    public void setDependent_name(String dependent_name) {
        this.dependent_name.set(dependent_name);
    }
    public StringProperty dependent_nameProperty() {
        return dependent_name;
    }

    // relationship 的三个 method
    public String getStudent_id() {
        return student_id.get();
    }
    public void setStudent_id(String student_id) {
        this.student_id.set(student_id);
    }
    public StringProperty student_idProperty() {
        return student_id;
    }

    // dependent_name 的三个 method
    public String getStudent_name() {
        return student_name.get();
    }
    public void setStudent_name(String student_name) {
        this.student_name.set(student_name);
    }
    public StringProperty student_nameProperty() {
        return student_name;
    }

    // relationship 的三个 method
    public String getRelationship() {
        return relationship.get();
    }
    public void setRelationship(String relationship) {
        this.relationship.set(relationship);
    }
    public StringProperty relationshipProperty() {
        return relationship;
    }
}
