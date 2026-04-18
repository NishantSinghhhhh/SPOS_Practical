import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class ClientServlet extends HttpServlet {

    public void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        String uname = req.getParameter("uname");
        String pass = req.getParameter("pass");

      res.setContentType("text/html");
        PrintWriter pw = res.getWriter();

    pw.println("<html><body>");
        pw.println("<h2>Client Side Display</h2>");
        pw.println("<p>Username: " + uname + "</p>");
      pw.println("<p>Password: " + pass + "</p>");
        pw.println("</body></html>");

        pw.close();
    }
}