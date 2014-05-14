package elements;

import org.xml.sax.Attributes;
import types.GraphicsType;
import types.Type;
import visitors.Visitor;

import java.io.IOException;
import java.io.PrintStream;

/** Class to represent a graphics node (without children).
 * Its type (rect, circle etc) is determined by the type field
 * (composition with Strategy pattern to achieve type specialisation)
 * @author abx
 *         Date: 8/02/11
 *         Time: 1:08 PM
 * @version $REV$
 *          Repository: $URL$
 */

public class SvgGraphicsElement extends SvgElement {

    protected GraphicsType type;

    public SvgGraphicsElement() {
        super();
    }

    public SvgGraphicsElement(String elname, Attributes atts) {
        super(elname,atts);

    }

    @Override
    public void setType(Type type) {
        this.type = (GraphicsType) type;
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
        sb.append("/>" +EOL);
        //sb.append("Type of the element is " + this.type.getName());
        output.println(sb);
    }

    /** Define how visitor "does" this element
     *
     * @param visitor which performs required action
     */
    @Override
    public void accept(Visitor visitor) {
        this.type.accept(visitor);
    }
}

