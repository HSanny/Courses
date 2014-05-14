package elements;

import org.xml.sax.Attributes;
import types.ContainerType;
import types.Type;
import visitors.Visitor;

import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

/** Class to represent a container element (group etc)
 * which can have children. The actual type is determined
 * by type field (specialisation through composition).
 *
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 14/02/11, 2:16 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class SvgContainerElement extends SvgElement {

    public List<SvgElement> children;
    protected ContainerType type;

    public SvgContainerElement() {
        super();
        children = new ArrayList<SvgElement>();
    }

    public SvgContainerElement(String name, Attributes atts) {
        super(name,atts);
        children = new ArrayList<SvgElement>();
    }

    public void setType(Type type) {
        this.type = (ContainerType) type;
    }

    @Override
    public Type getType() {
        return type;
    }

    @Override
    public void prettyPrint(int indent, PrintStream output) throws IOException {
        assert (indent >= 0): "Indent must be non-negative integer";
        String pad = pad(indent);
        StringBuffer sb = new StringBuffer(pad + "<" + this.name);
        int j = name.length() + 2;
        for(String aname: this.attributes.keySet()) {
            sb.append(EOL + pad(indent+j) + aname + "=" +
                    "\"" + this.attributes.get(aname) + "\"");
        }


        if (this.children.size() == 0) {
            sb.append("/>" + EOL);
        }
        else {
            sb.append(">" + EOL);
            output.println(sb);
            sb.delete(0,sb.length());
            for (SvgElement child: this.children) {
                child.prettyPrint(indent + LEVEL,output);
            }
            sb.append(pad + "</" + this.name + ">" + EOL);
        }
        //sb.append("Type of the element is " + this.type.getName());
        output.println(sb);
    }

    /** Define how visitor "does" this element
     *
     * @param visitor which performs required action
     */
    @Override
    public void accept(Visitor visitor) {
        type.accept(visitor);
    }
}
