import java.io.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.regex.Matcher.*;


public class LineReader {
/**
 * normal case constructor for a LineReader over a BufferedReader
 * @param doDebug
 * @param fbr BufferedReader (normally the input file)
 */
	public LineReader(boolean doDebug, String headerFlag, BufferedReader fbr ){
		debugging = doDebug;
		br = fbr;
		header = Pattern.compile (headerFlag + "\\s(.*?)\\s" + headerFlag);
	};
	
	// pattern of header lines
	Pattern header;
	Matcher mc;
	
	/**
	 * constructor for testing LineReader functions over a String
	 * @param tvalue testing String
	 */
	public LineReader (String tvalue){
		debugging = true;
		currentLine = new String(tvalue); 
	}
	
	private BufferedReader br;
	private boolean debugging = false;
	private boolean reachedEOF = false;
	private String currentLine;
	
	public boolean atEOF() {
		return reachedEOF;
	}
	
	/**
	 * determine whether the currentLine contains a header line in the format "@@...@@"
	 * @return true if currentLine is in header format
	 */
	public boolean isHeader(){
		if (debugging) {
			System.out.println("isHeader: " + currentLine);
		}
		// in a header line the first 2 chars and last 2 chars must be '@'
		mc = header.matcher(currentLine);
		return mc.matches();
		// alternatives:
		// return (currentLine.matches("^@@ .* @@$"));
		// return (currentLine.startsWith("@@ ") && currentLine.endsWith("@@ "));
		/* //clumsy alternative:
		 * return ( 
		 * 		(currentLine.length() >= 4) &&
		 * 		(currentLine.charAt(0)=='@') & (currentLine.charAt(1) == '@')
		 * 		(currentLine.charAt(currentLine.length()-1)=='@') &
		 * 			(currentLine.charAt(currentLine.length()-2) == '@'));
		 */
		}

	private int lineCount = 0;
	public int getLineCount() {return lineCount;}

	/**
	 * advance to next line of LineReader's input file, incrementing lineCount
	 */
	public void advance() {
		try {
			String r = br.readLine();
			lineCount++;
			if (r == null) {
				reachedEOF = true;
				r = "";
			} 

			currentLine =  r;
		} catch (IOException e) {
			reachedEOF = true;
			currentLine =  "";
		}
	}
	/**
	 * extract the expected filename from current line, assumed to be a header line
	 * @return filename
	 */
	public String extractFileName() {
		return mc.group(1);
		// poor design alternatives
		//   return currentLine.substring(2,currentLine.length()-3).trim(); 
		/*   char [] name = new char[currentLine.length()-4];
		     currentLine.getChars(2, currentLine.length()-3, name, 0);
		     return new String(name).trim();
		 */
		/* String contents []= headerLine.split(Separator); 
		 * if (contents.length!=1) {// problem 
		 *   else return contents[0];
		 *   */
	}

	public String getCurrentLine() {
		return currentLine;
	}

}
