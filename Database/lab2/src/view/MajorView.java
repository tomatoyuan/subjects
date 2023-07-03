package view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;
import model.Major;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLIntegrityConstraintViolationException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class MajorView {
    @FXML
    private TableView<Major> tableView;
    @FXML
    private TableColumn<Major,Integer> major_id;
    @FXML
    private TableColumn<Major, String> major_name;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Major> majorData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showMajor() {
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Major";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                majorData.add(new Major(Integer.valueOf(rs.getString(1)), rs.getString(2)));
            }

            major_id.setCellValueFactory( new PropertyValueFactory<>("major_id") );
            major_name.setCellValueFactory( new PropertyValueFactory<>("major_name") );
            tableView.setItems(majorData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableMajor(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Major";
            ResultSet rs = stmt.executeQuery(sql);
            majorData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                majorData.add(new Major(Integer.valueOf(rs.getString(1)), rs.getString(2)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteMajor() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Major " +
                                "WHERE major_id=" + tableView.getItems().get(selectedIndex).getMajor_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select a major in the table.");
                //alert.setContentText("Please select a person in the table");
                alert.showAndWait();
            }
        }catch (SQLIntegrityConstraintViolationException e){
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setHeaderText("该条记录暂时不可删除！");
            alert.setContentText("由于外键约束，此记录暂时不可删除！");
            alert.showAndWait();
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    /**
     * Called when the user clicks the new button. Opens a alert to edit
     * details for a new major.
     */
    @FXML
    private void handleNewMajor() {
        try{
            List<Object> objects = this.showAddMajorView();
            boolean okClicked = (Boolean)objects.get(0);
            String name = (String)objects.get(1);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                String sql =    "INSERT INTO Major(major_name) "+
                                "VALUES (\""+name+"\")";
                stmt.executeUpdate(sql);
                refreshTableMajor();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该专业名字已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddMajorView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddMajorView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddMajorView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedMajor();
        String name = controller.getMajor_name();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(name);
        return objects;
    }
}
