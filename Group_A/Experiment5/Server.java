import java.rmi.*;
import java.rmi.server.*;

public class Server extends UnicastRemoteObject implements Pallindrome {

  public Server() throws RemoteException {}

    public String checkPalindrome(String s) throws RemoteException {
        String rev = new StringBuilder(s).reverse().toString();
    if(s.equalsIgnoreCase(rev))
            return s + " is a Palindrome  |  Reverse: " + rev;
        return s + " is NOT a Palindrome  |  Reverse: " + rev;
    }

  public String reverseit(String s) throws RemoteException {
        return new StringBuilder(s).reverse().toString();
    }

    public static void main(String[] args) {
    try {
            Server obj = new Server();
            Naming.rebind("PalinCheck", obj);
        System.out.println("Server is running...");
        } catch(Exception e) {
            e.printStackTrace();
    }
    }
}