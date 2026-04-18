import java.rmi.*;

public interface RemoteInterface extends Remote {
    String getMessage(String input) throws RemoteException;
  int addNumbers(int a, int b) throws RemoteException;
}