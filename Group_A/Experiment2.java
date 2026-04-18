import java.awt.*;
import java.awt.event.*;

public class Experiment2 extends Frame implements MouseListener, MouseMotionListener {
    String msg = "";
    int clickCount = 0;
    int mouseX = 0, mouseY = 0;

    public Experiment2() {
    setTitle("Mouse Event Frame");
        setSize(500, 400);
        addMouseListener(this);
    addMouseMotionListener(this);
        setVisible(true);
    }

  public void mouseEntered(MouseEvent e) {
        setBackground(Color.lightGray);
        msg = "Mouse Entered";
    repaint();
    }

    public void mouseExited(MouseEvent e) {
        setBackground(Color.white);
    msg = "Mouse Exited";
        repaint();
    }

    public void mouseClicked(MouseEvent e) {
    clickCount++;
        msg = "Clicked at: (" + e.getX() + ", " + e.getY() + ")";
        repaint();
  }

    public void mouseMoved(MouseEvent e) {
        mouseX = e.getX();
        mouseY = e.getY();
    repaint();
    }

    public void mousePressed(MouseEvent e) {
        setBackground(Color.pink);
    repaint();
    }

    public void mouseReleased(MouseEvent e) {
    setBackground(Color.lightGray);
        repaint();
    }

  public void mouseDragged(MouseEvent e) {
        mouseX = e.getX();
        mouseY = e.getY();
    msg = "Dragging at: (" + mouseX + ", " + mouseY + ")";
        repaint();
    }

    public void paint(Graphics g) {
    g.setFont(new Font("Arial", Font.BOLD, 14));
        g.drawString(msg, 20, 80);
        g.drawString("Click Count: " + clickCount, 20, 110);
    g.drawString("Mouse Position: (" + mouseX + ", " + mouseY + ")", 20, 140);
    }

  public static void main(String[] args) {
        new Experiment2();
    }
}