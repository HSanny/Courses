package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 22/02/11, 11:58 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Polygon extends GraphicsType {

    public Polygon(SvgElement el) {
        super(el);
        this.name = "polygon";
    }

    //public final String name = "polygon";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitPolygon(this.owner);
    }
}
