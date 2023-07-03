package view;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;
import model.ClassInfo;

import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;

/**
 * @author 番茄元
 * @date 2023/3/29
 */
public class sample {

    private String url= "jdbc:mysql://localhost:3306/school";//JDBC的URL，在此处标明所使用的数据库
    private String rootName = "root";
    private String pwd ="1230";
    private Connection conn;

    public  sample(){
        try{
            Class.forName("com.mysql.cj.jdbc.Driver");
            //建立数据库连接
            conn = DriverManager.getConnection(url,rootName,pwd);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    @FXML
    private void ManageApartment(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("ApartmentView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            ApartmentView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showApartment();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("公寓信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    private void ManageClass(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("ClassView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            ClassView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showClass();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("班级信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    private void ManageMajor(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("MajorView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            MajorView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showMajor();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("专业信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @FXML
    private void ManageCollege(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("CollegeView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            CollegeView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showCollege();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("学院信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @FXML
    private void ManageDean(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("DeanView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            DeanView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showDean();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("院长信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @FXML
    private void ManageTeacher(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("TeacherView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            TeacherView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showTeacher();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("教师信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @FXML
    private void ManageStudent(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("StudentView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            StudentView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showStudent();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("学生信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @FXML
    private void ManageDependent(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("DependentView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            DependentView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showDependent();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("家属信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    @FXML
    private void ManageCourse(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("CourseView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            CourseView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showCourse();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("课程信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    // 管理学生成绩
    @FXML
    private void ManageSC(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("SCView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            SCView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showSC();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("学生成绩信息");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    private void ManageClassInfo(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("ClassInfoView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            ClassInfoView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showClassInfo();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("班级学生数量视图");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @FXML
    private void ManageST(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("STView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            STView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showST();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("学生课程对应教师视图");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    @FXML
    private void ManageApartmentQuery(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("ApartmentQuery.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            ApartmentQuery controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showApartmentStudent();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("公寓入住情况查询");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @FXML
    private void ManageStuRecycleBin(){
        try {
            // Load root layout from fxml file.
            FXMLLoader loader = new FXMLLoader();
            loader.setLocation(Main.class.getResource("StuRecycleBinView.fxml"));
            AnchorPane rootLayout = (AnchorPane) loader.load();
            // Give the controller access to the main app.
            StuRecycleBinView controller = loader.getController();
            controller.setConnection(this.conn);
            controller.showStuRecycleBin();
            // Show the scene containing the root layout.
            Scene scene = new Scene(rootLayout);
            Stage stage = new Stage();
            stage.setTitle("已删学生信息存档");
            stage.setScene(scene);
            stage.showAndWait();

        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
