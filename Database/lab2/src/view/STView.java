package view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import model.ClassInfo;
import model.StudentTeacher;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

/**
 * @author 番茄元
 * @date 2023/3/31
 */
public class STView {
    @FXML
    private TableView<StudentTeacher> tableView;
    @FXML
    private TableColumn<StudentTeacher, String> student_id;
    @FXML
    private TableColumn<StudentTeacher, String> student_name;
    @FXML
    private TableColumn<StudentTeacher, String> course_name;
    @FXML
    private TableColumn<StudentTeacher, String> teacher_name;

    // 维持数据库连接的对象
    private Connection conn;

    private ObservableList<StudentTeacher> STData = FXCollections.observableArrayList();

    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showST(){
        try{
            Statement stmt  = conn.createStatement();
            // 创建视图
            // CREATE VIEW StudentTeacher AS
            // SELECT S.student_id, S.student_name, S.course_name, C.teacher_name
            // FROM SC S INNER JOIN Course C
            // ON S.course_id = C.course_id;

            String sql =    "SELECT * " +
                            "FROM StudentTeacher";
            ResultSet rs = stmt.executeQuery(sql);
            STData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                STData.add(new StudentTeacher(  rs.getString(1), rs.getString(2),
                                                rs.getString(3), rs.getString(4)));
            }
            student_id.setCellValueFactory( new PropertyValueFactory<>("student_id") );
            student_name.setCellValueFactory( new PropertyValueFactory<>("student_name") );
            course_name.setCellValueFactory( new PropertyValueFactory<>("course_name") );
            teacher_name.setCellValueFactory( new PropertyValueFactory<>("teacher_name") );
            tableView.setItems(STData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

}
