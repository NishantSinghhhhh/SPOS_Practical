import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;

public class Experiment1 extends Applet implements KeyListener {
    String msg = "";
    int pressCount = 0;
    int releaseCount = 0;

    public void init() {
    addKeyListener(this);
        setFocusable(true);
        setBackground(Color.lightGray);
    }

    public void keyPressed(KeyEvent e) {
        pressCount++;
    setBackground(Color.yellow);
        msg = "Key Down: " + e.getKeyText(e.getKeyCode()) 
            + "  Code: " + e.getKeyCode()
        + "  Press Count: " + pressCount;
        repaint();
    }

  public void keyReleased(KeyEvent e) {
        releaseCount++;
    setBackground(Color.green);
        msg = "Key Released: " + e.getKeyText(e.getKeyCode())
          + "  Code: " + e.getKeyCode()
            + "  Release Count: " + releaseCount;
        repaint();
    }

    public void keyTyped(KeyEvent e) {
    setBackground(Color.cyan);
        msg = "Key Typed: " + e.getKeyChar();
        repaint();
  }

    public void paint(Graphics g) {
        g.setColor(Color.black);
        g.setFont(new Font("Arial", Font.BOLD, 14));
    g.drawString(msg, 20, 100);
        g.drawString("Total Presses: " + pressCount, 20, 130);
        g.drawString("Total Releases: " + releaseCount, 20, 160);
    }
}