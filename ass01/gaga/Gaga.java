import elements.SvgContainerElement;
import org.xml.sax.SAXException;

import scanner.SvgScanner;
import visitors.Java2DRenderer;
import visitors.Validator;
import gui.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.Scanner;
import java.io.*;

/**
 * <p>
 * This is the root class of the Gaga system. 
 * For the GUI version this creates a model and view,
 *  then delegates handling the input file to model,
 *   and setting up the graphics output to its.
 * Other types of processing will be added later.
 * <p/>
 * <p/>
 * <p>Run it as follows: <code>java Gaga [<em>file.svg</em>]</code></p>
 * <p>If no command line argument is given the program will prompt to request a filename from input.
 * <p/>
 * <p><em>The name Gaga means "General Assistant for Graphics Authors"
 * </em></p>
 *
 * @author Alexei Khorev (abx) (pre-VCS creator)
 * @author Chris Johnson
 * @author $Author: u8600417 $ 
 *         Created: 26/02/11, 9:08 PM
 * @version $Rev: 1531 $ ($Date: 2011-05-04 11:50:37 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 * @see scanner.SvgScanner
 * @see elements.SvgElement
 * @see visitors.Visitor
 */
// u5223173


public class Gaga {


    public static void main(String[] args) {
        Model model = new Model();
        model.open(args);
        View view = new View(model);
        Controller controller = new Controller(model, view);
        
        view.setScale(1.0d); // test
    }
 
}
