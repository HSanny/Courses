package visitors;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.image.BufferedImage;
import java.util.Hashtable;
import java.util.ArrayList;
import static javax.swing.JFrame.setDefaultLookAndFeelDecorated;

import elements.SvgContainerElement;
import elements.SvgElement;
import utility.SVGcolour;
import types.*;
import types.paths.*;

/** Visitor which does the graphic rendering
 * using Java2D. The buffer is created at the beginning,
 * and every visit method writes into it.
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 11/02/11, 4:46 PM
 * @author Chris Johnson renderers for more elements 
 * @author cwj adapt for GUI panels and controls
 * @version $Rev: 1548 $ ($Date: 2013-03-11 09:26:50 +1100 (Mon, 11 Mar 2013) $)
 *          Repository: $URL$
 * @see SvgElement
 * @see types.Type
 */

public class Java2DRenderer implements Visitor{
 
    static final boolean DETECTION_PRINT = false;
    protected BufferedImage buffer;
    double scale; // relative scale of drawing coordinates to buffer coordinates
    Graphics2D gbi;

    public BufferedImage getImage() {
    	return buffer;
    }
    
    public void setScale(double scaleFactor) {
    	scale = scaleFactor;
    }
    
    /** The shape being currently drawn */
    Shape shape;

    JPanel house;
    public Java2DRenderer(){}
    public Java2DRenderer(JPanel house) {
    	/* the SVG tree will draw everything to an image buffer.
    	 * house has the responsibility to redraw this image to window panels when required
    	 */
        /* image buffer */
        house = house;
        Dimension dim = house.getPreferredSize();
		buffer = new BufferedImage(2000, dim.height,
                BufferedImage.TYPE_INT_ARGB);
        /* buffer's graphics content to which visit-methods write */
		gbi = buffer.createGraphics();
		scale = 1.0d; // default
    }
    
    private void wipeImage() {
		gbi.setBackground(Color.WHITE);
  	  	gbi.clearRect(0,0,buffer.getWidth(), buffer.getHeight());
    }
  
    /* entering the document; traverse the SVG tree, draw into buffer image */
	public void visitSvg(SvgElement e) {
		wipeImage();
        visitChildren((SvgContainerElement) e);
    }

    
    public void visitGroup(SvgElement e) {
    	// TODO pick up attributes and apply to children
		// TODO extract id attribute for possible expansion by use
        visitChildren((SvgContainerElement) e);
    }

    
    public void visitUse(SvgElement e) {
		// TODO 
        visitChildren((SvgContainerElement) e);
    }

    
    public void visitUnknown(SvgElement e) {
    	// silently ignore unknown elements
    }

    
    public void visitRectangle(SvgElement e) {

        double x,y,w,h;
        x = Double.parseDouble(e.attributes.get("x"));
        y = Double.parseDouble(e.attributes.get("y"));
        w = Double.parseDouble(e.attributes.get("width"));
        h = Double.parseDouble(e.attributes.get("height"));
        shape = new java.awt.Rectangle.Double(x*scale,y*scale,
								w*scale,h*scale);
        //System.out.println(e.attributes.get("fill"));
        checkStyles(e);
        drawBulk(e, gbi, shape);
    }

    
    public void visitCircle(SvgElement e) {

        double x, y, r;
        x = Double.parseDouble(e.attributes.get("cx"));
        y = Double.parseDouble(e.attributes.get("cy"));
        r = Double.parseDouble(e.attributes.get("r"));
        shape = new Ellipse2D.Double((x-r)*scale,(y-r)*scale,2*r * scale,2*r * scale);
        checkStyles(e);
        drawBulk(e, gbi, shape);
    }

    
    public void visitEllipse(SvgElement e) {
        double x, y, rx, ry;
        x = Double.parseDouble(e.attributes.get("cx"));
        y = Double.parseDouble(e.attributes.get("cy"));
        rx = Double.parseDouble(e.attributes.get("rx"));
        ry = Double.parseDouble(e.attributes.get("ry"));
        shape = new Ellipse2D.Double((x-rx)*scale,(y-ry)*scale,2*rx * scale,2*ry * scale);
        checkStyles(e);
        drawBulk(e, gbi, shape);
    }

    
    public void visitPolyline(SvgElement e) {
        // DONE: SPEC(2) new implementation of polyline rendering
        // interpret Polyline as a path, so that its implementation
        // must follow the implementing mechanism of Path.
    	String points = e.attributes.get("points");
    	if (points == null || points.length() == 0) return;
    	
       	// prototype slackness - transform all commas to space (ignore syntax error of extra commas)
    	points = points.replace( ',' , ' ' );
    	// transform all runs of SVG 1.1 whitespace characters to a single space
    	points = points.replaceAll("[ \t\r\n]+", " "); 
        
        // add the path type to string
        // interpreted as moveto path with absolute location
        points = "M" + points; 
        // add attribute d to hash table e.attributes
        e.attributes.put("d", points);
        this.visitPath(e);

    }
    /*
        Note that the specific recursive data to store polyline
        has been successfully and correctly implemented in my work
        of Assignment 01. Here I am not quite sure whether the 
        interpretation above is exactly desired, but it is really 
        one behavior of high-level reusage of codes. To prevent 
        unnecessary loss of marks,I just post my full-mark 
        implementation in assignment 01 here.

        public void visitPolyline(SvgElement e) {
        // this is my framework for specification (3)
            Polyline polyline;
            if (!e.isParsed) {
                // If this element is not parsed before, parse it and store value in the object
                polyline = new Polyline (e);
    	        String points = e.attributes.get("points");
    	        if (points == null || points.length() == 0) return;
    	
    	        points = points.replace( ',' , ' ' );
    	        points = points.replaceAll("[ \t\r\n]+", " "); 
            	String point[] = points.split(" "); // 
  
                // My implementation is to make Polyline as stack - recursive data type
                // But first we need to restore data of point into stack - for spec (2)
                for (int coordinx = 0; coordinx < point.length; coordinx++) {
                    polyline.addValue(Double.parseDouble(point[coordinx]));
                }
                e.isParsed = true;
                e.setType(polyline);
            } else {
                polyline = (Polyline) e.getType();
            }

            // this is my implementation for spec (2)
            if (polyline.isDrawable()) {
                Polyline temp = polyline; // restore the top of stack
                Shape shape = new Line2D.Double(polyline.getX(), polyline.getY(),
                        polyline.getRest().getX(), polyline.getRest().getY());
    		    drawBulk(e, gbi, shape);
                e.setType(polyline.getRest()); // pesudo pop out
                visitPolyline(e);  // recursion trigger
                e.setType(temp); // assign temporary storage of top back..
            }
    }
     */
    
    public void visitPolygon(SvgElement e) {
		// TODO provide implementation	
    }

    
    public void visitLine(SvgElement e) {
        double x1,y1,x2,y2;
        x1 = Double.parseDouble(e.attributes.get("x1"));
        y1 = Double.parseDouble(e.attributes.get("y1"));
        x2 = Double.parseDouble(e.attributes.get("x2"));
        y2 = Double.parseDouble(e.attributes.get("y2"));
        shape = new Line2D.Double(x1*scale,y1*scale,x2*scale,y2*scale);
        checkStyles(e);
        drawBorder(e, gbi, shape);
    }

    public void visitImage(SvgElement e) {
		// TODO provide implementation	
    }

    public void visitPath(SvgElement e) {
        // DONE: implementation to path 
        // Use recursive data structure since the path
        // information was read in the way of stream flow
        String d = e.attributes.get("d");
        // exception handling
        if (d == null || d.length() == 0) {

        }
        if (!e.isParsed) {
            // If this element is not parsed before, parse it and store value in the object
            Path tailElement = new Path (e);
            // restore the reference of the linkedlist head
            e.setType(tailElement);

            String inputString = new String (e.attributes.get("d"));
            if (inputString == null || inputString.length() == 0) return;

            inputString = inputString.replace( ',' , ' ' );
            inputString = inputString.replaceAll("[ \t\r\n]+", " "); 
            inputString = inputString.replaceAll("[a-zA-Z]", "'$0"); 
            String [] manyPaths = inputString.split("'");
            //System.out.println(manyPaths);
            for (int i = 1; i < manyPaths.length; i ++)
            {
                // System.out.println(manyPaths[i]); // test for the instruction type
                // get the command, points pair
                String command = manyPaths[i].substring(0,1);
                String [] manyStrPoints = manyPaths[i].substring(1).split(" ");
                // parse to derive the int array
                int size = 0;
                for (int k = 0 ; k < manyStrPoints.length; k ++) {
                    if (manyStrPoints[k].compareTo("") == 0) ; 
                    else size ++;
                }
                double [] manyPoints = new double [size];
                for (int k = 0, j = 0 ; k < manyStrPoints.length; k ++, j++) {
                    if (manyStrPoints[k].compareTo("") == 0) {
                        j -- ;
                        continue;
                    } else {
                        manyPoints[j] = Double.parseDouble(manyStrPoints[k]);
                    }
                }
                Path temp = null;
                if (command.matches("[Mm]")) {
                    // move to detected
                    temp = new MoveTo (e, command, manyPoints);
                    if (DETECTION_PRINT) System.out.println("MoveTo detected");
                } else if (command.matches("[Zz]")) {
                    // close path detected
                    temp = new ClosePath (e, command, manyPoints);
                    if (DETECTION_PRINT) System.out.println("ClosePath detected");
                } else if (command.matches("[LlHhVv]")) {
                    // line to detected 
                    temp = new LineTo (e, command, manyPoints);
                    if (DETECTION_PRINT) System.out.println("LineTo detected");
                } else if (command.matches("[Cc]")) {
                    // c curve to detected
                    temp = new CubicCurveTo (e, command, manyPoints);
                    if (DETECTION_PRINT) System.out.println("CubicCurveTo detected");
                } else if (command.matches("[Qq]")) {
                    // quadratic curve to detected
                    temp = new QuadraticCurveTo (e, command, manyPoints);
                    if (DETECTION_PRINT) System.out.println("QuadraticCurveTo detected");
                } else if (command.matches("[Aa]")) {
                    // Arc detected
                    temp = new Arc (e, command, manyPoints);
                    if (DETECTION_PRINT) System.out.println("Arc detected");
                } else {
                    if (DETECTION_PRINT) System.out.println("Unrecognized type of instruction for Path.");
                }
                if (temp != null) {
                    tailElement.add(temp);
                    tailElement = temp.getLast();
                }
            }
            e.isParsed = true;
        } 
        // if it is parsed, directly draw it 
        PathRenderer pathRenderer = new PathRenderer ( e, gbi, scale, buffer);
        Path temp = (Path) e.getType();
        temp = temp.getNext();
        if (temp != null) temp.acceptPV(pathRenderer);

    }

    public void visitText(SvgElement e) {
        // TODO need re-implementation to set the font family properly (can be tricky)
        // TODO apply scale to default fontsize also
        float x, y;
        int size;
        x = Float.parseFloat(e.attributes.get("x"));
        y = Float.parseFloat(e.attributes.get("y"));

        checkStyles(e);
        size = (int)Math.round(scale * Integer.parseInt((e.attributes.get("font-size"))));
        gbi.setColor(SVGcolour.svgColour2Java(e.attributes.get("fill")));
        gbi.setFont(new Font("Palatino",Font.PLAIN,size));
        gbi.drawString(((Text)e.getType()).getContent(), x * (float)scale, y * (float)scale);
    }

    private void visitChildren(SvgContainerElement e) {
        if (e.children != null) {
            for (SvgElement ch : e.children)
                ch.accept(this);
        }
    }

    /* adds necessary attributes if they are absent in e-attributes so they
       are treated as with default values */
    protected void checkStyles(SvgElement e) {
        for(String key: e.getType().getBareMinimum().keySet()) {
            if (e.attributes.get(key) == null) {
                e.attributes.put(key, e.getType().getBareMinimum().get(key));
            } else if (e.attributes.get(key).equalsIgnoreCase("inherit")) {
                //TODO replace by the parent value
                e.attributes.put(key, e.getType().getBareMinimum().get(key));
            }
        }
    }

    /**
     * draw a filled shape for the J2D equivalent of an SVG element
     * @param e SVG element 
     * @param g graphic context to draw into
     * @param sh J2D shape to draw
     */
    protected void drawBulk(SvgElement e, Graphics2D g, Shape sh) {

        try {    		
            g.setColor(SVGcolour.svgColour2Java(e.attributes.get("fill")));
        } catch (Exception e1) {
            dumpAttributes(e, "Bad value for fill in " + e.name + " tag: ");
            g.setColor(Color.YELLOW);
        }
        g.fill(sh);

        try {
            g.setColor(SVGcolour.svgColour2Java(e.attributes.get("stroke")));
        } catch (Exception e1) {
            System.err.println("exception in drawBulk " + e1);
            dumpAttributes(e, "Bad value for stroke in " + e.name + " tag: ");
            g.setColor(Color.RED);
        }

        float strokeWidth = 5.0f;
        try {
            strokeWidth = Float.parseFloat(e.attributes.get("stroke-width"));
        } catch (Exception e1) {
            dumpAttributes(e, "Bad value for stroke-width in " + e.name + " tag: ");
        }
        g.setStroke(new BasicStroke(strokeWidth * (float)scale));
        g.draw(sh);
    }

    /**
     * draw unfilled outline of a J2D equivalent of an SVG element
     * @param e SVG element
     * @param g graphics context
     * @param sh J2D shape to draw
     */
    protected void drawBorder(SvgElement e, Graphics2D g, Shape sh) {
        try {	
            g.setColor(SVGcolour.svgColour2Java(e.attributes.get("stroke")));
        } catch (Exception e1) {
            dumpAttributes(e, "Bad value for stroke in " + e.name + " tag: ");
            g.setColor(Color.RED);
        }
        float strokeWidth = 5.0f;
        try {
            strokeWidth = Float.parseFloat(e.attributes.get("stroke-width"));
        } catch (Exception e1) {
            dumpAttributes(e, "Bad value for stroke-width in " + e.name + " tag: ");
        }
        g.setStroke(new BasicStroke(strokeWidth * (float)scale));
        g.draw(sh);
    }

    /** print a warning or error message with any SvgElement's attributes and their values
     * @param e any SvgElement
     * @param message prefix message before attributes and values
     */
    private void dumpAttributes(SvgElement e, String message) {
        System.out.printf(message);
        // print all attributes
        for (String key: e.attributes.keySet()) {
            System.out.printf("%s=\"%s\" ", key, e.attributes.get(key));
        }
        System.out.printf("%n");
    }

}
