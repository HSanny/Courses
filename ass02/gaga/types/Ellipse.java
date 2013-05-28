package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 22/02/11, 11:56 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Ellipse extends GraphicsType {

    public Ellipse(SvgElement el) {
        super(el);
        this.name = "ellipse";
        this.bareMinimum.put("cx", "0");
        this.bareMinimum.put("cy", "0");
        this.bareMinimum.put("rx", "0");
        this.bareMinimum.put("ry", "0");
    }

   //public final String name = "ellipse";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitEllipse(this.owner);
    }
}
