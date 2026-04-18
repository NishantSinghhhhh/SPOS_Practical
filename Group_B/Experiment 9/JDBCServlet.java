import java.io.*;
import java.sql.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class JDBCServlet extends HttpServlet {
    String url = "jdbc:mysql://localhost:3306/testdb";
    String user = "root";
  String pass = "password";

    public void init() throws ServletException {
        try {
      Class.forName("com.mysql.cj.jdbc.Driver");
        } catch(ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

  public void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        String action = req.getParameter("action");
      String sid = req.getParameter("sid");
        String sname = req.getParameter("sname");
        String course = req.getParameter("course");

    res.setContentType("text/html");
        PrintWriter pw = res.getWriter();
        pw.println("<html><body><h2>JDBC Result</h2>");

      try {
            Connection con = DriverManager.getConnection(url, user, pass);
            Statement st = con.createStatement();

        st.executeUpdate("CREATE TABLE IF NOT EXISTS students " +
                "(id INT PRIMARY KEY, name VARCHAR(50), course VARCHAR(50))");

            if(action.equals("Insert")) {
          st.executeUpdate("INSERT INTO students VALUES (" + sid + ", '" + sname + "', '" + course + "')");
                pw.println("<p style='color:green'>Student Inserted Successfully!</p>");
            }
          else if(action.equals("Search")) {
                ResultSet rs = st.executeQuery("SELECT * FROM students WHERE id = " + sid);
                if(rs.next()) {
            pw.println("<p>ID: " + rs.getInt(1) + "</p>");
                    pw.println("<p>Name: " + rs.getString(2) + "</p>");
                    pw.println("<p>Course: " + rs.getString(3) + "</p>");
          } else {
                    pw.println("<p style='color:red'>Student Not Found!</p>");
                }
            }
          else if(action.equals("Delete")) {
                st.executeUpdate("DELETE FROM students WHERE id = " + sid);
                pw.println("<p style='color:orange'>Student Deleted!</p>");
          }
            else if(action.equals("ViewAll")) {
                ResultSet rs = st.executeQuery("SELECT * FROM students");
                pw.println("<table border='1'><tr><th>ID</th><th>Name</th><th>Course</th></tr>");
          while(rs.next()) {
                    pw.println("<tr><td>" + rs.getInt(1) + "</td><td>" + 
                        rs.getString(2) + "</td><td>" + rs.getString(3) + "</td></tr>");
                }
          pw.println("</table>");
            }

            con.close();
        } catch(Exception ex) {
      pw.println("<p style='color:red'>Error: " + ex.getMessage() + "</p>");
        }

        pw.println("<br><a href='index.html'>Go Back</a>");
      pw.println("</body></html>");
        pw.close();
    }
}