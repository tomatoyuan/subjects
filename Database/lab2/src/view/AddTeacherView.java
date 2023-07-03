package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class AddTeacherView {
    @FXML
    private TextField teacher_name;
    @FXML
    private TextField teacher_salary;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getTeacher_name() { return teacher_name.getText(); }
    public String getTeacher_salary() { return teacher_salary.getText(); }

    public boolean isOkClickedTeacher() { return okClicked; }

    @FXML
    private void handleOk() {
        if (!teacher_name.getText().equals("")) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("教师名字不可为空值");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }
}
