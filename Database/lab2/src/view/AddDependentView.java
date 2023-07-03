package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class AddDependentView {
    @FXML
    private TextField student_name;
    @FXML
    private TextField dependent_name;
    @FXML
    private TextField relationship;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getStudent_name() { return student_name.getText(); }
    public String getDependent_name() { return dependent_name.getText(); }
    public String getRelationship() { return relationship.getText(); }

    public boolean isOkClickedDependent() { return okClicked; }

    @FXML
    private void handleOk() {
        if (!student_name.getText().equals("") && !dependent_name.getText().equals("") && !relationship.getText().equals("")) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("学生姓名/家属姓名/学生关系不可为空值");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }
}
