import java.net.*;

public class Experiment6 {
    public static void main(String[] args) {
      try {
            InetAddress ip = InetAddress.getLocalHost();
            System.out.println("Host Name: " + ip.getHostName());
          System.out.println("IP Address: " + ip.getHostAddress());

            InetAddress ip2 = InetAddress.getByName("www.google.com");
          System.out.println("Google IP: " + ip2.getHostAddress());

            InetAddress[] all = InetAddress.getAllByName("www.google.com");
            for(InetAddress addr : all) {
              System.out.println("Address: " + addr.getHostAddress());
            }
        } catch(UnknownHostException e) {
          e.printStackTrace();
        }
    }
}