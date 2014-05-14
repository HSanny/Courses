package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 25/02/11, 12:17 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Group extends ContainerType {

    public Group(SvgElement el) {
        super(el);
        this.name = "g";
    }

    //public final String name = "g";

    @Override
    public void accept(Visitor visitor) {

        visitor.visitGroup(this.owner);
    }
}
