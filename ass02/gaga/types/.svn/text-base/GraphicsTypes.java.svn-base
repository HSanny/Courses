package types;

/** Enum with information about concrete SVG graphics type
 * elements. Included for the extendability and ease of
 * scanning. When a new primitive graphics subclass is added,
 * this class needs to be expanded appropriately, but
 * <em>SvgScanner</em> will not be affected
 *
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 15/02/11, 12:31 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 *
 * @see scanner.SvgScanner
 */

public enum GraphicsTypes {

    Circle("circle"),
    Ellipse("ellipse"),
    Line("line"),
    Rectangle("rect"),
    Polyline("polyline"),
    Polygon("polygon"),
    Path("path"),
    Image("image");


    GraphicsTypes(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    /** Checks if a tag name is in the set defined by GraphicsTypes (case-insensitive)
     * as the passed parameter
     * @param n the name of the instance
     * @return true if the enum has an instance with the passed name
     * */
    public static boolean hasInstance(String n) {
        for (GraphicsTypes bt: GraphicsTypes.values()) {
            if (bt.getName().equalsIgnoreCase(n))
                return true;
        }
        //  nothing found
        return false;
    }

    /**
     * Find the GraphicsType matching a tag name (case-insensitive)
     * @param name the string name to be matched
     * @return the enum instance with the matched name or null if no match
     */
    public static GraphicsTypes getValue(String name) {
        for (GraphicsTypes bt: GraphicsTypes.values()) {
            if (bt.getName().equalsIgnoreCase(name))
                return bt;
        }
        // nothing found
        return null;
    }

    private final String name;

}
