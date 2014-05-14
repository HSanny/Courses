package visitors;

import elements.SvgElement;

/** Top of Visitor Hierarchy. Declares
 * a visit method for every concrete element class
 * from SvgElement hierarchy. Any new element subclass
 * will require the corresponding visit method to be added in.
 * 
 * Different implementing classes (visitors) will provide appropriate
 * implementation to all methods.
 * 
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 11/02/11, 4:47 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 * @see SvgElement
 * @see types.Type
 * @see Validator
 * @see Java2DRenderer
 */

public interface Visitor {

    /* visit method for container elements */
	public void visitSvg(SvgElement e);
    public void visitGroup(SvgElement e);
    public void visitUse(SvgElement e);
    public void visitUnknown(SvgElement e);

    /* visit method for primitive graphics elements */
	public void visitRectangle(SvgElement e);
    public void visitCircle(SvgElement e);
    public void visitEllipse(SvgElement e);
    public void visitPolyline(SvgElement e);
    public void visitPolygon(SvgElement e);
    public void visitLine(SvgElement e);
    public void visitImage(SvgElement e);
    public void visitPath(SvgElement e);
    public void visitFrame(SvgElement e);

    /* visit method text element (has only one child character data) */
	public void visitText(SvgElement e);
}
