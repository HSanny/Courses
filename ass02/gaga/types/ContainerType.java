package types;

import elements.SvgElement;
import visitors.Visitor;

/** Top class of the container element sub-hierarchy
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 13/02/11, 5:22 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 * @see Group
 * @see Use
 * etc
 */

abstract public class ContainerType extends Type {

    public SvgElement owner;

    /*public static GraphicsType SvgElementStrategyFactory(String s) {
        return null;
    }*/

    public ContainerType(SvgElement el) {
        this.owner = el;
    }

}
