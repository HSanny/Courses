package scanner;

import elements.SvgContainerElement;
import elements.SvgElement;
import elements.SvgGraphicsElement;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;
import types.ContainerTypes;
import types.GraphicsTypes;
import types.Text;
import types.Type;

import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Collections;

/** SAX scanner which re-implements all SAX event handlers
 * to identify the tag, create an appropriate <em>SvgElement</em>
 * object, set up its parent and insert it in the right place in the
 * scene graph representing document tree. Also is tuned on error
 * and dtd events but does nothing when detects them.
 * 
 * @author abx
 *         Date: 8/02/11
 *         Time: 1:18 PM
 * @version $REV$
 *          Repository: $URL$
 * @see SvgElement
 * @see types.Type
 * @see Gaga
 */


public class SvgScanner extends DefaultHandler {

    /** The root of the document tree built by the SvgScanner object
     * when created by opening an svg-file*/
    SvgContainerElement rootElement;

    /** current element and its parent during the scanning process */
    SvgElement currentElement, parentElement;

     /* to control exclusion of illegally nested tags */
    private boolean recording;

    private final int indent = 2;
    private final String pad = Collections.nCopies(indent,' ').toString();
    private final StringBuffer padbuffer;

    private void stepRight() {
        padbuffer.append(pad);
    }

    private void stepLeft() {
        padbuffer.delete(padbuffer.length() - indent, padbuffer.length());
    }

    /**
     * parameter-less constructor does the same as its SAX parent
     */
    public SvgScanner() {
        super();
        padbuffer = new StringBuffer(pad);
    }

    /**
     * SvgScanner constructor which creates a SAX scanner object,
     * opens an svg file, scans and parses it; the resulting document tree root
     * is referenced by <code>this.root</code> field
     * @param fn the name of an svg file
     * @throws SAXException if the scanning fails due to document being malformed
     * @throws IOException if the file cannot be open for reading
     */
    public SvgScanner(String fn) throws SAXException, IOException {
        super();
        padbuffer = new StringBuffer(pad);
        XMLReader xr = XMLReaderFactory.createXMLReader();
        xr.setContentHandler(this);
        xr.setErrorHandler(this);

        FileReader fr = new FileReader(fn);
        xr.parse(new InputSource(fr));
    }

    public SvgContainerElement getRoot() {
        return this.rootElement;
    }


    /** the main method retained for testing
     * @param args names of svg files to be scanned and trees built.
     * @throws Exception in case if can't read files or they are bad XMLs
     */
    public static void main(String args[]) throws Exception {

        XMLReader xr = XMLReaderFactory.createXMLReader();
        //SAXOutputter so = new SAXOutputter();
        SvgScanner handler = new SvgScanner();
        xr.setContentHandler(handler);
        xr.setErrorHandler(handler);

        // Parse each file provided on the command line.
        for (String arg : args) {
            FileReader r = new FileReader(arg);          
            xr.parse(new InputSource(r));
        }
    }

    // SAX event handlers

    /**
     * event handler for the start document tag;
     * senses the document beginning and creates the root
     * of a tree element and its children (the root has
     * only one child --- the <svg> node which contains
     * all the graphics and group nodes as its children
     */
    @Override
    public void startDocument() {
        System.out.println("Start document");
        recording = true;
        rootElement = new SvgContainerElement();
        rootElement.name = "root";
        //rootElement.children = new ArrayList<SvgElement>();
        currentElement = rootElement;
        parentElement = null;
    }


    /**
     * that's it, the document is scanned, and the tree is built;
     * we only make crudest check that it was not malformed or empty
     */
    @Override
    public void endDocument() {
        System.out.println("End document; here is the document tree:");
        //((SvgContainerElement)currentElement.parent).children.add(currentElement);
        assert rootElement.children != null
                && rootElement.children.size() == 1
                && rootElement.children.get(0).name.endsWith("svg"):
                "The document was empty or malformed";

        //System.out.println(rootElement.children.get(0).name);
        //System.out.println(rootElement.children.size());
        //rootElement.children.get(0).prettyPrint(0, System.out);
    }

    /** The start of a new element event handler:
     * first it checks that the corresponding element is nested inside
     * the (next to) top <svg> element; then it determines the kind of the
     * element -- group-like or basic graphics -- and creates SvgContainerElement
     * or SvgGraphicsElement element correspondingly, sets its name ("unknown" if
     * the tag is not recognised) and type (container type or graphics type)
     *
     * @param uri is The Namespace URI, not used since we do not process Namespace
     * @param name is the opening (local) tag name ("svg", "g", "rect" etc)
     * @param qName is the qualified name (with prefix set for namespace), not used
     * @param atts are the tag's attributes (we only read them to store in our own
     * Map data structure (field in SvgElement class)
     */
    @Override
    public void startElement(String uri, String name, String qName, Attributes atts) {
        //System.out.println("The tag name is " + name);
        //System.out.println(currentElement == rootElement);
        assert (!name.equals("svg") || currentElement == rootElement):
                "Malformed SVG file: <svg> tag is nested inside";

        stepRight();

        SvgElement element;
        String typename = "Unknown";

        /*is this a container element ("svg, "g" or "use")?*/
        if (ContainerTypes.hasInstance(name)) {
            element = new SvgContainerElement(name,atts);
            typename = ContainerTypes.getValue(name).toString();
        }
        /*is this a graphics element ("rect", "circle" etc)?*/
        else if (GraphicsTypes.hasInstance(name)) {
            //elname = name;
            element = new SvgGraphicsElement(name,atts);
            typename = GraphicsTypes.getValue(name).toString();
        }
        /*this is an unknown (container) element
        (retained but will not be processed)*/
        else {
            element = new SvgContainerElement(name,atts);
            typename = ContainerTypes.getValue(name).toString();
        }

        try {
            /* Use Java Reflections to determine the Type class
            Based on the tag name, make this class,
            create the corresponding Type object, and
            set Type field of the newly created element to it */
            Class cls = Class.forName("types." + typename);
            /* the type classes all have only one constructor */
            //Type eltype = cls.getClass();
            //Object type  = cls.newInstance();
            Constructor constructor = cls.getConstructors()[0];
            Object type = constructor.newInstance(element);

            //System.out.println("Type object created? " + (type != null));
            //System.out.println(type.getClass().getName());
            type = cls.cast(type);

            element.setType((Type) type);
            //System.out.println("; its name is " + type.getClass().getName());
            //System.out.println("Attach the type " + element.getType().getName());
        } catch (ClassNotFoundException e) {
            e.printStackTrace(System.out);
        } catch (InstantiationException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }

        //System.out.println("Created element has type " + element.getType().getName());

        /* all this worth the trouble if current element is allowed to
         * have nested elements, otherwise we start to ignore following
         * tags until we reach the last non-container element */
        if (currentElement instanceof SvgContainerElement && recording) {
            element.setParent(currentElement);
            currentElement = element;
        } else {
            recording = false;
            /*System.out.printf("Created element %s%n", element.name);
            System.out.printf("Current element %s is not container: ",
                    currentElement.name);
            System.out.println("ignoring all nested elements");*/
        }
        //System.out.println("Going into " + currentElement.name);
    }

    /** The end of the currently processed element event handler;
     * Checks that the closing tag name matches the opening tag name
     * (which is also the tree element name) and returns the current
     * to element reference to the parent
     *
     * @param uri is The Namespace URI, not used since we do not process Namespace
     * @param name is the closing (local) tag name ("svg", "g", "rect" etc)
     * @param qName is the qualified name (with prefix set for namespace), not used
     */
    @Override
    public void endElement(String uri, String name, String qName) {

        stepLeft();
        /*assert (name.equals(currentElement.name)):
                "Malformed SVG document: broken order of nested tags";*/
        if (name.equals(currentElement.name))
            recording = true;
        if (recording)
            currentElement = currentElement.parent;
        //System.out.println(" to get back to " + currentElement.name);

    }

    /** The event handler to process character data inside an element &mdash;
     * not only character data (text) inside a node, but the internal content
     * of a node too. We only react to the <text> element, though, since
     * we need just text inside <text>...</text> nodes.
     *
     * @param ch the array of the text characters
     * @param start is the begin index value in the character array
     * @param length is the number of characters from array to use
     */
    @Override
    public void characters(char[] ch, int start, int length) {

        //System.out.println("The current node is " + currentElement.name);
        //System.out.println(currentElement.name);
        StringBuffer sb = new StringBuffer();
        if (currentElement.name.equals("text")) {
            for (int i = start; i < start + length; i++)
                sb.append(ch[i]);
            Type t = ((SvgContainerElement) currentElement).getType();
            ((Text)t).setContent(sb.toString());
            //System.out.println(sb.toString());
        }
    }

}