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
import model.Student;

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
public class StudentView {
    @FXML
    private TableView<Student> tableView;
    @FXML
    private TableColumn<Student, String> student_id;
    @FXML
    private TableColumn<Student, String> student_name;
    @FXML
    private TableColumn<Student, Integer> apartment_id;
    @FXML
    private TableColumn<Student, String> apartment_name;
    @FXML
    private TableColumn<Student, Integer> class_id;
    @FXML
    private TableColumn<Student, String> class_name;
    @FXML
    private TableColumn<Student, Integer> college_id;
    @FXML
    private TableColumn<Student, String> college_name;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Student> studentData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showStudent(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM student";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
            studentData.add(new Student(rs.getString(1),                  rs.getString(2),
                                        Integer.valueOf(rs.getString(3)), rs.getString(4),
                                        Integer.valueOf(rs.getString(5)), rs.getString(6),
                                        Integer.valueOf(rs.getString(7)), rs.getString(8)));
            }
            student_id.setCellValueFactory( new PropertyValueFactory<>("student_id") );
            student_name.setCellValueFactory( new PropertyValueFactory<>("student_name") );
            apartment_id.setCellValueFactory( new PropertyValueFactory<>("apartment_id") );
            apartment_name.setCellValueFactory( new PropertyValueFactory<>("apartment_name") );
            class_id.setCellValueFactory( new PropertyValueFactory<>("class_id") );
            class_name.setCellValueFactory( new PropertyValueFactory<>("class_name") );
            college_id.setCellValueFactory( new PropertyValueFactory<>("college_id") );
            college_name.setCellValueFactory( new PropertyValueFactory<>("college_name") );
            tableView.setItems(studentData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableStudent(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM student";
            ResultSet rs = stmt.executeQuery(sql);
            studentData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                studentData.add(new Student(rs.getString(1),                  rs.getString(2),
                                            Integer.valueOf(rs.getString(3)), rs.getString(4),
                                            Integer.valueOf(rs.getString(5)), rs.getString(6),
                                            Integer.valueOf(rs.getString(7)), rs.getString(8)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteStudent() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Student " +
                                "WHERE student_id = " + tableView.getItems().get(selectedIndex).getStudent_id();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
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

    /**
     * Called when the user clicks the new button. Opens a alert to edit
     * details for a new dean.
     */
    @FXML
    private void handleNewStudent() {
        try{
            List<Object> objects = this.showAddStudentView();
            boolean okClicked = (Boolean)objects.get(0);
            String sid = (String)objects.get(1);
            String sname = (String)objects.get(2);
            String aname = (String)objects.get(3);
            String clname = (String)objects.get(4);
            String coname = (String)objects.get(5);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                // INSERT INTO Student(student_id, student_name, apartment_id, apartment_name, class_id, class_name, college_id, college_name)
                // SELECT "sid", "sname", apartment_id, apartment_name, class_id, class_name, college_id, college_name
                // FROM Apartment, Class, College
                // WHERE aname = Apartment.apartment_name AND clname = Class.class_name AND coname = College.college_name
                // e.g.
                // INSERT INTO Student(student_id, student_name, apartment_id, apartment_name, class_id, class_name, college_id, college_name)
                // SELECT "7203610712", "番茄元", apartment_id, apartment_name, class_id, class_name, college_id, college_name
                // FROM Apartment, Class, College
                // WHERE "A01" = Apartment.apartment_name AND "20W0314" = Class.class_name AND "HITTT" = College.college_name
                // 如果外键为空，则不插入该学生
                String sql =    "INSERT INTO Student(student_id, student_name, apartment_id, apartment_name, class_id, class_name, college_id, college_name) " +
                                "SELECT \"" + sid + "\", \"" + sname + "\", apartment_id, apartment_name, class_id, class_name, college_id, college_name " +
                                "FROM Apartment, Class, College " +
                                "WHERE \"" + aname + "\" = Apartment.apartment_name AND \"" + clname + "\" = Class.class_name AND \"" + coname + "\" = College.college_name";
                stmt.executeUpdate(sql);

                refreshTableStudent();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该学生已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddStudentView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddStudentView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddStudentView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedStudent();
        String student_id = controller.getStudent_id();
        String student_name = controller.getStudent_name();
        String apartment_name = controller.getApartment_name();
        String class_name = controller.getClass_name();
        String college_name = controller.getCollege_name();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(student_id);
        objects.add(student_name);
        objects.add(apartment_name);
        objects.add(class_name);
        objects.add(college_name);
        return objects;
    }
}
