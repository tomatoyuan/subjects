package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class AddDeanView {
    @FXML
    private TextField dean_name;
    @FXML
    private TextField dean_salary;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) { this.stage = stage; }

    public String getDean_name() { return dean_name.getText(); }
    public String getDean_salary() { return dean_salary.getText(); }

    public boolean isOkClickedDean() { return okClicked; }

    @FXML
    private void handleOk() {
        if (!dean_name.getText().equals("")) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("院长名字不可为空值");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }
}
