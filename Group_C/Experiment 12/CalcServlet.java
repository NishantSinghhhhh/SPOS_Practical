import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class CalcServlet extends HttpServlet {

    public void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {

        double n1 = Double.parseDouble(req.getParameter("num1"));
      double n2 = Double.parseDouble(req.getParameter("num2"));
        String op = req.getParameter("op");

        double result = 0;
      String opSymbol = "";
        String error = "";

        switch(op) {
      case "add": result = n1 + n2; opSymbol = "+"; break;
            case "sub": result = n1 - n2; opSymbol = "-"; break;
            case "mul": result = n1 * n2; opSymbol = "*"; break;
      case "div":
                if(n2 == 0) error = "Cannot divide by Zero!";
                else { result = n1 / n2; opSymbol = "/"; }
          break;
            case "mod":
                if(n2 == 0) error = "Cannot mod by Zero!";
          else { result = n1 % n2; opSymbol = "%"; }
                break;
        }

      res.setContentType("text/html");
        PrintWriter pw = res.getWriter();
        pw.println("<html><body>");
      pw.println("<div style='text-align:center; font-family:Arial; margin-top:80px'>");
        pw.println("<div style='background:white; width:350px; margin:auto; padding:30px;" +
            "border-radius:10px; box-shadow:0 0 10px gray'>");
      pw.println("<h2>Calculator Result</h2>");

        if(!error.isEmpty()) {
            pw.println("<h3 style='color:red'>" + error + "</h3>");
      } else {
            pw.println("<h3>" + n1 + " " + opSymbol + " " + n2 + " = <span style='color:green'>" + result + "</span></h3>");
        }

    pw.println("<br><a href='index.html'>Calculate Again</a>");
        pw.println("</div></div>");
        pw.println("</body></html>");
    pw.close();
    }
}