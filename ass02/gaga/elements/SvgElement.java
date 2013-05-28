package elements;

import org.xml.sax.Attributes;
import types.Type;
import visitors.Visitor;

import java.io.IOException;
import java.io.PrintStream;
import java.util.Arrays;
import java.util.TreeMap;

/** Top class in the element hierarchy.
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 12/02/11, 12:54 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


abstract public class SvgElement {

    public String name;
    public SvgElement parent;

    private Type type;

   /* using common DS to make attributes ordered and only have
    * simple names without prefixes and qualifiers;
    * perhaps, one should declare them as org.xml.sax.Attributes */
    public TreeMap<String, String> attributes;

    public boolean isParsed = false;
    protected SvgElement() {
        this.attributes = new TreeMap<String,String>();
        parent = null;
    }

    protected SvgElement(String elname, Attributes atts) {
        //this.attributes = new TreeMap<String, String>();
        this();

        this.name = elname;
        for (int i = 0; i < atts.getLength(); i++)
            this.attributes.put(atts.getLocalName(i), atts.getValue(i));
        //    throw new IllegalNameException("Cannot create " + this.name +
    }

    public void setType(Type type) {
        this.type = type;
    }
    public Type getType() {
        return this.type;
    }

    /** Set parent equal to p */
    public void setParent(SvgElement p)
            throws ClassCastException, NullPointerException {
        if (p == null)
            throw new NullPointerException("Parent does not exist");
        else if (p instanceof SvgContainerElement) {
            this.parent = p;
            ((SvgContainerElement) p).children.add(this);
        } else {
            throw new ClassCastException("Parent is not a container");
        }
    }

    abstract public void prettyPrint(int indent, PrintStream output)
            throws IOException;

    abstract public void accept(Visitor visitor);

    protected final String EOL = System.getProperty("line.separator");
    protected final int LEVEL = 4; // the size of level indentation

    /* Even with latest API, the Java way to create
       a padded string of a required length is awkward.
       Let's say this again: EVERYONE must move over to Python!  :-)
    */
    protected String pad(final int p) {
        /*char[] ca = new char[p];
        Arrays.fill(ca, ' ');
        return new String(ca);*/
        // Alternatively:
        StringBuffer padbuffer = new StringBuffer();
        for (int i = 0; i < p; i++)
            padbuffer.append(' ');
        return new String(padbuffer);
    }
}
