import java.net.*;

public class Experiment6 {
    public static void main(String[] args) {
    try {
            InetAddress localIP = InetAddress.getLocalHost();
            System.out.println("=== Local Machine ===");
        System.out.println("Host Name : " + localIP.getHostName());
            System.out.println("IP Address: " + localIP.getHostAddress());

        byte[] rawBytes = localIP.getAddress();
            System.out.print("Byte Array: ");
            for(byte b : rawBytes)
        System.out.print((b & 0xFF) + " ");
            System.out.println();

            String[] sites = {"www.google.com", "www.github.com", "www.youtube.com"};

        for(String site : sites) {
                System.out.println("\n=== " + site + " ===");
                InetAddress ip = InetAddress.getByName(site);
            System.out.println("IP: " + ip.getHostAddress());

                boolean reachable = ip.isReachable(3000);
            System.out.println("Reachable: " + reachable);

                InetAddress[] allAddr = InetAddress.getAllByName(site);
                System.out.println("All IPs:");
            for(InetAddress addr : allAddr)
                    System.out.println("  -> " + addr.getHostAddress());
            }

    } catch(Exception e) {
            e.printStackTrace();
        }
    }
}