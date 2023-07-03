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
import model.College;

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
public class CollegeView {
    @FXML
    private TableView<College> tableView;
    @FXML
    private TableColumn<College,Integer> college_id;
    @FXML
    private TableColumn<College, String> college_name;
    @FXML
    private TableColumn<College,Integer> dean_id;
    @FXML
    private TableColumn<College, String> dean_name;
    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<College> collegeData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showCollege(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM College";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                collegeData.add(new College(Integer.valueOf(rs.getString(1)), rs.getString(2),
                                            Integer.valueOf(rs.getString(3)), rs.getString(4)));
            }

            college_id.setCellValueFactory( new PropertyValueFactory<>("college_id") );
            college_name.setCellValueFactory( new PropertyValueFactory<>("college_name") );
            dean_id.setCellValueFactory( new PropertyValueFactory<>("dean_id") );
            dean_name.setCellValueFactory( new PropertyValueFactory<>("dean_name") );
            tableView.setItems(collegeData);
        }catch(Exception e) {
            e.printStackTrace();
        }
    }

    public void refreshTableCollege(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM College";
            ResultSet rs = stmt.executeQuery(sql);
            collegeData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                collegeData.add(new College(Integer.valueOf(rs.getString(1)), rs.getString(2),
                                            Integer.valueOf(rs.getString(3)), rs.getString(4)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteCollege() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM College " +
                                "WHERE college_id=" + tableView.getItems().get(selectedIndex).getCollege_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select an college in the table.");
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
     * details for a new college.
     */
    @FXML
    private void handleNewCollege() {
        try{
            List<Object> objects = this.showAddCollegeView();
            boolean okClicked = (Boolean)objects.get(0);
            String name = (String)objects.get(1);
            String dname = (String)objects.get(2);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                //INSERT INTO College(college_name, dean_id, dean_name)
                //SELECT name, dean_id, dean_name FROM Dean
                //WHERE dean_name = dname
                // 输入学院名和院长名，自动填充院长ID，如果没有该院长，则创建不成功
                String sql =    "INSERT INTO College(college_name, dean_id, dean_name) " +
                                "SELECT \"" + name +"\", dean_id, dean_name FROM Dean " +
                                "WHERE dean_name=\"" + dname + "\"";
                stmt.executeUpdate(sql);

                refreshTableCollege();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该学院名字已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddCollegeView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddCollegeView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddCollegeView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedCollege();
        String name = controller.getCollege_name();
        String dname = controller.getCollegeDean_name();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(name);
        objects.add(dname);
        return objects;
    }
}
