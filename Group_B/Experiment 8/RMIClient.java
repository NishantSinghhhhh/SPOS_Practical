import java.rmi.*;
import java.rmi.registry.*;

public class RMIClient {
    public static void main(String[] args) {
    try {
            if(System.getSecurityManager() == null) {
                System.setSecurityManager(new SecurityManager());
          }

            Registry reg = LocateRegistry.getRegistry("localhost", 1099);
            RemoteInterface obj = (RemoteInterface) reg.lookup("MyRemote");

        System.out.println(obj.getMessage("Hello Server!"));
            System.out.println("Sum of 10 + 20 = " + obj.addNumbers(10, 20));

        } catch(Exception e) {
            e.printStackTrace();
        }
    }
}