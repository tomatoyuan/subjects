package view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import model.Apartment;
import model.ApartmentStudent;
import model.Student;

import java.sql.*;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class ApartmentQuery {
    @FXML
    private TextField textField;

    @FXML
    private TableView<Student> studentTable;
    @FXML
    private TableColumn<Student, String> student_id;
    @FXML
    private TableColumn<Student, String> student_name;

    @FXML
    private TableView<ApartmentStudent> apartmentStudentTable;
    @FXML
    private TableColumn<ApartmentStudent, String> apartment_name;
    @FXML
    private TableColumn<ApartmentStudent, Integer> student_number;

    // 维持数据库连接的对象
    private Connection conn;

    private ObservableList<Student> studentData = FXCollections.observableArrayList();
    private ObservableList<ApartmentStudent> apartmentStudentData = FXCollections.observableArrayList();

    public void setConnection(Connection conn){
        this.conn = conn;
    }
    public void showApartmentStudent(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT Apartment.apartment_name, count(*) AS total " +
                            "FROM Apartment, Student "+
                            "WHERE Apartment.apartment_id = Student.apartment_id "+
                            "GROUP BY Apartment.apartment_name " +
                            "HAVING total >= 1";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                apartmentStudentData.add(new ApartmentStudent(Integer.valueOf(rs.getString(2)),rs.getString(1)));
            }
            student_number.setCellValueFactory( new PropertyValueFactory<>("student_number") );
            apartment_name.setCellValueFactory( new PropertyValueFactory<>("apartment_name") );
            apartmentStudentTable.setItems(apartmentStudentData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleOkClicked(){
        String name = textField.getText();
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT S.student_id, S.student_name, S.apartment_id, S.apartment_name, S.class_id, S.class_name, S.college_id, S.college_name " +
                            "FROM Apartment, Student AS S " +
                            "WHERE S.apartment_id = Apartment.apartment_id AND S.apartment_id IN ( " +
                            "   SELECT  Apartment.apartment_id " +
                            "   FROM Apartment " +
                            "   WHERE Apartment.apartment_name = \"" + name + "\" " +
                            ")";
            ResultSet rs = stmt.executeQuery(sql);
            studentData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                studentData.add(new Student(rs.getString(1) ,                 rs.getString(2),
                                            Integer.valueOf(rs.getString(3)), rs.getString(4),
                                            Integer.valueOf(rs.getString(5)), rs.getString(6),
                                            Integer.valueOf(rs.getString(7)), rs.getString(8)));
            }
            student_id.setCellValueFactory( new PropertyValueFactory<>("student_id") );
            student_name.setCellValueFactory( new PropertyValueFactory<>("student_name") );
            studentTable.setItems(studentData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}
