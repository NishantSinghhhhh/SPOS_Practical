import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class ServerServlet extends HttpServlet {
    String validUser = "admin";
    String validPass = "1234";

  public void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        String uname = req.getParameter("uname");
      String pass = req.getParameter("pass");

        res.setContentType("text/html");
        PrintWriter pw = res.getWriter();

    pw.println("<html><body>");
        pw.println("<h2>Server Side Verification</h2>");

      if(uname.equals(validUser) && pass.equals(validPass)) {
            pw.println("<h3 style='color:green'>Login Successful!</h3>");
            pw.println("<p>Welcome, " + uname + "!</p>");
      } else {
            pw.println("<h3 style='color:red'>Login Failed!</h3>");
            pw.println("<p>Invalid Username or Password</p>");
        }

    pw.println("</body></html>");
        pw.close();
    }
}