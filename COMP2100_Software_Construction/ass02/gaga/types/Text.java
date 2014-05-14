package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 25/02/11, 12:26 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Text extends ContainerType {

    public Text(SvgElement el) {
        super(el);
        this.name = "text";
        this.bareMinimum.put("x", "0");
        this.bareMinimum.put("y", "0");
        this.bareMinimum.put("font-size", "25");
        this.bareMinimum.put("fill", "#000");
    }

    //public final String name = "text";

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    private String content;

    @Override
    public void accept(Visitor visitor) {

        visitor.visitText(this.owner);
    }

}
