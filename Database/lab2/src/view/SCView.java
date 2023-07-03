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
import model.SC;
import model.Student;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLIntegrityConstraintViolationException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

/** 学生选课视图
 * @author 番茄元
 * @date 2023/3/29
 */
public class SCView {
    @FXML
    private TableView<SC> SCTable;
    @FXML
    private TableColumn<SC, String> student_id;
    @FXML
    private TableColumn<SC, String> student_name;
    @FXML
    private TableColumn<SC, String> course_id;
    @FXML
    private TableColumn<SC, String> course_name;
    @FXML
    private TableColumn<SC, String> grade;

    // 维持数据库连接的对象
    private Connection conn;

    private ObservableList<SC> SCData = FXCollections.observableArrayList();

    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showSC(){
        try{
            Statement stmt  = conn.createStatement();
            // CREATE VIEW [Schedule] AS
            // SELECT student_id, student_name, course_id, course_name, teacher_id, teacher_name, credit
            // FROM Student, Course
            // WHERE Student.name=
            String sql =    "SELECT Student.student_id, Student.student_name, Course.course_id, Course.course_name, SC.grade " +
                            "FROM Student, Course, SC "+
                            "WHERE SC.student_id = Student.student_id AND SC.course_id = Course.course_id";
            ResultSet rs = stmt.executeQuery(sql);
            SCData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                SCData.add(new SC(  rs.getString(1), rs.getString(2),
                                    rs.getString(3), rs.getString(4),
                                    rs.getString(5)));
            }
            student_id.setCellValueFactory( new PropertyValueFactory<>("student_id") );
            student_name.setCellValueFactory( new PropertyValueFactory<>("student_name") );
            course_id.setCellValueFactory( new PropertyValueFactory<>("course_id") );
            course_name.setCellValueFactory( new PropertyValueFactory<>("course_name") );
            grade.setCellValueFactory( new PropertyValueFactory<>("grade") );
            SCTable.setItems(SCData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableSC(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM SC";
            ResultSet rs = stmt.executeQuery(sql);
            SCData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                SCData.add(new SC(  rs.getString(1), rs.getString(2),
                                    rs.getString(3), rs.getString(4),
                                    rs.getString(5)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteSC() {
        try{
            int selectedIndex = SCTable.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM SC " +
                                "WHERE student_id=" + SCTable.getItems().get(selectedIndex).getStudent_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                SCTable.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select a student in the table.");
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

    @FXML
    public void handleAddSC(){
        try{
            List<Object> objects = this.showAddSCView();
            boolean okClicked = (Boolean)objects.get(0);
            String studentName = (String)objects.get(1);
            String courseName = (String)objects.get(2);
            String scGrade = (String)objects.get(3);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                // 先找到学生id和课程id
                String sql =    "SELECT student_id "+
                                "FROM Student "+
                                "WHERE student_name = \""+studentName+"\"";
                ResultSet rs = stmt.executeQuery(sql);
                String studentID = "";
                //遍历查询的结果集
                while (rs.next()) {
                    studentID = rs.getString(1);
                }

                String sq2 =    "SELECT course_id "+
                                "FROM course "+
                                "WHERE course_name = \""+courseName+"\"";
                ResultSet rs2 = stmt.executeQuery(sq2);
                String courseID = "";
                //遍历查询的结果集
                while (rs2.next()) {
                    courseID = rs2.getString(1);
                }
                // 由于学生姓名和课程名可以通过对应的表查到，这里尝试直接通过对象的值显示在表格中，但是不存入数据库
                String sq3 =    "INSERT into SC(student_id, student_name, course_id, course_name, grade) "+
                                "VALUES (\"" + studentID + "\", \""+ studentName + "\", \"" + courseID + "\", \"" + courseName + "\", \"" + scGrade + "\")";
                stmt.executeUpdate(sq3);
                this.showSC();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于外键约束，部分域非法导致添加失败！");
                alert.setContentText("您的输入存在非法字段（学生名、课程名必须是真实存在的）！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddSCView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddSC.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddSC controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClicked();
        String student_name = controller.getStudent_name();
        String course_name = controller.getCourse_name();
        String grade = controller.getGrade();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(student_name);
        objects.add(course_name);
        objects.add(grade);
        return objects;
    }
}
