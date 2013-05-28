package gui;

import static javax.swing.JFrame.setDefaultLookAndFeelDecorated;

import javax.swing.*;

import java.util.Calendar;
import javax.swing.border.BevelBorder;

import java.awt.*;
import java.awt.geom.*;
import java.util.Observable;
import java.util.Observer;
import java.util.Stack;

import visitors.*;

/**
 * A panel for drawing the picture onto. This follows the Observer
 * pattern, watching the Model and updating itself when the
 * Model changes.
 *
 * @author created by Ian Barnes modified Alexie Khorev
 * @author rewritten into Gaga Chris Johnson
 * @version $Revision: 1532 $ 
 * $Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $ 30/4/2011
 */
@SuppressWarnings("serial")
public class PictrPanel extends JPanel implements Observer{

	boolean debug = false;
	boolean debugLayout = false;
    Model model;
   
    Graphics2D gg;
  
    double maxWidth, maxHeight;
    
    public Graphics2D getGraphics2D(){
    	return gg;
    }
 
    private double drawingScale = 0.01;

    public double getDrawingScale() {
		return drawingScale;
	}

	public void setDrawingScale(double drawingScale) {
		this.drawingScale = drawingScale;
	}

	private Java2DRenderer renderer;
	private Rectangle screen;

	/**
     * Constructor creates a renderer used later to create the image
     */
    public PictrPanel(Model m){
        model = m;
        setPreferredSize(new Dimension(800, 800));
        setBackground(new Color(99,99,99));
        setForeground(Color.black);
        setDrawingScale(0.01);
     
        renderer = new Java2DRenderer(this);
        renderer.setScale(1.0d); // default scale 1.0
        screen = this.getBounds();
        int scrW = screen.width;
        int scrH = screen.height;
 
        setVisible(true);
        }	

    /**
     * Respond to a notification from the Model that its
     * contents have changed.
     */
    public void update(Observable o, Object arg) {
    	  System.err.println("update PictrPanel");
        revalidate();
        repaint();
    }
    
	/**
	 * change the scale of the rendered image 
	 */
	public void setScale(double scaleFactor){
		renderer.setScale(scaleFactor);
	}
 
    private int counter = 0; // FOR DEBUGGING
    
    /**
     * Draw the elements of the document, using our Java2DRenderer
     */
    public void paintComponent(Graphics g) {   
    	  super.paintComponent(g);
        	if(! debug){
        		model.accept(renderer);
        		Graphics2D gg = (Graphics2D) g;
                gg.drawImage(renderer.getImage(), 0, 0, null);
        	} else {     	
        		// log some words instead
        		counter++;
        		System.out.println("GuiRenderer.visitDocument " + counter + " *** ");
        	}

    }


}
