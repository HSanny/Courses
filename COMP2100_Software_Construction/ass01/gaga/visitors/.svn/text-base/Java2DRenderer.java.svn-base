package visitors;

import elements.SvgContainerElement;
import elements.SvgElement;
import utility.SVGcolour;
import types.Text;
import types.*;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.image.BufferedImage;
import java.util.Hashtable;

import java.lang.management.*;

import static javax.swing.JFrame.setDefaultLookAndFeelDecorated;

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

/** Note that there are two switch, one is specified for reporting element tree, the other is 
 *      for reporting the result of my solution for specification (3)
 *  The first is default to be OFF, the second is default to be true for convenient marking
 *
 * This is my solution for the specification (3) 
 * Having read the original framework of Gaga, I prefer not to create a new visitor but directly
 *      modify the java2DRender file (If create a new visitor, then I have to modify related codes 
 *      in GUI part of gaga in order to take effect the new visitor)
 * First, I have changed the implementation of most of methods, including:
 *      - visitSvg (invoke the original implementation and my new implementation 
 *          while each time of resizing for comparison of time consumption)
 *      - visitRectangle
 *      - visitFrame (This is also for specification (1) )
 *      - visitEllipse 
 *      - visitCircle
 *      - visitText
 * Note that I did not implement the solution to specification (3) for visitPolyline
 *
 * Besides, I add corresponding fields to almost all the types, these types including:
 *      - Rectangle 
 *      - Frame
 *      - Polyline
 *      - Ellipse
 *      - Circle
 *
 * Most importantly, I add a new field called isParsed to SvgElement file to record whether
 *  this element has been parsed before:
 *      - if not parsed before, we should access the uniform hashmap 
 *          called attributes and restore parsed data in the newly 
 *          added fields with specific (not uniform) data type
 *      - if this element is parsed, we can directly access the 
 *          newly created field of each type for attributes information
 * */

public class Java2DRenderer  implements Visitor {
    // This is the switch for choosing whether new implementation is used (see visitSvg for usage) ..
    //   This switch will be used in almost all visit method.
    // if true, program will only execute my improved implementation
    // if false, program will only execute the original implementation
    static boolean TIME_IMRPOVEMENT = true;
    // Switch for showing the result of comparison between 
    static final boolean TIME_COMPARISON_PRINTED = true;
 
    private BufferedImage buffer;
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

    private static long getCpuTime( ) {
	     ThreadMXBean bean = ManagementFactory.getThreadMXBean( );
	     if ( bean.isCurrentThreadCpuTimeSupported() )
	         return bean.getCurrentThreadCpuTime( );
         else 
             return 0L;
	 }

    public Java2DRenderer(JPanel house) {
    	/* the SVG tree will draw everything to an image buffer.
    	 * house has the responsibility to redraw this image to window panels when required
    	 */
        /* image buffer */
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
        // Here, we run two implementation of visiting svg element. 
        // We run both of Implementation for comparison.
        long originalCost, myCost;
        long beforeVisit, afterVisit;
        // first is original one. Parse without additional data storage
        Java2DRenderer.TIME_IMRPOVEMENT = false;
        beforeVisit = getCpuTime();
        visitChildren((SvgContainerElement) e);
        afterVisit = getCpuTime();
        originalCost = (afterVisit - beforeVisit)/1000;
        // second one is for new implementation. Parse with data storage. (quicker)
        wipeImage();
        Java2DRenderer.TIME_IMRPOVEMENT = true;
        beforeVisit = getCpuTime();
        visitChildren((SvgContainerElement) e);
        afterVisit = getCpuTime();
        myCost = (afterVisit - beforeVisit)/1000;
        // print the result of comparison
        if (TIME_COMPARISON_PRINTED) {
            System.out.println(String.format("Time Consumption of Origin: %d microseconds", originalCost));
            System.out.println(String.format("My implementation: %d microseconds", myCost));
            System.out.println(String.format("Time Saving: %d  microseconds", (originalCost - myCost)));
            System.out.println("");
        }
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
        if (Java2DRenderer.TIME_IMRPOVEMENT) {
            // if improved algorithm is required, ...
            if (!e.isParsed) {
                // If this element is not parsed before, parse it and store value in the object
                types.Rectangle rect = new types.Rectangle (e);
                x = Double.parseDouble(e.attributes.get("x"));
                y = Double.parseDouble(e.attributes.get("y"));
                w = Double.parseDouble(e.attributes.get("width"));
                h = Double.parseDouble(e.attributes.get("height"));
                rect.setX(x);
                rect.setY(y);
                rect.setWidth(w);
                rect.setHeight(h);
                e.isParsed = true;
                e.setType(rect);
            } else {
                // if this element is parsed before, directly read the data from field of object
                types.Rectangle rect = (types.Rectangle) e.getType();
                x = rect.getX();
                y = rect.getY();
                w = rect.getWidth();
                h = rect.getHeight();
            } 
        } else {
            // if we use original implementation
            x = Double.parseDouble(e.attributes.get("x"));
            y = Double.parseDouble(e.attributes.get("y"));
            w = Double.parseDouble(e.attributes.get("width"));
            h = Double.parseDouble(e.attributes.get("height"));
        }
        shape = new java.awt.Rectangle.Double(x*scale,y*scale,
								w*scale,h*scale);
        //System.out.println(e.attributes.get("fill"));
        checkStyles(e);
        drawBulk(e, gbi, shape);
    }

    
    public void visitCircle(SvgElement e) {
        double x, y, r;
        if (Java2DRenderer.TIME_IMRPOVEMENT) {
             // if improved algorithm is required, ...
            if (!e.isParsed) {
                 // If this element is not parsed before, parse it and store value in the object
                Circle circle = new Circle (e);
                x = Double.parseDouble(e.attributes.get("cx"));
                y = Double.parseDouble(e.attributes.get("cy"));
                r = Double.parseDouble(e.attributes.get("r"));
                circle.setCX(x);
                circle.setCY(y);
                circle.setRadius(r);
                e.isParsed = true;
                e.setType(circle);
            } else {
                // if this element is parsed before, directly read the data from field of object
                Circle circle = (Circle) e.getType();
                x = circle.getCX();
                y = circle.getCY();
                r = circle.getRadius();
            } 
        } else {
            // if we use original implementation
            x = Double.parseDouble(e.attributes.get("cx"));
            y = Double.parseDouble(e.attributes.get("cy"));
            r = Double.parseDouble(e.attributes.get("r"));
        }

        shape = new Ellipse2D.Double((x-r)*scale,(y-r)*scale,2*r * scale,2*r * scale);
        checkStyles(e);
        drawBulk(e, gbi, shape);
    }

    
    public void visitEllipse(SvgElement e) {
        double x, y, rx, ry;
        if (Java2DRenderer.TIME_IMRPOVEMENT) {
             // if improved algorithm is required, ...
            if (!e.isParsed) {
                 // If this element is not parsed before, parse it and store value in the object
                Ellipse elp = new Ellipse (e);
                x = Double.parseDouble(e.attributes.get("cx"));
                y = Double.parseDouble(e.attributes.get("cy"));
                rx = Double.parseDouble(e.attributes.get("rx"));
                ry = Double.parseDouble(e.attributes.get("ry"));
                elp.setCX(x);
                elp.setCY(y);
                elp.setRX(rx);
                elp.setRY(ry);
                e.isParsed = true;
                e.setType(elp);
            } else {
                // if this element is parsed before, directly read the data from field of object
                Ellipse elp = (Ellipse) e.getType();
                x = elp.getCX();
                y = elp.getCY();
                rx = elp.getRX();
                ry = elp.getRY();
            } 
        } else {
            // if we use original implementation
            x = Double.parseDouble(e.attributes.get("cx"));
            y = Double.parseDouble(e.attributes.get("cy"));
            rx = Double.parseDouble(e.attributes.get("rx"));
            ry = Double.parseDouble(e.attributes.get("ry"));
        }
        shape = new Ellipse2D.Double((x-rx)*scale,(y-ry)*scale,2*rx * scale,2*ry * scale);
        checkStyles(e);
        drawBulk(e, gbi, shape);
    }

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

    public void visitFrame(SvgElement e){
        double x1, x2, y1, y2;
        double thin, thout;
        String colin, colout;

        if (Java2DRenderer.TIME_IMRPOVEMENT) {
             // if improved algorithm is required, ...
            if (!e.isParsed) {
                 // If this element is not parsed before, parse it and store value in the object
                types.Frame frame = new types.Frame (e);
                x1 = Double.parseDouble(e.attributes.get("x1"));
                y1 = Double.parseDouble(e.attributes.get("y1"));
                x2 = Double.parseDouble(e.attributes.get("x2"));
                y2 = Double.parseDouble(e.attributes.get("y2"));
                thin = Double.parseDouble(e.attributes.get("thin"));
                thout = Double.parseDouble(e.attributes.get("thout"));
                colin = e.attributes.get("colin");
                colout = e.attributes.get("colout");
                frame.setX1(x1);
                frame.setY1(y1);
                frame.setX2(x2);
                frame.setY2(y2);
                frame.setThin(thin);
                frame.setThout(thout);
                frame.setColin(colin);
                frame.setColout(colout);
                e.isParsed = true;
                e.setType(frame);
            } else {
                // if this element is parsed before, directly read the data from field of object
                types.Frame frame = (types.Frame) e.getType();
                x1 = frame.getX1();
                y1 = frame.getY1();
                x2 = frame.getX2();
                y2 = frame.getY2();
                thin = frame.getThin();
                thout = frame.getThout();
                colin = frame.getColin();
                colout = frame.getColout();
            } 
        } else {
            // use Original Implementation
            // read attributes
            x1 = Double.parseDouble(e.attributes.get("x1"));
            y1 = Double.parseDouble(e.attributes.get("y1"));
            x2 = Double.parseDouble(e.attributes.get("x2"));
            y2 = Double.parseDouble(e.attributes.get("y2"));
            thin = Double.parseDouble(e.attributes.get("thin"));
            thout = Double.parseDouble(e.attributes.get("thout"));
            colin = e.attributes.get("colin");
            colout = e.attributes.get("colout");
        }
        
        // recognize the corner coordinate
        double minX, maxX, minY, maxY;
        minX = (x1 < x2) ? x1 : x2;
        maxX = (x1 < x2) ? x2 : x1;
        minY = (y1 < y2) ? y1 : y2;
        maxY = (y1 < y2) ? y2 : y1;

        // set the width of frame
        double frameWidth = thin + thout;

        // draw outer rectangle
        checkStyles(e);
        Shape innerRectangle, outerRectangle;

        double xOuterRectangle = minX*scale; 
        double yOuterRectangle = minY*scale; 
        double widthOuterRectangle = (maxX-minX)*scale; 
        double heightOuterRectangle = (maxY-minY)*scale; 
        outerRectangle = new java.awt.Rectangle.Double(xOuterRectangle, yOuterRectangle, widthOuterRectangle, heightOuterRectangle);
        gbi.setColor(SVGcolour.svgColour2Java(colout));
        gbi.fill(outerRectangle);
        gbi.draw(outerRectangle);

        double xInnerRectangle = (minX+frameWidth)*scale; 
        double yInnerRectangle = (minY+frameWidth)*scale; 
        double widthInnerRectangle = (maxX-minX-2*frameWidth)*scale; 
        double heightInnerRectangle = (maxY-minY-2*frameWidth)*scale;
        innerRectangle = new java.awt.Rectangle.Double(xInnerRectangle, yInnerRectangle, widthInnerRectangle, heightInnerRectangle);
        gbi.setColor(SVGcolour.svgColour2Java(colin));
        gbi.fill(innerRectangle);
        gbi.draw(innerRectangle);
    }

    
    public void visitPolygon(SvgElement e) {
		// TODO provide implementation	
    }

    
    public void visitLine(SvgElement e) {
	    double x1,y1,x2,y2;
        if (Java2DRenderer.TIME_IMRPOVEMENT) {
             // if improved algorithm is required, ...
            if (!e.isParsed) {
                 // If this element is not parsed before, parse it and store value in the object
                Line line = new Line (e);
                x1 = Double.parseDouble(e.attributes.get("x1"));
                y1 = Double.parseDouble(e.attributes.get("y1"));
                x2 = Double.parseDouble(e.attributes.get("x2"));
                y2 = Double.parseDouble(e.attributes.get("y2"));
                line.setX1(x1);
                line.setY1(y1);
                line.setX2(x2);
                line.setY2(y2);
                e.isParsed = true;
                e.setType(line);
            } else {
                // if this element is parsed before, directly read the data from field of object
                Line line = (Line) e.getType();
                x1 = line.getX1();
                y1 = line.getY1();
                x2 = line.getX2();
                y2 = line.getY2();
            } 
        } else {
            // use Original Implementation
            x1 = Double.parseDouble(e.attributes.get("x1"));
            y1 = Double.parseDouble(e.attributes.get("y1"));
            x2 = Double.parseDouble(e.attributes.get("x2"));
            y2 = Double.parseDouble(e.attributes.get("y2"));
        }
        shape = new Line2D.Double(x1*scale,y1*scale,x2*scale,y2*scale);
        checkStyles(e);
        drawBorder(e, gbi, shape);
    }

    public void visitImage(SvgElement e) {
		// TODO provide implementation	
    }

    public void visitPath(SvgElement e) {
		// TODO provide implementation	
    }

    
    public void visitText(SvgElement e) {
		// TODO need re-implementation to set the font family properly (can be tricky)
    	// TODO apply scale to default fontsize also
        float x, y;
        int size;
        String fill;
        String content;
        if (Java2DRenderer.TIME_IMRPOVEMENT) {
             // if improved algorithm is required, ...
            if (!e.isParsed) {
                 // If this element is not parsed before, parse it and store value in the object
                types.Text text = new types.Text (e);
                x = Float.parseFloat(e.attributes.get("x"));
                y = Float.parseFloat(e.attributes.get("y"));
                size = (int)Math.round(scale * Integer.parseInt(e.attributes.get("font-size")));
                fill = e.attributes.get("fill");
                content = ((Text) e.getType()).getContent();

                text.setX(x);
                text.setY(y);
                text.setFontSize(size);
                text.setFill(fill);
                text.setContent(content);

                e.isParsed = true;
                e.setType(text);
            } else {
                // if this element is parsed before, directly read the data from field of node object
                types.Text text = (types.Text) e.getType();
                x = text.getX();
                y = text.getY();
                size = text.getFontSize();
                fill = text.getFill();
                content = text.getContent();
            } 
        } else {
            // use Original Implementation
            x = Float.parseFloat(e.attributes.get("x"));
            y = Float.parseFloat(e.attributes.get("y"));
            size = (int)Math.round(scale * Integer.parseInt(e.attributes.get("font-size")));
            fill = e.attributes.get("fill");
            content = ((Text) e.getType()).getContent();
        }

        checkStyles(e);
        gbi.setColor(SVGcolour.svgColour2Java(fill));
        gbi.setFont(new Font("Palatino", Font.PLAIN, size));
        gbi.drawString(content, x * (float)scale, y * (float)scale);
    }

    private void visitChildren(SvgContainerElement e) {
        if (e.children != null) {
            for (SvgElement ch : e.children)
                ch.accept(this);
        }
    }

    /* adds necessary attributes if they are absent in e-attributes so they
	are treated as with default values */
	private void checkStyles(SvgElement e) {
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
	private void drawBulk(SvgElement e, Graphics2D g, Shape sh) {

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
	private void drawBorder(SvgElement e, Graphics2D g, Shape sh) {
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
        
