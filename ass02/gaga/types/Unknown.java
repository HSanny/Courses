package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 27/02/11, 4:31 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Unknown extends ContainerType {

    public Unknown(SvgElement el) {
        super(el);
        this.name = "unknown";
    }

    //public final String name = "unknown";

    @Override
    public void accept(Visitor visitor) {

        visitor.visitUnknown(this.owner);
    }
}
