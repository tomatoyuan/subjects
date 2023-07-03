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
import model.Course;
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
public class CourseView {
    @FXML
    private TableView<Course> tableView;
    @FXML
    private TableColumn<Course, String> course_id;
    @FXML
    private TableColumn<Course, String> course_name;
    @FXML
    private TableColumn<Course, Integer> teacher_id;
    @FXML
    private TableColumn<Course, String> teacher_name;
    @FXML
    private TableColumn<Course, String> credit;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Course> courseData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showCourse(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Course";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                    courseData.add(new Course(  rs.getString(1), rs.getString(2),
                                                Integer.valueOf(rs.getString(3)), rs.getString(4),
                                                rs.getString(5)));
            }
            course_id.setCellValueFactory( new PropertyValueFactory<>("course_id") );
            course_name.setCellValueFactory( new PropertyValueFactory<>("course_name") );
            teacher_id.setCellValueFactory( new PropertyValueFactory<>("teacher_id") );
            teacher_name.setCellValueFactory( new PropertyValueFactory<>("teacher_name") );
            credit.setCellValueFactory( new PropertyValueFactory<>("credit") );
            tableView.setItems(courseData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableCourse(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Course";
            ResultSet rs = stmt.executeQuery(sql);
            courseData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                courseData.add(new Course(  rs.getString(1), rs.getString(2),
                                            Integer.valueOf(rs.getString(3)), rs.getString(4),
                                            rs.getString(5)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteCourse() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Dean " +
                                "WHERE dean_id=" + tableView.getItems().get(selectedIndex).getCourse_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select a course in the table.");
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
    private void handleNewCourse() {
        try{
            List<Object> objects = this.showAddCourseView();
            boolean okClicked = (Boolean)objects.get(0);
            String cid = (String)objects.get(1);
            String cname = (String)objects.get(2);
            String tname = (String)objects.get(3);
            String credit = (String)objects.get(4);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                // INSERT INTO Course(course_id, course_name, teacher_id, teacher_name, credit)
                // SELECT "cid", "cname", teacher_id, "teacher_name", "credit"
                // FROM Teacher
                // WHERE teacher_name = tname
                // 输入学院名和院长名，自动填充院长ID，如果没有该院长，则创建不成功
                String sql =    "INSERT INTO Course(course_id, course_name, teacher_id, teacher_name, credit) " +
                        "SELECT \"" + cid + "\", " + "\"" + cname + "\", teacher_id, teacher_name, \"" + credit + "\" " +
                        "FROM Teacher " +
                        "WHERE teacher_name = \"" + tname + "\"";
                stmt.executeUpdate(sql);

                refreshTableCourse();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该课程已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddCourseView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddCourseView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddCourseView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedCourse();
        String course_id = controller.getCourse_id();
        String course_name = controller.getCourse_name();
        String teacher_name = controller.getTeacher_name();
        String credit = controller.getCredit();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(course_id);
        objects.add(course_name);
        objects.add(teacher_name);
        objects.add(credit);
        return objects;
    }
}
