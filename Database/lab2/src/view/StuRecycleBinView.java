package view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import model.Student;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

/**
 * @author 番茄元
 * @date 2023/3/31
 */
public class StuRecycleBinView {
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

    public void showStuRecycleBin(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM StuRecycleBin";
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

}
