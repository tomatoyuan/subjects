package view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import model.ClassInfo;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

/**
 * @author 番茄元
 * @date 2023/3/30
 */
public class ClassInfoView {
    @FXML
    private TableView<ClassInfo> tableView;
    @FXML
    private TableColumn<ClassInfo, Integer> class_id;
    @FXML
    private TableColumn<ClassInfo, String> class_name;
    @FXML
    private TableColumn<ClassInfo, Integer> tot_num;


    // 维持数据库连接的对象
    private Connection conn;

    private ObservableList<ClassInfo> ClassInfoData = FXCollections.observableArrayList();

    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showClassInfo(){
        try{
            Statement stmt  = conn.createStatement();
            // 创建视图
            // CREATE VIEW ClassInfo AS
            // SELECT Class.class_id AS cid, Class.class_name AS cname, COUNT(*) AS tot_num
            // FROM Student, Class
            // GROUP BY Class.class_id;

            String sql =    "SELECT * " +
                            "FROM ClassInfo";
            ResultSet rs = stmt.executeQuery(sql);
            ClassInfoData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                ClassInfoData.add(new ClassInfo(Integer.valueOf(rs.getString(1)), rs.getString(2), Integer.valueOf(rs.getString(3))));
            }
            class_id.setCellValueFactory( new PropertyValueFactory<>("class_id") );
            class_name.setCellValueFactory( new PropertyValueFactory<>("class_name") );
            tot_num.setCellValueFactory( new PropertyValueFactory<>("tot_num") );
            tableView.setItems(ClassInfoData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}
