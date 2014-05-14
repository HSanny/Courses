package types;

import elements.SvgElement;
import visitors.Visitor;

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
    }

    //public final String name = "path";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitPath(this.owner);
    }
}
