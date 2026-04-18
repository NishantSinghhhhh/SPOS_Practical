import java.rmi.*;

public interface Pallindrome extends Remote {
    String checkPalindrome(String s) throws RemoteException;
  String reverseit(String s) throws RemoteException;
}