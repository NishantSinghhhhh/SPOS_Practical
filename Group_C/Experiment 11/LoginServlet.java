import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class LoginServlet extends HttpServlet {

    public void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        String uname = req.getParameter("uname");
      String pass = req.getParameter("pass");
        String remember = req.getParameter("remember");

        res.setContentType("text/html");
      PrintWriter pw = res.getWriter();

        if(uname.equals("admin") && pass.equals("1234")) {

            HttpSession session = req.getSession();
          session.setAttribute("username", uname);
            session.setMaxInactiveInterval(30*60);

            if(remember != null) {
          Cookie c1 = new Cookie("username", uname);
                Cookie c2 = new Cookie("password", pass);
                c1.setMaxAge(7*24*60*60);
          c2.setMaxAge(7*24*60*60);
                res.addCookie(c1);
                res.addCookie(c2);
          pw.println("<p style='color:green'>Cookie saved for 7 days!</p>");
            }

            String encodedURL = res.encodeURL("welcome.jsp");

          pw.println("<html><body>");
            pw.println("<h2 style='color:green'>Login Successful!</h2>");
            pw.println("<p>Welcome, " + uname + "!</p>");
          pw.println("<p>Session ID: " + session.getId() + "</p>");
            pw.println("<a href='" + encodedURL + "'>Go to Welcome Page</a>");
            pw.println("</body></html>");

      } else {
            pw.println("<html><body>");
            pw.println("<h2 style='color:red'>Login Failed!</h2>");
          pw.println("<a href='index.html'>Try Again</a>");
            pw.println("</body></html>");
        }

    pw.close();
    }
}