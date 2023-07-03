package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class AddCollegeView {
    @FXML
    private TextField college_name;
    @FXML
    private TextField dean_name;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getCollege_name() { return college_name.getText(); }
    public String getCollegeDean_name() { return dean_name.getText(); }

    public boolean isOkClickedCollege() { return okClicked; }

    @FXML
    private void handleOk() {
        if (!college_name.getText().equals("") && !dean_name.getText().equals("")) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("学院或院长名字不可为空值");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }
}
