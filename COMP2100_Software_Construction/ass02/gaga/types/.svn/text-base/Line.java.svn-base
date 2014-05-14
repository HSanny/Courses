package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 25/02/11, 12:33 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Line extends GraphicsType {

    public Line(SvgElement el) {
        super(el);
        this.name = "line";
        this.bareMinimum.put("x1", "0");
        this.bareMinimum.put("y1", "0");
        this.bareMinimum.put("x2", "0");
        this.bareMinimum.put("y2", "0");
    }

    //public final String name = "line";

    @Override
    public void accept(Visitor visitor) {

        visitor.visitLine(this.owner);
    }
}
