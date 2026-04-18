import java.rmi.*;
import java.rmi.server.*;
import java.rmi.registry.*;

public class RMIServer extends UnicastRemoteObject implements RemoteInterface {

  public RMIServer() throws RemoteException {}

    public String getMessage(String input) throws RemoteException {
        return "Server received: " + input + " | Response: Hello from Server!";
    }

  public int addNumbers(int a, int b) throws RemoteException {
        return a + b;
    }

    public static void main(String[] args) {
    try {
            if(System.getSecurityManager() == null) {
                System.setSecurityManager(new SecurityManager());
          }

            RMIServer obj = new RMIServer();
            Registry reg = LocateRegistry.createRegistry(1099);
        reg.rebind("MyRemote", obj);

            System.out.println("RMI Server is running on port 1099...");
        } catch(Exception e) {
      e.printStackTrace();
        }
    }
}