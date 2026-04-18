import java.awt.*;
import java.awt.event.*;

public class Experiment3 extends Frame implements ActionListener {
    Label l1,l2,l3,l4,l5;
    TextField t1,t2,t3,t4,t5;
    Button b1, b2;

    public Experiment3() {
    setTitle("Student Marks - 5 Subjects");
        setSize(400, 380);
        setLayout(new FlowLayout());

    l1 = new Label("Maths:");
        l2 = new Label("Science:");
        l3 = new Label("English:");
    l4 = new Label("History:");
        l5 = new Label("Computer:");

        t1 = new TextField(10);
    t2 = new TextField(10);
        t3 = new TextField(10);
        t4 = new TextField(10);
    t5 = new TextField(10);

        b1 = new Button("Show Result");
        b2 = new Button("Clear");

    add(l1); add(t1);
        add(l2); add(t2);
        add(l3); add(t3);
    add(l4); add(t4);
        add(l5); add(t5);
        add(b1); add(b2);

    b1.addActionListener(this);
        b2.addActionListener(this);
        setVisible(true);
    }

  public void actionPerformed(ActionEvent e) {
        if(e.getSource() == b2) {
            t1.setText(""); t2.setText("");
        t3.setText(""); t4.setText("");
            t5.setText("");
            return;
        }

    int m1 = Integer.parseInt(t1.getText());
        int m2 = Integer.parseInt(t2.getText());
        int m3 = Integer.parseInt(t3.getText());
    int m4 = Integer.parseInt(t4.getText());
        int m5 = Integer.parseInt(t5.getText());

        int total = m1+m2+m3+m4+m5;
    double perc = total/5.0;

        String grade;
        if(perc >= 75) grade = "A";
    else if(perc >= 60) grade = "B";
        else if(perc >= 50) grade = "C";
        else if(perc >= 35) grade = "D";
    else grade = "F";

        String res = perc >= 35 ? "Pass" : "Fail";

    Frame f2 = new Frame("Result Window");
        f2.setSize(320, 250);
        f2.setLayout(new FlowLayout());
    f2.add(new Label("Total Marks: " + total + " / 500"));
        f2.add(new Label("Percentage: " + perc + "%"));
        f2.add(new Label("Grade: " + grade));
    f2.add(new Label("Result: " + res));
        f2.setVisible(true);
    }

  public static void main(String[] args) {
        new Experiment3();
    }
}