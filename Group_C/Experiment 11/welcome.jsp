<%@ page language="java" %>
<!DOCTYPE html>
<html>
<head>
    <title>Welcome</title>
    <style>
        body { font-family: Arial; text-align: center; background: #f0f0f0; }
        .box { background: white; width: 400px; margin: 80px auto; padding: 30px;
               border-radius: 10px; box-shadow: 0 0 10px gray; }
    </style>
</head>
<body>
    <div class="box">
        <%
            HttpSession s = request.getSession(false);
            String user = (String) s.getAttribute("username");

            Cookie[] cookies = request.getCookies();
            String cookieUser = "No Cookie Found";
            if(cookies != null) {
                for(Cookie c : cookies) {
                    if(c.getName().equals("username"))
                        cookieUser = c.getValue();
                }
            }
        %>

        <h2>Welcome, <%= user %>!</h2>
        <p>Session ID: <%= s.getId() %></p>
        <p>Cookie Username: <%= cookieUser %></p>
        <p>URL Rewriting is active on this page</p>
        <br>
        <a href="LogoutServlet">Logout</a>
    </div>
</body>
</html>