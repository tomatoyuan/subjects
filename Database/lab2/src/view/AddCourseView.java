package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class AddCourseView {
    @FXML
    private TextField course_id;
    @FXML
    private TextField course_name;
    @FXML
    private TextField teacher_name;
    @FXML
    private TextField credit;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getCourse_id() { return course_id.getText(); }
    public String getCourse_name() { return course_name.getText(); }
    public String getTeacher_name() { return teacher_name.getText(); }
    public String getCredit() { return credit.getText(); }

    public boolean isOkClickedCourse() { return okClicked; }

    @FXML
    private void handleOk() {
        if (  !course_id.getText().equals("")    && !course_name.getText().equals("")
           && !teacher_name.getText().equals("") && !credit.getText().equals("") ) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("有信息值为空值，请仔细填写！");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }
}
