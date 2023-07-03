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
import model.Dean;
import model.Teacher;

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
public class TeacherView {
    @FXML
    private TableView<Teacher> tableView;
    @FXML
    private TableColumn<Teacher,Integer> teacher_id;
    @FXML
    private TableColumn<Teacher, String> teacher_name;
    @FXML
    private TableColumn<Teacher,Integer> teacher_salary;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Teacher> teacherData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showTeacher(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Teacher";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                teacherData.add(new Teacher(Integer.valueOf(rs.getString(1)), rs.getString(2),
                                            Integer.valueOf(rs.getString(3))));
            }
            teacher_id.setCellValueFactory( new PropertyValueFactory<>("teacher_id") );
            teacher_name.setCellValueFactory( new PropertyValueFactory<>("teacher_name") );
            teacher_salary.setCellValueFactory( new PropertyValueFactory<>("teacher_salary") );
            tableView.setItems(teacherData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableTeacher(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Teacher";
            ResultSet rs = stmt.executeQuery(sql);
            teacherData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                //如果没有填入工资，则默认年薪20w
                Integer temp_salary = (Integer)200000;
                if (rs.getString(3) != null) {
                    temp_salary = Integer.valueOf(rs.getString(3));
                }


                teacherData.add(new Teacher(Integer.valueOf(rs.getString(1)), rs.getString(2),
                                            temp_salary));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteTeacher() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Teacher " +
                                "WHERE teacher_id=" + tableView.getItems().get(selectedIndex).getTeacher_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select a teacher in the table.");
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
     * details for a new teacher.
     */
    @FXML
    private void handleNewTeacher() {
        try{
            List<Object> objects = this.showAddTeacherView();
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
                String sql =    "INSERT INTO Teacher(teacher_name, teacher_salary) " +
                                "VALUE (\"" + name + "\", " + salary + ")";
                stmt.executeUpdate(sql);

                refreshTableTeacher();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该教师已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddTeacherView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddTeacherView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddTeacherView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedTeacher();
        String name = controller.getTeacher_name();
        String salary = controller.getTeacher_salary();
        if (salary.equals("")) salary = "200000"; // 默认工资 20w
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(name);
        objects.add(salary);
        return objects;
    }
}
