package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class AddStudentView {
    @FXML
    private TextField student_id;
    @FXML
    private TextField student_name;
    @FXML
    private TextField apartment_name;
    @FXML
    private TextField class_name;
    @FXML
    private TextField college_name;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getStudent_id() { return student_id.getText(); }
    public String getStudent_name() { return student_name.getText(); }
    public String getApartment_name() { return apartment_name.getText(); }
    public String getClass_name() { return class_name.getText(); }
    public String getCollege_name() { return college_name.getText(); }

    public boolean isOkClickedStudent() { return okClicked; }

    @FXML
    private void handleOk() {
        if (   !student_id.getText().equals("")     && !student_name.getText().equals("")
            && !apartment_name.getText().equals("") && !class_name.getText().equals("")
            && !college_name.getText().equals("") ) {
//        if ( !student_id.getText().equals("") ) {
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
