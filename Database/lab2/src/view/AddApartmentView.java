package view;

import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class AddApartmentView {

    @FXML
    private TextField apartment_name;

    private boolean okClicked = false;
    private Stage stage;

    public void setStage(Stage stage) {
        this.stage = stage;
    }

    public String getApartment_name() { return apartment_name.getText(); }

    public boolean isOkClickedApartment(){ return okClicked; }

    @FXML
    private void handleOk() {
        if (!apartment_name.getText().equals("")) {
            okClicked = true;
            stage.close();
        } else {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("Please correct invalid fields");
            alert.setContentText("公寓名字不可为空值");
            alert.showAndWait();
        }
    }

    @FXML
    private void handleCancel() {
        stage.close();
    }

}
