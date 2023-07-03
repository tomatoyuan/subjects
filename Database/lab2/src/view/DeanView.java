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
import model.Dean;

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
public class DeanView {
    @FXML
    private TableView<Dean> tableView;
    @FXML
    private TableColumn<Dean,Integer> dean_id;
    @FXML
    private TableColumn<Dean, String> dean_name;
    @FXML
    private TableColumn<Dean,Integer> dean_salary;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Dean> deanData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showDean(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Dean";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                //如果没有填入工资，则默认年薪千万
                Integer temp_salary = (Integer)10000000;
                if (rs.getString(3) != null) {
                    temp_salary = Integer.valueOf(rs.getString(3));
                }


                deanData.add(new Dean(  Integer.valueOf(rs.getString(1)), rs.getString(2),
                                        temp_salary));
            }
            dean_id.setCellValueFactory( new PropertyValueFactory<>("dean_id") );
            dean_name.setCellValueFactory( new PropertyValueFactory<>("dean_name") );
            dean_salary.setCellValueFactory( new PropertyValueFactory<>("dean_salary") );
            tableView.setItems(deanData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableDean(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Dean";
            ResultSet rs = stmt.executeQuery(sql);
            deanData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                //如果没有填入工资，则默认年薪千万
                Integer temp_salary = (Integer)10000000;
                if (rs.getString(3) != null) {
                    temp_salary = Integer.valueOf(rs.getString(3));
                }


                deanData.add(new Dean(  Integer.valueOf(rs.getString(1)), rs.getString(2),
                                        temp_salary));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteDean() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Dean " +
                                "WHERE dean_id=" + tableView.getItems().get(selectedIndex).getDean_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select a dean in the table.");
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
     * details for a new dean.
     */
    @FXML
    private void handleNewDean() {
        try{
            List<Object> objects = this.showAddDeanView();
            boolean okClicked = (Boolean)objects.get(0);
            String name = (String)objects.get(1);
            String salary = (String)objects.get(2);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                //INSERT INTO College(college_name, dean_id, dean_name)
                //SELECT name, dean_id, dean_name FROM Dean
                //WHERE dean_name = dname
                // 输入学院名和院长名，自动填充院长ID，如果没有该院长，则创建不成功
                String sql =    "INSERT INTO Dean(dean_name, dean_salary) " +
                                "VALUE (\"" + name + "\", " + salary + ")";
                stmt.executeUpdate(sql);

                refreshTableDean();
                conn.commit();
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该院长已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddDeanView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddDeanView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddDeanView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedDean();
        String name = controller.getDean_name();
        String salary = controller.getDean_salary();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(name);
        objects.add(salary);
        return objects;
    }
}
