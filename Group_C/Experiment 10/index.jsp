<%@ page language="java" contentType="text/html; charset=UTF-8" %>
<!DOCTYPE html>
<html>
<head>
    <title>Simple JSP Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            text-align: center;
        }
        .box {
            background-color: white;
            width: 400px;
            margin: 100px auto;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px gray;
        }
        h1 { color: #3366cc; }
        p { color: #555; }
    </style>
</head>
<body>
    <div class="box">
        <h1>Welcome to JSP!</h1>
        <p>This is a simple JSP page displaying a message.</p>

        <%
            String msg = "Hello from the Server Side!";
            java.util.Date date = new java.util.Date();
        %>

        <h3><%= msg %></h3>
        <p>Current Date and Time: <%= date %></p>
        <p>Server Info: <%= application.getServerInfo() %></p>
    </div>
</body>
</html>