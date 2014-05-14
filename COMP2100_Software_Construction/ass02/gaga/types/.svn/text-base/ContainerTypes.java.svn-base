package types;

/** Enum with information about concrete SVG container type
 * elements. Included for the extendability and ease of
 * scanning. When a new container  subclass is added,
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
public enum ContainerTypes {

    Svg("svg"),
    Group("g"),
    Use("use"),
    Text("text"),
    Unknown("unknown");

    ContainerTypes(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    /**
     * Check if a tag name is in the set defined as ContainerTypes (case insensitive)
     *
     * @param typeName the name of the tag
     * @return true if typeName names one of the ContainerTypes
     */
    public static boolean hasInstance(String typeName) {
        for (ContainerTypes ct : ContainerTypes.values()) {
            if (ct.getName().equalsIgnoreCase(typeName))
                return true;
        }
        //  nothing found
        return false;
    }

    /**
     * Returns the ContainerType matching a tag name (case insensitive) or Unknown if no match
     * @param typeName tag name to match with a ContainerType name 
     * @return the ContainerType instance with the matched name or Unknown if no match
     */
    public static ContainerTypes getValue(String typeName) {
        for (ContainerTypes ct: ContainerTypes.values()) {
            if (ct.getName().equalsIgnoreCase(typeName))
                return ct;
        }
        return Unknown;
    }

    private final String name;

}
