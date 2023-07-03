package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.stage.Stage;
import javafx.scene.control.TextField;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class AddClassView {
    @FXML
    private TextField class_name;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getClass_name() { return class_name.getText(); }

    public boolean isOkClickedClass() { return okClicked; }

    @FXML
    private void handleOk() {
        if (!class_name.getText().equals("")) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("班级名字不可为空值");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }
}
