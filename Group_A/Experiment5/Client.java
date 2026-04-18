import java.rmi.*;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
    try {
            Pallindrome obj = (Pallindrome) Naming.lookup("rmi://localhost/PalinCheck");
            Scanner sc = new Scanner(System.in);

        System.out.println("Enter -1 to stop");
            while(true) {
                System.out.print("Enter string or number: ");
            String input = sc.nextLine();
                if(input.equals("-1")) break;
                System.out.println(obj.checkPalindrome(input));
            System.out.println("Reversed: " + obj.reverseit(input));
                System.out.println();
            }

        sc.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }
}