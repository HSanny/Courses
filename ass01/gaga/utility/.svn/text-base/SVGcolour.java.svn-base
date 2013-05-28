package utility;

import java.awt.Color;
import java.util.Hashtable;


/** utility functions for  handling SVG colours 
 * @author cwj 
 *
 */
public class SVGcolour {

	private static final String ERRORCOLOUR = "#ffff00"; // yellow
	
	/**
	 * map a SVG colour value to equivalent Java Color value
	 * @param svgColour svg string attribute purporting to be a color value 
	 * @return equivalent Java Color value default YELLOW if an error
	 * silent: no error messages
	 */
	public static Color svgColour2Java(String svgColour) {

		String svgc = svgColour.trim().toLowerCase();
		String svgHexval = ERRORCOLOUR;
		
		// special SVG attributes for colour are "none" and "inherit"
		if (svgc.equals("inherit")) {
			// cannot handle "inherit" in this colour scheme - must be handled in program logic
			return Color.decode(ERRORCOLOUR);
		}
		if (svgc.equals("none")) {
			// value "none" denotes no fill, rather than a colour: use white/alpha=0 fully transparent
			return new Color(255,255,255,0);
		}
		else if (svgc.charAt(0) == '#') {
			// handle hex codes
			svgHexval = svgc;
		
			// translate 3 hex digit codes #RGB to expanded 6 digit #RRGGBB
			if (svgHexval.length() == 4 & svgHexval.charAt(0) == '#') {
				// double each digit
				svgHexval = "#" + svgHexval.charAt(1) + svgHexval.charAt(1)  
				+ svgHexval.charAt(2) + svgHexval.charAt(2) 
				+ svgHexval.charAt(3) + svgHexval.charAt(3) ;
			}
		} else {
			// handle a colour function or identifier 
			// TODO recognise and translate rgb function with integer args
			// TODO recognise and translate rgb function with percentage args
			if ( svgc.startsWith("rgb")) {
				svgHexval = ERRORCOLOUR;          // TODO -------------------
			}
		
			else {
				// otherwise lookup SVG colour names to equivalent hex value
				svgHexval = lookupSvgColourName(svgc);
			}
		}
/* debug   System.err.println("svgColourJava (" + svgColour + "/" + svgc + ") maps to <" + svgHexval + ">"
		+ "java code: " + Color.decode(svgHexval));*/
		return Color.decode(svgHexval);
	}

	
	/**
	 * map a SVG named colour to equivalent 6 digit hex string representation including leading '#'
	 * @param svgName purported name of an SVG colour 
	 * @return string hex value of the colour, if an error return "#ffff00" (ie. yellow)
	 * silent : no error messages
	 */
	public static String lookupSvgColourName(String svgName) {

		final colourTable cmap = new SVGcolour().new colourTable();
		String cHex = cmap.lookup(svgName);
		if (cHex != null) {
			return cHex;
		} else {
			return ERRORCOLOUR;
		}
	}

	class colourTable {
		Hashtable<String,String> nameMap = new Hashtable<String,String>(148);
		public colourTable() {
			// set up nameMap as the table of equivalents	canonical names - lowercase canonical values 6 digit hex
			nameMap.put("black", "#000000");
			nameMap.put("navy", "#000080");
			nameMap.put("darkblue", "#00008B");
			nameMap.put("mediumblue", "#0000CD");
			nameMap.put("blue", "#0000FF");
			nameMap.put("darkgreen", "#006400");
			nameMap.put("green", "#008000");
			nameMap.put("teal", "#008080");
			nameMap.put("darkcyan", "#008B8B");
			nameMap.put("deepskyblue", "#00BFFF");
			nameMap.put("darkturquoise", "#00CED1");
			nameMap.put("mediumspringgreen", "#00FA9A");
			nameMap.put("lime", "#00FF00");
			nameMap.put("springgreen", "#00FF7F");
			nameMap.put("cyan", "#00FFFF");
			nameMap.put("aqua", "#00FFFF");
			nameMap.put("midnightblue", "#191970");
			nameMap.put("dodgerblue", "#1E90FF");
			nameMap.put("lightseagreen", "#20B2AA");
			nameMap.put("forestgreen", "#228B22");
			nameMap.put("seagreen", "#2E8B57");
			nameMap.put("darkslategray", "#2F4F4F");
			nameMap.put("darkslategrey", "#2F4F4F");
			nameMap.put("limegreen", "#32CD32");
			nameMap.put("mediumseagreen", "#3CB371");
			nameMap.put("turquoise", "#40E0D0");
			nameMap.put("royalblue", "#4169E1");
			nameMap.put("steelblue", "#4682B4");
			nameMap.put("darkslateblue", "#483D8B");
			nameMap.put("mediumturquoise", "#48D1CC");
			nameMap.put("indigo", "#4B0082");
			nameMap.put("darkolivegreen", "#556B2F");
			nameMap.put("cadetblue", "#5F9EA0");
			nameMap.put("cornflowerblue", "#6495ED");
			nameMap.put("mediumaquamarine", "#66CDAA");
			nameMap.put("dimgrey", "#696969");
			nameMap.put("dimgray", "#696969");
			nameMap.put("slateblue", "#6A5ACD");
			nameMap.put("olivedrab", "#6B8E23");
			nameMap.put("slategrey", "#708090");
			nameMap.put("slategray", "#708090");
			nameMap.put("lightslategray", "#778899");
			nameMap.put("lightslategrey", "#778899");
			nameMap.put("mediumslateblue", "#7B68EE");
			nameMap.put("lawngreen", "#7CFC00");
			nameMap.put("chartreuse", "#7FFF00");
			nameMap.put("aquamarine", "#7FFFD4");
			nameMap.put("maroon", "#800000");
			nameMap.put("purple", "#800080");
			nameMap.put("olive", "#808000");
			nameMap.put("gray", "#808080");
			nameMap.put("grey", "#808080");
			nameMap.put("skyblue", "#87CEEB");
			nameMap.put("lightskyblue", "#87CEFA");
			nameMap.put("blueviolet", "#8A2BE2");
			nameMap.put("darkred", "#8B0000");
			nameMap.put("darkmagenta", "#8B008B");
			nameMap.put("saddlebrown", "#8B4513");
			nameMap.put("darkseagreen", "#8FBC8F");
			nameMap.put("lightgreen", "#90EE90");
			nameMap.put("mediumpurple", "#9370DB");
			nameMap.put("darkviolet", "#9400D3");
			nameMap.put("palegreen", "#98FB98");
			nameMap.put("darkorchid", "#9932CC");
			nameMap.put("yellowgreen", "#9ACD32");
			nameMap.put("sienna", "#A0522D");
			nameMap.put("brown", "#A52A2A");
			nameMap.put("darkgray", "#A9A9A9");
			nameMap.put("darkgrey", "#A9A9A9");
			nameMap.put("lightblue", "#ADD8E6");
			nameMap.put("greenyellow", "#ADFF2F");
			nameMap.put("paleturquoise", "#AFEEEE");
			nameMap.put("lightsteelblue", "#B0C4DE");
			nameMap.put("powderblue", "#B0E0E6");
			nameMap.put("firebrick", "#B22222");
			nameMap.put("darkgoldenrod", "#B8860B");
			nameMap.put("mediumorchid", "#BA55D3");
			nameMap.put("rosybrown", "#BC8F8F");
			nameMap.put("darkkhaki", "#BDB76B");
			nameMap.put("silver", "#C0C0C0");
			nameMap.put("mediumvioletred", "#C71585");
			nameMap.put("indianred", "#CD5C5C");
			nameMap.put("peru", "#CD853F");
			nameMap.put("chocolate", "#D2691E");
			nameMap.put("tan", "#D2B48C");
			nameMap.put("lightgray", "#D3D3D3");
			nameMap.put("lightgrey", "#D3D3D3");
			nameMap.put("thistle", "#D8BFD8");
			nameMap.put("orchid", "#DA70D6");
			nameMap.put("goldenrod", "#DAA520");
			nameMap.put("palevioletred", "#DB7093");
			nameMap.put("crimson", "#DC143C");
			nameMap.put("gainsboro", "#DCDCDC");
			nameMap.put("plum", "#DDA0DD");
			nameMap.put("burlywood", "#DEB887");
			nameMap.put("lightcyan", "#E0FFFF");
			nameMap.put("lavender", "#E6E6FA");
			nameMap.put("darksalmon", "#E9967A");
			nameMap.put("violet", "#EE82EE");
			nameMap.put("palegoldenrod", "#EEE8AA");
			nameMap.put("lightcoral", "#F08080");
			nameMap.put("khaki", "#F0E68C");
			nameMap.put("aliceblue", "#F0F8FF");
			nameMap.put("honeydew", "#F0FFF0");
			nameMap.put("azure", "#F0FFFF");
			nameMap.put("sandybrown", "#F4A460");
			nameMap.put("wheat", "#F5DEB3");
			nameMap.put("beige", "#F5F5DC");
			nameMap.put("whitesmoke", "#F5F5F5");
			nameMap.put("mintcream", "#F5FFFA");
			nameMap.put("ghostwhite", "#F8F8FF");
			nameMap.put("salmon", "#FA8072");
			nameMap.put("antiquewhite", "#FAEBD7");
			nameMap.put("linen", "#FAF0E6");
			nameMap.put("lightgoldenrodyellow", "#FAFAD2");
			nameMap.put("oldlace", "#FDF5E6");
			nameMap.put("red", "#FF0000");
			nameMap.put("fuchsia", "#FF00FF");
			nameMap.put("magenta", "#FF00FF");
			nameMap.put("deeppink", "#FF1493");
			nameMap.put("orangered", "#FF4500");
			nameMap.put("tomato", "#FF6347");
			nameMap.put("hotpink", "#FF69B4");
			nameMap.put("coral", "#FF7F50");
			nameMap.put("darkorange", "#FF8C00");
			nameMap.put("lightsalmon", "#FFA07A");
			nameMap.put("orange", "#FFA500");
			nameMap.put("lightpink", "#FFB6C1");
			nameMap.put("pink", "#FFC0CB");
			nameMap.put("gold", "#FFD700");
			nameMap.put("peachpuff", "#FFDAB9");
			nameMap.put("navajowhite", "#FFDEAD");
			nameMap.put("moccasin", "#FFE4B5");
			nameMap.put("bisque", "#FFE4C4");
			nameMap.put("mistyrose", "#FFE4E1");
			nameMap.put("blanchedalmond", "#FFEBCD");
			nameMap.put("papayawhip", "#FFEFD5");
			nameMap.put("lavenderblush", "#FFF0F5");
			nameMap.put("seashell", "#FFF5EE");
			nameMap.put("cornsilk", "#FFF8DC");
			nameMap.put("lemonchiffon", "#FFFACD");
			nameMap.put("floralwhite", "#FFFAF0");
			nameMap.put("snow", "#FFFAFA");
			nameMap.put("yellow", "#FFFF00");
			nameMap.put("lightyellow", "#FFFFE0");
			nameMap.put("ivory", "#FFFFF0");
			nameMap.put("white", "#FFFFFF");

		}

		/**
		 * lookup RGB hex string for a colour name in the SVG colour table
		 * @param svgColourName a purported colour name, case independent
		 * @return string hex value or null if a bad name
		 */
		public String lookup(String svgColourName) {
			return nameMap.get(svgColourName.toLowerCase());	
		}

	}
public static void main(String args[]) {
	// tester
	System.out.println("red looks like " + lookupSvgColourName("red"));
	System.out.println("red translates to java color " + svgColour2Java("red"));
	System.out.println("BLACK looks like " + lookupSvgColourName("BLACK"));
	System.out.println("unknown name looks like " + lookupSvgColourName("unknown"));
}
}
