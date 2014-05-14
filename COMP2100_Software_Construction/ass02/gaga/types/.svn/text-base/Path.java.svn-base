package types;

import elements.SvgElement;
import visitors.Visitor;
import visitors.PathVisitor;
import types.paths.Point2D;
/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 23/02/11, 12:21 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Path extends GraphicsType {
    public Path(SvgElement el) {
        super(el);
        this.name = "path";
        this.command = "";
    }

    protected String label;

    /** command list:
     *   moveto [Mm]
     *   closepath [Zz]
     *   lineto [LlHhVv]
     *   Cubic Bezier curveto [Cc]
     *   Quadratic Bezier curveto [Qq]
     *   arc [Aa]
     * */
    protected String command; 
    public String getCommand() {
        return this.command;
    }

    // current position: the position before 
    // executing this path node.
    protected Point2D cp = new Point2D (-1,-1); // init to be illegal 
    public boolean isCurrentPositionValid() {
        if (this.cp.getAbscissa() > 0 && this.cp.getOrdinate() > 0) 
            return true;
        else 
            return false;
    }
    public void setCurrentPosition(int abs, int ord) {
        this.cp.setAbscissa(abs);
        this.cp.setOrdinate(ord);
    }
    public Point2D getCurrentPosition() {
        return this.cp;
    }
    public void setCurrentPositionForNext(int abs, int ord) {
        if (this.hasNext()) {
            this.getNext().setCurrentPosition(abs, ord);
        }
    }

    // recursive structure (linked list) of path
    protected Path next;
    public boolean hasNext() {
        if (this.next == null) return false;
        else return true;
    }
    public Path getNext() { return this.next; }
    public Path getLast() {
        if (this.hasNext()) 
            return this.next.getLast();
        else 
            return this;
    }
    // modifier of path class 
    public void add(Path p) {
        if (p == null) return ;
        if (this.hasNext()) {
            this.next.add(p);
        } else {
            this.next = p;
        }
    }


    //public final String name = "path";

    public void acceptPV(PathVisitor visitor) {};
    @Override
    public void accept(Visitor visitor) {
        visitor.visitPath(this.owner);
    }
}
