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
import model.Apartment;
import model.Class;

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
public class ClassView {
    @FXML
    private TableView<Class> tableView;
    @FXML
    private TableColumn<Class, Integer> class_id;
    @FXML
    private TableColumn<Class, String> class_name;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Class> classData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showClass(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Class";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                classData.add(new Class(Integer.valueOf(rs.getString(1)), rs.getString(2)));
            }

            class_id.setCellValueFactory( new PropertyValueFactory<>("class_id") );
            class_name.setCellValueFactory( new PropertyValueFactory<>("class_name") );
            tableView.setItems(classData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableClass(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Class";
            ResultSet rs = stmt.executeQuery(sql);
            classData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                classData.add(new Class(Integer.valueOf(rs.getString(1)), rs.getString(2)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteClass() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Class " +
                                "WHERE class_id=" + tableView.getItems().get(selectedIndex).getClass_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select an class in the table.");
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
     * details for a new class.
     */
    @FXML
    private void handleNewClass() {
        try{
            List<Object> objects = this.showAddClassView();
            boolean okClicked = (Boolean)objects.get(0);
            String name = (String)objects.get(1);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                String sql =    "INSERT INTO Class(class_name) "+
                                "VALUES (\""+name+"\")";
                stmt.executeUpdate(sql);
                refreshTableClass();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该班级名字已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddClassView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddClassView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddClassView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedClass();
        String name = controller.getClass_name();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(name);
        return objects;
    }
}
