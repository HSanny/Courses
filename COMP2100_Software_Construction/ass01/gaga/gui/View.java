package gui;

import static javax.swing.JFrame.setDefaultLookAndFeelDecorated;

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;


/**
 * Starting point for the graphical user interface, responsible
 * for creating and displaying all user interface elements.
 *
 * @author Ian Barnes
 * @author Chris Johnson
 * @version $Revision: 1532 $, $Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $
 */
public class View {

    Model model;
    JFrame frame;
    PictrPanel panel;

    // zoom controller as an example
	 JButton scaleButton = new JButton("zoom in");
	 boolean scaleIsLarge = true;

	 /**
     * Create and show the main window and its contents.
     */
    public View(Model m){
    	model = m;
    	frame = new JFrame();
    	frame.setSize(new Dimension(500, 650));
    	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setDefaultLookAndFeelDecorated(true);
        
    	panel = new PictrPanel(model);

    	// Lay out the text panel and control panel using BoxLayout
    	JPanel panelMain = new JPanel();
    	panelMain.setPreferredSize(new Dimension(600, 800));
    	panelMain.setLayout(new BoxLayout(panelMain,BoxLayout.Y_AXIS)); 

    
    	// the scroll pane contains the drawing canvas
    	JScrollPane scrollPane = new JScrollPane(panel,
    			JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
    			JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    	scrollPane.setMaximumSize(new Dimension(1000, 800));
    	scrollPane.setPreferredSize(new Dimension(600, 800));
    	
    	// the control panel has a label (and allows future controls to be added)
    	JPanel controlPanel = new JPanel();
    	controlPanel.setPreferredSize(new Dimension(250, 40));
    	controlPanel.setMaximumSize(new Dimension(1000, 100));
    	
    	JLabel docName = new JLabel("filename: " + model.getDocName());
    	controlPanel.add(docName);

    	  	// Action for scale button:
     	// toggle between large or default scale rendering of embedded drawing objects
     	class ScaleButtonListener implements ActionListener {
     		public void actionPerformed(ActionEvent event){
     			scaleIsLarge = ! scaleIsLarge;
     			if (scaleIsLarge) {
     				setScale(1.0);
     				scaleButton.setText("Zoom in");
     				panel.repaint();
     			} else {
     				setScale(2.0);
     				scaleButton.setText("Zoom out");
     				panel.repaint();
     			}
     		}
     	}
     	scaleButton.addActionListener(new ScaleButtonListener());
     	controlPanel.add(scaleButton);
    	panelMain.add(controlPanel);
    	panelMain.add(scrollPane);

    	frame.setContentPane(panelMain);
  
    	// set up Model-View-Controller interaction from model to this panel
    	model.addObserver(panel);
    	// NOTE: programmer must add update and notify calls to make this work if the model changes    	
    
    	frame.setTitle("Gaga SVG viewer " + "$Revision: 0$");
    	frame.setVisible(true);
    }
    
	
	/**
	 * change the scale of the rendered image 
	 */
	public void setScale(double scaleFactor){
	  panel.setScale(scaleFactor);	
	}
}
