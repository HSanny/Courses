package types;

import visitors.Visitor;

import java.util.TreeMap;

/** Top of the element type hierarchy
 * Has two abstract subclasses -- the separation
 * is for expandability purpose mainly: the way scanner
 * selects the right element depends on whether it is a group
 * type, or a primitive type. Extension of either sub-hierarchy
 * will not affect the scanning process. Information about
 * concrete classes in the two sub-hierarchies is stored
 * in two enum classes: <em>ContainerTypes</em> and
 * <em>GraphicsTypes</em>. Use of enums makes scanner
 * work easier and more reliable
 * 
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 25/02/11, 12:52 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 * @see ContainerType
 * @see ContainerTypes
 * @see GraphicsType
 * @see GraphicsTypes
 */

abstract public class Type {

    protected String name;

    /* essential attributes */
    protected TreeMap<String,String> bareMinimum;

    public Type() {
        this.bareMinimum = new TreeMap<String,String>();

        /* Set up the style attributes to default values */
        this.bareMinimum.put("fill", "#FFFFFF");	// white fill
        this.bareMinimum.put("stroke", "#000");		// black strokes
        this.bareMinimum.put("stroke-width", "1.0f");	// 1 pixel width
    }

    public TreeMap<String,String> getBareMinimum() {
        return bareMinimum;
    }

    public abstract void accept(Visitor visitor);

    /*public void setName(String name) {
        this.name = name;
    }*/

    public String getName() {
        return this.name;
    }

}
