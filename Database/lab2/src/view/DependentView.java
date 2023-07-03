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
import model.Dependent;

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
public class DependentView {
    @FXML
    private TableView<Dependent> tableView;
    @FXML
    private TableColumn<Dependent, String> student_id;
    @FXML
    private TableColumn<Dependent, String> student_name;
    @FXML
    private TableColumn<Dependent, String> dependent_name;
    @FXML
    private TableColumn<Dependent, String> relationship;

    // 维持数据库连接的对象
    private Connection conn;
    private ObservableList<Dependent> dependentData = FXCollections.observableArrayList();
    public void setConnection(Connection conn){
        this.conn = conn;
    }

    public void showDependent(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Dependent";
            ResultSet rs = stmt.executeQuery(sql);
            //遍历查询的结果集
            while (rs.next()) {
                dependentData.add(new Dependent(rs.getString(1), rs.getString(2),
                                                rs.getString(3), rs.getString(4)));
            }
            dependent_name.setCellValueFactory( new PropertyValueFactory<>("dependent_name") );
            student_id.setCellValueFactory( new PropertyValueFactory<>("student_id") );
            student_name.setCellValueFactory( new PropertyValueFactory<>("student_name") );
            relationship.setCellValueFactory( new PropertyValueFactory<>("relationship") );
            tableView.setItems(dependentData);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public void refreshTableDependent(){
        try{
            Statement stmt  = conn.createStatement();
            String sql =    "SELECT * " +
                            "FROM Dependent";
            ResultSet rs = stmt.executeQuery(sql);
            dependentData.clear();
            //遍历查询的结果集
            while (rs.next()) {
                dependentData.add(new Dependent(rs.getString(1), rs.getString(2),
                                                rs.getString(3), rs.getString(4)));
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void handleDeleteDependent() {
        try{
            int selectedIndex = tableView.getSelectionModel().getSelectedIndex();
            if (selectedIndex >= 0) {
                // 先删除数据库
                Statement stmt  = conn.createStatement();
                String sql =    "DELETE FROM Dependent " +
                                "WHERE dependent_name=" + tableView.getItems().get(selectedIndex).getDependent_name();
                stmt.executeUpdate(sql);
                // 再删除显示的表
                tableView.getItems().remove(selectedIndex);
            } else {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("Please select a dependent in the table.");
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
            List<Object> objects = this.showAddDependentView();
            boolean okClicked = (Boolean)objects.get(0);
            String my_dependent_name = (String)objects.get(1);
            String my_student_name = (String)objects.get(2);
            String my_relationship = (String)objects.get(3);
            if (okClicked) {
                // 给数据库添加
                Statement stmt  = conn.createStatement();
                conn.setAutoCommit(false);  //将自动提交设置为false
                // INSERT INTO Dependent(dependent_name, student_id, student_name, relationship)
                // SELECT "my_dependent_name", student_id, student_name, "my_relationship"
                // FROM Student AS S
                // WHERE "my_student_name" = S.student_name
                // E.g.
                // INSERT INTO Dependent(dependent_name, student_id, student_name, relationship)
                // SELECT "大番茄", student_id, student_name, "父亲"
                // FROM Student AS S
                // WHERE "番茄元" = S.student_name
                // 家属名，关系，和对应的学生，如果没有对应学生，则创建不成功
                String sql =    "INSERT INTO Dependent(dependent_name, student_id, student_name, relationship) " +
                                "SELECT \"" + my_dependent_name + "\", student_id, student_name, \"" + my_relationship + "\" " +
                                "FROM Student AS S " +
                                "WHERE \"" + my_student_name + "\" = S.student_name";
                stmt.executeUpdate(sql);

                refreshTableDependent();
                conn.commit();
                conn.setAutoCommit(true);
            }
        }catch(SQLIntegrityConstraintViolationException e){
            try{
                conn.rollback();
                conn.setAutoCommit(true);
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setHeaderText("由于唯一性索引约束，部分域非法导致添加失败！");
                alert.setContentText("该家属已经存在，请修改！");
                alert.showAndWait();
            }catch(Exception e1){
                e1.printStackTrace();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    private List<Object> showAddDependentView() throws Exception{
        // Load root layout from fxml file.
        FXMLLoader loader = new FXMLLoader();
        loader.setLocation(Main.class.getResource("AddDependentView.fxml"));
        AnchorPane rootLayout = (AnchorPane) loader.load();

        // Show the scene containing the root layout.
        Scene scene = new Scene(rootLayout);
        Stage stage = new Stage();
        stage.setScene(scene);
        // Give the controller access to the main app.
        AddDependentView controller = loader.getController();
        controller.setStage(stage);
        stage.showAndWait();

        // 返回ok状态以及用户填写的内容
        boolean flag =  controller.isOkClickedDependent();
        String dependent_name = controller.getDependent_name();
        String student_name = controller.getStudent_name();
        String relationship = controller.getRelationship();
        List<Object> objects = new ArrayList<>();
        objects.add(flag);
        objects.add(dependent_name);
        objects.add(student_name);
        objects.add(relationship);
        return objects;
    }
}
