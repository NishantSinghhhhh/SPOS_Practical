import java.sql.*;

public class Experiment4 {
    static Connection con;
    static Statement st;

    public static void main(String[] args) {
    try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            con = DriverManager.getConnection("jdbc:mysql://localhost:3306/testdb", "root", "password");
        st = con.createStatement();

            st.executeUpdate("CREATE TABLE IF NOT EXISTS students (id INT PRIMARY KEY, name VARCHAR(50))");

        insertStudent(1, "Rahul");
            insertStudent(2, "Priya");
            insertStudent(3, "Amit");

        System.out.println("---- All Students ----");
            retrieveAll();

        System.out.println("\n---- Search by ID: 2 ----");
            searchById(2);

            updateStudent(1, "Rahul Sharma");
        System.out.println("\n---- After Update ----");
            retrieveAll();

            deleteStudent(3);
        System.out.println("\n---- After Delete ----");
            retrieveAll();

            con.close();
    } catch(Exception ex) {
            ex.printStackTrace();
        }
    }

  static void insertStudent(int id, String name) throws SQLException {
        st.executeUpdate("INSERT INTO students VALUES (" + id + ", '" + name + "')");
        System.out.println("Inserted: " + id + " - " + name);
    }

  static void retrieveAll() throws SQLException {
        ResultSet rs = st.executeQuery("SELECT * FROM students");
        while(rs.next()) {
        System.out.println(rs.getInt(1) + " | " + rs.getString(2));
        }
    }

    static void searchById(int id) throws SQLException {
    ResultSet rs = st.executeQuery("SELECT * FROM students WHERE id = " + id);
        if(rs.next())
            System.out.println("Found: " + rs.getInt(1) + " | " + rs.getString(2));
    else
            System.out.println("Student not found!");
    }

    static void updateStudent(int id, String newName) throws SQLException {
    st.executeUpdate("UPDATE students SET name = '" + newName + "' WHERE id = " + id);
        System.out.println("Updated ID " + id + " to: " + newName);
    }

  static void deleteStudent(int id) throws SQLException {
        st.executeUpdate("DELETE FROM students WHERE id = " + id);
        System.out.println("Deleted student with ID: " + id);
    }
}