import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class LogoutServlet extends HttpServlet {

  public void doGet(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        HttpSession session = req.getSession(false);
      if(session != null)
            session.invalidate();

        Cookie[] cookies = req.getCookies();
      if(cookies != null) {
            for(Cookie c : cookies) {
                c.setMaxAge(0);
          res.addCookie(c);
            }
        }

    res.setContentType("text/html");
        PrintWriter pw = res.getWriter();
        pw.println("<html><body>");
      pw.println("<h2 style='color:orange'>Logged Out Successfully!</h2>");
        pw.println("<a href='index.html'>Login Again</a>");
        pw.println("</body></html>");
    pw.close();
    }
}