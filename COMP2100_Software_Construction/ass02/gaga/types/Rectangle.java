package types;

import elements.SvgElement;
import visitors.Visitor;

import java.util.TreeMap;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 22/02/11, 4:12 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Rectangle extends GraphicsType {

    public Rectangle(SvgElement el) {
        super(el);
        //this.bareMinimum = new TreeMap<String,String>();
        this.name = "rect";
        this.bareMinimum.put("x", "0");
        this.bareMinimum.put("y", "0");
        this.bareMinimum.put("width", "0");
        this.bareMinimum.put("height", "0");
    }

    //public final String name = "rect";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitRectangle(this.owner);
    }
}
