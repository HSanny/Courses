package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 25/02/11, 12:24 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Svg extends ContainerType {

    public Svg(SvgElement el) {
        super(el);
        this.name = "svg";
    }

    //public final String name = "svg";

    @Override
    public void accept(Visitor visitor) {

        visitor.visitSvg(this.owner);
    }
}
