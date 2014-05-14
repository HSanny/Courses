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

    private float x;
    private float y;
    private int fontSize;
    private String fill; 

    public float getX() { return this.x; }
    public void setX(float value) { this.x = value; }
    public float getY() { return this.y; }
    public void setY(float value) { this.y = value; }
    public int getFontSize() { return this.fontSize; }
    public void setFontSize(int value) { this.fontSize = value; }
    public String getFill() { return this.fill; }
    public void setFill(String value) { this.fill = value; } 

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
