package visitors;

import elements.SvgContainerElement;
import elements.SvgElement;
import types.*;

import java.util.Arrays;

/** A visitor for an SVG tree that traverses all elements in the tree
 * Check that type of node is the expected one for the named visit method (defensive checking)
 * Verify (and add) minimum default attributes for each node
 * Print the properties of each element
 * 
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 23/02/11, 12:35 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 * @see scanner.SvgScanner
 * @see SvgElement
 * 
 * TODO redirect output to an arbitrary OutputStream 
 * TODO add a flag which will control output 
 * TODO - add a switch to suppress isPrinted 
 * TODO - recognise and link <use> of <def>s
 * 
 */

public class Validator implements Visitor {
    /** This is the solution for the additional specification.
     *    1. add an internal switch to enable or disable reporting of 
     *      the SVG tree structure to the console
     *    2. Submit your program with reporting switched OFF: 
     *      your program should have no output to console
     * */
    static final boolean isPrinted = false;
    
    public void visitSvg(SvgElement e) {
        assert (e.getType() instanceof Svg):
                String.format("Node type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportContainerElement(e);
        }
        visitChildren((SvgContainerElement) e);

    }

    
    public void visitGroup(SvgElement e) {
        assert (e.getType() instanceof Group):
                String.format("Node type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportContainerElement(e);
        }
        visitChildren((SvgContainerElement) e);
    }

    
    public void visitUse(SvgElement e) {
        assert (e.getType() instanceof Use):
                String.format("Node type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportContainerElement(e);
        }
        visitChildren((SvgContainerElement) e);
    }

    
    public void visitUnknown(SvgElement e) {
         assert (e.getType() instanceof Unknown):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportContainerElement(e);
        }
        visitChildren((SvgContainerElement) e);
    }

    
    public void visitRectangle(SvgElement e) {
        assert (e.getType() instanceof Rectangle):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitCircle(SvgElement e) {
        assert (e.getType() instanceof Circle):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());
        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitEllipse(SvgElement e) {
        assert (e.getType() instanceof Ellipse):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }


    
    public void visitPolyline(SvgElement e) {
        assert (e.getType() instanceof Polyline):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitPolygon(SvgElement e) {
        assert (e.getType() instanceof Polygon):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitLine(SvgElement e) {
        assert (e.getType() instanceof Line):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitImage(SvgElement e) {
        assert (e.getType() instanceof Image):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitPath(SvgElement e) {
        assert (e.getType() instanceof Path):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        verify(e);
        if (this.isPrinted) {
            reportGraphicsElement(e);
        }
    }

    
    public void visitText(SvgElement e) {
        assert (e.getType() instanceof Text):
                String.format("Node of type(%s)/name(%s) mismatch",
                        e.name, e.getType().getName());

        String text = ((Text) e.getType()).getContent();
        if (this.isPrinted) {
            reportTextElement(e,text);
        }
    }

    private void reportContainerElement(SvgElement e) {
        if (this.isPrinted) {
            System.out.println("Container node: " + e.name +
                    ", type: " + e.getType().getClass().getName());
        }
            reportAttributes(e,4);
    }

    private void reportGraphicsElement(SvgElement e) {
        if (this.isPrinted) {
            System.out.println("Graphics node: " + e.name +
                    ", type: " + e.getType().getClass().getName());
        }
            reportAttributes(e,4);
    }

    private void reportTextElement(SvgElement e, String text) {
        if (this.isPrinted) {
            System.out.println("Text node: " + e.name +
                    ", type: " + e.getType().getClass().getName());
        }
        reportAttributes(e,4);
        if (this.isPrinted) {
            System.out.printf("%sThe text data: %s%n", pad(4), text);
        }
    }

    private void reportAttributes(SvgElement e, int indent) {
        for (String key: e.attributes.keySet()) {
            String value =  e.attributes.get(key);
            if (this.isPrinted) {
                System.out.printf("%s%s=\"%s\"%n", pad(indent), key, value);
            }
        }
    }

    /* to check if e has most essential attributes set,
     * and add/set them if not pulling them from the attached type */
    private void verify(SvgElement e) {
        Type type = e.getType();
        for(String key: type.getBareMinimum().keySet()) {
            if (e.attributes.get(key) == null)
                e.attributes.put(key, type.getBareMinimum().get(key));
        }
    }

    private String pad(int p) {
        char[] ca = new char[p];
        Arrays.fill(ca,' ');
        return new String(ca);
    }

    private void visitChildren(SvgContainerElement e) {
        if (e.children != null) {
            for (SvgElement ch: e.children)
                ch.accept(this);
        }
    }
}
