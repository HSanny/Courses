package gui;

import static javax.swing.JFrame.setDefaultLookAndFeelDecorated;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;
import javax.swing.text.DefaultHighlighter.DefaultHighlightPainter;
import javax.swing.text.*;
/**
 * Starting point for the graphical user interface, responsible
 * for creating and displaying all user interface elements.
 *
 * @author Ian Barnes
 * @author Chris Johnson
 * @version $Revision: 1532 $, $Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $
 */
public class View {

    Model model;
    JFrame frame;
    PictrPanel panel;

    /** Indicator for controlling */
    boolean singleDisplay = true;
    boolean graphicalDisplay = true;
    boolean scaleIsLarge = true;

    public String sourceFilename;
	 /**
     * Create and show the main window and its contents.
     */
    public View(Model m){
    	model = m;
        panel = new PictrPanel(model);
    }

    /** 
     * Welcome Menu for Gaga program includes 
     *  1, File Chooser to open local SVG file
     *  2, Dialog to accept URL address and 
     *    renderer it as downloaded local file.
     * */
    public void welcomeWindowDisplay()  {
//{{{
        // new window to enable user to choose source of file:
        //     - local svg file 
        //     - remote svg file
        final JFrame welcomeWindow = new JFrame("Welcome to Gaga, your friendly SVG renderer..");
        welcomeWindow.setSize(500, 400);
        welcomeWindow.setLocation(600, 400);
        welcomeWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        // panel definition
        final Container content = welcomeWindow.getContentPane();
        content.setBackground(Color.white);
        content.setLayout(null); 
        final JPanel buttonPanels = new JPanel ();
        buttonPanels.setBounds(150, 80, 200, 250);
        // button and label definition
        final JButton remoteOption = new JButton("Open remote svg file");
        final JButton localOption = new JButton("Open local svg file");
        final JButton quitOption = new JButton("Exit");
        final JLabel authorInfo = new JLabel("Jimmy Lin U5223173");
        // set location for components
        remoteOption.setBounds(180, 80, 150, 40);
        localOption.setBounds(180, 150, 150, 40);
        quitOption.setBounds(180, 220, 150, 40);
        authorInfo.setBounds(180, 320, 150, 40);
        // define innder classes to specify the actions.
        class RemoteOptionButtonListener implements ActionListener {
            public void actionPerformed(ActionEvent event){
                System.out.println("Read from remote url..");
                sourceFilename = "./temp.svg";
                String URLaddress = (String) JOptionPane.showInputDialog(
                        frame, "Please input URL of SVG file:\n",
                        "Remote SVG file", JOptionPane.PLAIN_MESSAGE,
                        null, null, "http://");
                download(URLaddress, sourceFilename);
                welcomeWindow.dispose();
                model.open(sourceFilename);
                mainViewDisplay(URLaddress);
            }
        }
        class LocalOptionButtonListener implements ActionListener {
            public void actionPerformed(ActionEvent event){
                System.out.println("Read from local directory..");
                sourceFilename = pickupLocalFile();
                if (sourceFilename != null) {
                    welcomeWindow.dispose();
                    model.open(sourceFilename);
                    mainViewDisplay(sourceFilename);
                } else {
                    System.exit(1);
                }
            }
        }
        class QuitOptionButtonListener implements ActionListener {
            public void actionPerformed(ActionEvent event) {
                System.out.println("Exit. Good bye!");
                welcomeWindow.dispose();
                System.exit(1);
            }
        }
        remoteOption.addActionListener(new RemoteOptionButtonListener());
        localOption.addActionListener(new LocalOptionButtonListener ());
        quitOption.addActionListener(new QuitOptionButtonListener());
        content.add(remoteOption);
        content.add(localOption);
        content.add(quitOption);
        content.add(authorInfo);
        welcomeWindow.setVisible(true);
//}}}
    }
    
    /** 
     * Main frame for Gaga program includes
     *  1, graphical representation
     *  2, textual representation 
     *      with hightlight functionality - spec(5)
     *  3, simultaneous display - spec(6)
     * */
    public void mainViewDisplay(String SvgFileName) {
//{{{
        frame = new JFrame();
        frame.setSize(new Dimension(1000, 800));
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setDefaultLookAndFeelDecorated(true);

        // the control panel has a label (and allows more controls to be added)
        final JPanel controlPanel = new JPanel();

        // control panel configuration
        controlPanel.setPreferredSize(new Dimension(250, 40));
        controlPanel.setMaximumSize(new Dimension(1000, 100));
        controlPanel.setLayout(null);

        // the scroll pane contains the drawing canvas
        final JScrollPane graphScrollPane = new JScrollPane(panel,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        graphScrollPane.setBounds(125, 100, 725, 600);

        // Button definition 
        final JButton scaleButton = new JButton("zoom in"); // zoom controller
        final JButton displayModeButton = new JButton("Double display"); // Display mode controller
        final JButton textOrGraphButton = new JButton("Textual-Only display"); // graph-text controller
        final JButton exitButton = new JButton("Exit"); // exit button
        int BUTTON_HEIGHT = 30, XOFFSET = 175;
        int FIRST_LINE_Y = 0, SECOND_LINE_Y = 50;
        // Button location in control panel
        scaleButton.setBounds(XOFFSET+55, SECOND_LINE_Y, 100, BUTTON_HEIGHT);
        displayModeButton.setBounds(XOFFSET+205, SECOND_LINE_Y, 200, BUTTON_HEIGHT);
        textOrGraphButton.setBounds(XOFFSET+450, SECOND_LINE_Y, 200, BUTTON_HEIGHT);
        exitButton.setBounds(700, FIRST_LINE_Y  + 14, 80, BUTTON_HEIGHT);

        // label definition
        final JLabel docName = new JLabel("filename: " + SvgFileName);
        final JLabel authorLabel = new JLabel("Author: Jimmy Lin  UID: u5223173");
        final JLabel collegeLabel = new JLabel("College Of Engineering and Computer Science");
        final JLabel univeristyLabel = new JLabel("Australian National University");
        docName.setBounds(150, FIRST_LINE_Y, 550, 50);
        authorLabel.setBounds(375, 700, 250, 30);
        collegeLabel.setBounds(350, 720, 300, 30);
        univeristyLabel.setBounds(390, 740, 220, 30);

        // text panel initialisation
        final JTextPane XMLTextPane = new JTextPane();
        // invoke subfunction to generate required text.
        DefaultStyledDocument document = getXMLStyledDocument(sourceFilename);
        XMLTextPane.setStyledDocument(document);

        final JScrollPane XMLTextScrollPane = new JScrollPane(XMLTextPane,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        XMLTextScrollPane.setBounds(125, 100, 725, 600);
        XMLTextScrollPane.setVisible(false);

        // Action for scale button:
        // toggle between large or default scale rendering of embedded drawing objects
        class ScaleButtonListener implements ActionListener {
            public void actionPerformed(ActionEvent event){
                scaleIsLarge = ! scaleIsLarge;
                if (scaleIsLarge) {
                    setScale(1.0);
                    scaleButton.setText("Zoom in");
                    panel.repaint();
                } else {
                    setScale(2.0);
                    scaleButton.setText("Zoom out");
                    panel.repaint();
                }
            }
        }
        class DisplayModeButtonListener implements ActionListener {
            public void actionPerformed(ActionEvent event) {
                singleDisplay = !singleDisplay;
                //  single and double display switch 
                if (singleDisplay) {
                    // single mode display
                    displayModeButton.setText("Side-by-Side display");
                    textOrGraphButton.setVisible(true);
                    if (graphicalDisplay) {
                        // display graph panel
                        graphScrollPane.setVisible(true);
                        // hide text panel
                        XMLTextScrollPane.setVisible(false);
                        // move graph panel to center
                        graphScrollPane.setBounds(125, 100, 725, 600);
                    } else {
                        // hide scale button
                        scaleButton.setVisible(false);
                        // hide graph panel
                        graphScrollPane.setVisible(false);
                        // show text panel and move to center for display
                        XMLTextScrollPane.setVisible(true);
                        XMLTextScrollPane.setBounds(125, 100, 725, 600);
                    }
                } else {
                    // double simultaneous display
                    // switch button text
                    displayModeButton.setText("Single display");
                    // hide unnecessary button and show needed button
                    textOrGraphButton.setVisible(false);
                    scaleButton.setVisible(true);
                    // show both text panel and graph panel
                    XMLTextScrollPane.setVisible(true);
                    graphScrollPane.setVisible(true);
                    // set suitable location 
                    XMLTextScrollPane.setBounds(525, 100, 425, 600);
                    graphScrollPane.setBounds(50, 100, 450, 600);
                }
            }
        }
        class GraphOrTextButtonListener implements ActionListener{
            public void actionPerformed(ActionEvent event) {
                graphicalDisplay = !graphicalDisplay;
                //  graphical and textual display switch 
                if (graphicalDisplay) {
                    textOrGraphButton.setText("Textual-Only display");
                    graphScrollPane.setVisible(true);
                    scaleButton.setVisible(true);
                    XMLTextScrollPane.setVisible(false);
                    graphScrollPane.setBounds(125, 100, 725, 600);
                } else {
                    textOrGraphButton.setText("Graphical-Only display");
                    graphScrollPane.setVisible(false);
                    scaleButton.setVisible(false);
                    XMLTextScrollPane.setVisible(true);
                    XMLTextScrollPane.setBounds(125, 100, 725, 600);
                }
            }
        }
        class ExitButtonListener implements ActionListener {
            public void actionPerformed(ActionEvent event){
                frame.dispose();
                System.exit(0);
            }
        }
        // add action listener to buttons
        scaleButton.addActionListener(new ScaleButtonListener());
        displayModeButton.addActionListener(new DisplayModeButtonListener());
        textOrGraphButton.addActionListener(new GraphOrTextButtonListener());
        exitButton.addActionListener(new ExitButtonListener());
        // add button to panels of main frame
        controlPanel.add(docName);
        controlPanel.add(scaleButton);
        controlPanel.add(displayModeButton);
        controlPanel.add(textOrGraphButton);
        controlPanel.add(exitButton);
        controlPanel.add(graphScrollPane);
        controlPanel.add(XMLTextScrollPane);
        controlPanel.add(authorLabel);
        controlPanel.add(collegeLabel);
        controlPanel.add(univeristyLabel);
        frame.setContentPane(controlPanel);

        // set up Model-View-Controller interaction from model to this panel
        model.addObserver(panel);
        // NOTE: programmer must add update and notify calls to make this work if the model changes    	

        frame.setTitle("Gaga SVG viewer " + "$Revision: 2.0$");
        frame.setVisible(true);
        frame.setResizable(false);
        //}}}
    }

    /** 
     * First download required SVG file from URL address
     *  save it in local directory and then open it as
     *  local file.
     * */
    public void download (String source, String savePath) {
        //{{{
        // local variable declaration
        URL url; // specified url socket
        URLConnection con;  // a connection to the url.
        DataInputStream dis;  // input stream that will read data from the file.
        FileOutputStream fos; //used to write data from inut stream to file.
        byte[] fileData;  //byte aray used to hold data from downloaded file.
        try {
            url = new URL(source);
            // open the url connection.
            con = url.openConnection(); 
            // get a data stream from the url connection.
            dis = new DataInputStream(con.getInputStream()); 
            fileData = new byte[con.getContentLength()];
            // fill byte array with bytes from the data input stream
            for (int x = 0; x < fileData.length; x++) { 
                fileData[x] = dis.readByte();
            }
            dis.close(); // close the data input stream
            //create an object representing the file we want to save
            fos = new FileOutputStream(new File(savePath));  
            fos.write(fileData);  // write out the file we want to save.
            fos.close(); // close the output stream writer
        }
        catch(MalformedURLException m) {
            // print out the exception instead of throw it
            System.out.println(m);
        }
        catch(IOException io) {
            // print out the exception instead of throw it
            System.out.println(io);
        }
        //}}}
    }

    /** 
     * Open up a file chooser to select the source SVG file 
     *  in local directory.
     * */
    public static String pickupLocalFile() {
        //{{{
        String sourceFilename = null;
        // no arg, prompt for console input
        System.out.print("Choose SVG filename: ");
        JFileChooser chooser = new JFileChooser(  // set default directory
                new File("/Users/JimmyLin/workspace/2100COMP/ass02/svgSamples/"));
        chooser.setDialogTitle("Open a local SVG file");
        FileNameExtensionFilter filter = new FileNameExtensionFilter("SVG document","svg");
        chooser.setFileFilter(filter);
        int returnVal = chooser.showOpenDialog(null);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            File chosenFile = chooser.getSelectedFile();
            sourceFilename = chosenFile.getPath() ;
            System.out.println("You chose to open this file: " + sourceFilename);
        } else if (returnVal == JFileChooser.CANCEL_OPTION) {
            System.out.println("Empty input.. Please choose a file next time..");
        } else if (returnVal == JFileChooser.ERROR_OPTION) {
            System.out.println("File Selection Error.. Please select again..");
        }

        if (! sourceFilename.endsWith((".svg"))) {
            System.err.println("filename " + sourceFilename + " requires .svg extension");
            System.exit(1);
        }
        return sourceFilename;
        //}}}
    }

    /** Open one SVG file and read the content of XML format
     *  and return one string containing all the contents.
     * */
    public static DefaultStyledDocument getXMLStyledDocument(String sourceFilename) {
        //{{{
        // read XML file into memory
        BufferedReader buffer = null;
        // JTextField for textual display
        try {
            buffer = new BufferedReader(new FileReader(sourceFilename));
        } catch (IOException e){
            System.out.println("file open exception...");
            System.exit(1);
        }  
        DefaultStyledDocument document = new DefaultStyledDocument ();
        // define document styles
        Style style = document.addStyle("", null);
        Style elementStyle = document.addStyle("", style);
        Style attributeStyle = document.addStyle("", style);
        Style valueStyle = document.addStyle("", style);
        StyleConstants.setForeground(elementStyle, Color.RED);
        StyleConstants.setForeground(attributeStyle, Color.BLUE);
        StyleConstants.setForeground(valueStyle, Color.GREEN);
        String XMLtext = "";
        String line;
        // assistant indicator
        boolean isComment = false;
        int indent = 0;
        while ( true) {
            try{
                line = buffer.readLine();
                if (line == null) break;
                // preprocessing of each line
                line = line.replace(">", "> ");
                line = line.replace("\t", "    ");
                line = line.replace("/>", " />");
                line = line.replace("</", " </");
                line = line.replace("?>", " ?>");
                line = line.replace("\">","\" > ");
                if (line.length() <= 1) {
                    XMLtext += "\n";
                    continue;
                }
                if (line.contains("<!DOCTYPE")) {
                    document.insertString(document.getLength(), "<!DOCTYPE ", elementStyle);
                    document.insertString(document.getLength(), line.replace("<!DOCTYPE", "") + "\n", style);
                    continue;
                }
                // deal with XML comment
                if (line.contains("-->")){
                    // end of comment, consider this case first
                    isComment = false;
                    document.insertString(document.getLength(), line + "\n" , style);
                    continue;
                } else if (line.contains("<!--<") || isComment) {
                    // start of comment or middle line of comment 
                    document.insertString(document.getLength(),  line + "\n" , style);
                    isComment = true;
                    continue;
                } 
                String [] tempStrArray = line.split(" ");
                for (String temp : tempStrArray) {

                    if (temp.length() <= 1 ) {
                        if (temp.matches(">")) {
                            document.insertString(document.getLength() - 1, 
                                  temp  , elementStyle);
                        }
                        continue;
                    }

                    if (temp.charAt(0) != '<') {
                        if (temp.contains("/>") || temp.contains("?>") || temp.contains(">") ) {
                            document.insertString(document.getLength() - 1, 
                                " " + temp , elementStyle);
                        }
                        else if (temp.contains("=")) {
                            String [] attributeValue = temp.split("=");
                            document.insertString(document.getLength(), 
                                produceIndent(indent) + attributeValue[0] + "=", attributeStyle);
                             document.insertString(document.getLength(), 
                                 attributeValue[1]  + "\n", valueStyle);
                        } else {
                            document.insertString(document.getLength(), 
                                produceIndent(indent) + temp + "\n", style);
                        }
                    } else {
                        document.insertString(document.getLength(), 
                                produceIndent(indent) + temp + "\n", elementStyle);
                    }
                    if (temp.contains("/>") || temp.contains("</") || temp.contains("?>")) indent --; 
                    else if (temp.contains("<")) indent ++;
                }
            } catch (Exception e) {
                break;
            }
        }
        return document;
        //}}}
    }

    public static String produceIndent (int indent) {
        String str = "";
        for (int i = 0 ; i < indent ; i ++) {
            str += "      "; // 6 whitespace 
        }
        return str;
    }
    public static String produceIndentForTag (int indent) {
        String str = "";
        str = produceIndent(indent - 1);
        if (indent > 0) {
            str += "   "; // 3 whitespace
        }
        return str;
    }
    /**
     * Change the scale of the rendered image 
     */
    public void setScale(double scaleFactor){
        panel.setScale(scaleFactor);	
    }
}
