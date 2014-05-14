import java.io.*;

public class archive {
	/// version 4 with create, extract, toc and separator flag
	 String USAGE = "archive [-v|--verbose] [-f separator|--flag=separator] [ -t|--toc] [-c|--create] archivefilename [chunkfilename...]";


	// globals: command line switches - default values
	 boolean verbose = false;
	 boolean toc = false; // output table of contents of the archive
	 String headerFlag = "@@";
	 boolean create = false;
	 boolean extract = false;

	 String archFilename = null;
	 String chunkFileNames[]  = null;

     String displayedStr = ""; 
     String errorStr = null;

	/**
	 * extract files from a simple text file archive
	 * @param args
	 */
	public  void main(String[] args) {
		/*
		 * format: Smith
		 * each chunk has a header line then the lines from the original file
		 *	default flag for the header separators is @@
@@ filename1 @@
line 1 of file chunk 1
line 2 of file chunk  1
...
last line of file chunk 1
@@ filename2 @@
line 1 of file chunk 2
line 2 of file chunk 2
...
last line of file chunk 2
(end of file)
		 */

		/* 
		// String USAGE = "extract [(-f|--flag literalstring)|-t|--list|(-x|--extract filename...)|-v|--verbose] archivefilename";
		 *  -f|--flag literalstring
		 * 	-t|--list|--toc			 = table of contents (output a list of filenames to stdout)
		 * 	-x|--extract|--get archivefilename [filename...] = extract named files (or all files by default)
		 * 	-v|--verbose 	= report each filename as it is extracted
		 *  -c|--create archivefilename [filename...]
		 */
		// based on Michael A Smith, Object-Oriented Software in C++, Chapman & Hall 1993, section 16.3

		// pick up and check all command line arguments
		processArgs(args);
		// TODO for student homework: replace all command line argument processing
		//      with Swing graphic user interface filechoosers and controls

		if (verbose) { // abuse verbose for diagnostic reporting here - Naughty
			report("chunk separator: " + headerFlag);
			report(" archive=" + archFilename + " [");
			if (create) report("create,");
			if (extract) report("extract,");

			if (toc) report("list toc,");

			if (chunkFileNames != null && chunkFileNames.length > 0) {
				report("chunk names [");
				for (String cfn: chunkFileNames )
					report(cfn + ",");
				report("]");
			} else report("no chunk names]");
			flushReport();
		}	
		if (archFilename == null) {
			usageError("no archive file named");
			System.exit(1);
		}


		// despatch on class of command: create an archive or scan through existing
		if (create) {
            try{
			doCreate(archFilename, headerFlag, chunkFileNames, verbose);
            } catch (Exception e) {
                System.out.println("IO exception..");
            }
		}
		else if (extract || toc) {
			doScan(archFilename, headerFlag, chunkFileNames, verbose, extract, toc);
		}

	}

	/**
	 * scan an existing archive file as chunks separated by header lines
	 * extract chunks to their files, report table of contents, or both
	 * be verbose if required
	 * @param archFilename source file for archive
	 * @param headerFlag string marking a header line
	 * @param chunkFileNames names of chunks to select and extract NOT IMPLEMENTED defaults to all chunks
	 * @param verbose whether to be verbose in the scanning and extracting or listing
	 * @param extracting  whether to extract the chunks to the file named in their headers
	 * @param toc whether to produce a Table of Contents listing each chunk
	 */
	private  void doScan(String archFilename,
			String headerFlag, String[] chunkFileNames,
			boolean verbose, boolean extracting, boolean toc) {
		FileInputStream ar = null;
		// state variables for the extractor
		 int arLineCount = 0;	// count of lines read from the archive
		 int fileCount = 0;		// count of file chunks extracted from the archive

		 if (chunkFileNames != null && chunkFileNames.length != 0){
			 reportln("Warning: selecting only named chunks is not implemented for extract and list functions");
		 }
		 
		// open archive file
		try
		{
			ar = new FileInputStream(archFilename);
		} catch (FileNotFoundException f) {
			fileError("cannot open archive file " + archFilename);
			//System.exit(1);
		}

		// read line by line	
		LineReader lr = new LineReader(false, headerFlag, new BufferedReader(new InputStreamReader(ar)));
		lr.advance();

		BufferedWriter bw = null;

		// check that archive starts with a header line
		if (! lr.isHeader()) {
			syntaxError("header line expected ", archFilename + " line 1" );
			// skip lines looking for a header 
			while (! lr.atEOF() && ! lr.isHeader()) {
				lr.advance();
			}
			
			if (lr.atEOF()) {
				syntaxError("no header lines found in ", archFilename);
				//System.exit(1);
			} else {
				reportln("skipping " + (lr.getLineCount()-1) + " non-header lines");
			}
		}

		do {
			//	 get filename from header line
			String fn = lr.extractFileName(); // name of the current file chunk
			if (verbose) {
				System.out.println("archive chunk <" + fn + ">");
                displayedStr += "archive chunk <" + fn + ">" + "\n";
			}

			fileCount++;

			if (extracting) {
				// open current chunk file for output
				try {
					bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fn)));
				} catch (FileNotFoundException e) {
					fileError("cannot open output file " + fn);
				}
			}

			// scan through the lines in this chunk until another header or at EOF
			int chunkLineStart = lr.getLineCount();

			for(lr.advance();
			(! lr.atEOF() && ! lr.isHeader());
			lr.advance()) {
				if (extracting) {
					try {

						bw.write(lr.getCurrentLine());
						bw.newLine();
					} catch (IOException e) {
						fileError("cannot write to output file " + fn);
					}
				}
			}

			// we now know the number of lines in this chunk for the ToC information
			if (toc){
				System.out.println(fileCount + ": " + fn + " " + (lr.getLineCount()-chunkLineStart-1));
                displayedStr += fileCount + ": " + fn + " " + (lr.getLineCount()-chunkLineStart-1) + "\n";
			}

			if (extracting) {
				try {
					bw.close();
				} catch (IOException e) {
					fileError("cannot close " + fn);
				}
			}
		} while (! lr.atEOF());

		if (verbose) {
			System.out.println("archive contained " + fileCount + " file chunks");
            displayedStr += "archive contained " + fileCount + " file chunks\n";
		}
	}

	/**
	 * 
	 * @param archFilename  name of archive file to create
	 * @param headerFlag  separator pattern for header lines
	 * @param chunkFileNames  list of names of files to gather into archive
	 * @param verbose whether to report progress
	 */
	 void doCreate (String archFilename,
			String headerFlag, String[] chunkFileNames,
			boolean verbose) throws IOException{
        // build up buffer writer object
        BufferedWriter bw = null;
        bw = new BufferedWriter (new OutputStreamWriter (
                    new FileOutputStream (archFilename)));
        // tranverse all the chunk files
        for (int i = 0 ; i < chunkFileNames.length ; i ++ ){
            // build up buffer reader object
            BufferedReader br = new BufferedReader (new FileReader (chunkFileNames[i]));
            // file object
            File tempFile = new File (chunkFileNames[i]);
            // deal with file header of each trunk
            String fileHeader = headerFlag + " "+ tempFile.getName() +" "+ headerFlag + "\n";
            // write into new archive
            bw.write(fileHeader);
            // concatenate into displayed string
            display(fileHeader);
            String line = null;
            // read next line and check whether this is valid
            line = br.readLine();
            while ( line != null) {
                // tranverse all the line and concatenate it to the  string
                bw.write(line + "\n");
                display(line);
                line = br.readLine();
            }
            br.close(); // close read file
            // add empty line to better separate different file
            bw.write("\n");
        }
        // close new created file
        bw.close();
        //System.exit(0);

        // open new archive file for writing, catch errors
        // for each chunk {
        //    write a header for the chunk
        //	  open the chunk file for reading, catch errors 
        //    copy the chunk to the archive file
        //    do the right thing if verbose
        //    }
        // do the right thing if verbose
    }


    /**
     * print a syntax error message and archive file line number to stderr (non-fatal)
     * @param message
     * @param location
     */
     void syntaxError(String message, String location){
        System.err.println("format error in archive at " + location + ": " + message);
        if (errorStr == null) {
            errorStr = "format error in archive at " + location + ": " + message + "\n";
        } else {
            errorStr += "format error in archive at " + location + ": " + message + "\n";
        }
    }

    /**
     * print file error message to stderr and fatal exit
     * @param message
     */
     void fileError(String message){
        System.err.println("file error in archive:  " + message);
        if (errorStr == null) {
            errorStr = "file error in archive:  " + message + "\n";
        } else {
            errorStr += "file error in archive:  " + message + "\n";
        }
        //System.exit(1);
    }

    /**
     * print usage message to stderr and fatal exit
     * @param message	additional message info
     */
     void usageError(String message) {
        System.err.println("usage: " + USAGE + " " + message );
        if (errorStr == null) {
            errorStr = "usage: " + USAGE + " " + message + "\n";
        } else {
            errorStr += "usage: " + USAGE + " " + message + "\n";
        }
        //System.exit(1);	
    }

    /**
     * parse the command arguments as switches and filenames following the Usage
     * @param args
     */
     void processArgs(String [] args) {
        boolean pickupNextF = false; // to pick up the arg following a -f switch
        boolean pickupNextC = false; // to pick up the arg following a -c switch

        // process the args as switches or filenames
        int argpos = -1; // track current index position in args
        for (String arg : args){
            argpos++;

            // pick up arg value following old-style -f and -c flags
            if (pickupNextF) {
				headerFlag=arg;
				pickupNextF = false;
			}
			else if (pickupNextC) {
				archFilename = arg;
				pickupNextC = false;
			}
			
			else if(arg.length() >= 1 && arg.startsWith("-")){
				// arg is a switch, determine which kind of switch
			
				if (arg.equals("-c")) {
					create = true;
					pickupNextC = true; // collect the next argument as the archive file name
				}
				
				else if (arg.startsWith("--create")) {
					// expect --create=filename
					create = true;
					int argvx = arg.indexOf("=");
					archFilename = arg.substring(argvx+1);
				}

				else if (arg.equals("-f")) {
					pickupNextF = true;  // collect the next argument as the flag
				}
				
				else if (arg.equals("-x") || arg.equals("--extract")) {
					extract = true;
				}
				
				else if (arg.startsWith("--flag")) {
					// expect --flag=string
					int argvx = arg.indexOf("=");
					headerFlag = arg.substring(argvx+1);
					System.err.println("--flag value is <" + headerFlag + ">"); // DEBUG
				}
				
				else if (arg.equals("-t") || arg.equals("--toc") || arg.equals("--list")) {
					toc = true;
				}
				else if (arg.equals("-v") || arg.equals("--verbose")) {
					verbose = true;
				}
				else {
					usageError(" unknown switch " + arg);
				}
			} else {
				// assume the first non-switch arg to be the archive filename
				if (archFilename == null) {
					archFilename = arg;
				}
				else {
					// assume any remaining args to be filenames of chunks
					int argcRemaining = args.length - argpos;	
					chunkFileNames = new String[argcRemaining];
					System.arraycopy(args, argpos, chunkFileNames, 0, argcRemaining);
					break;  // we have swallowed up all the rest of the args
				}
			}
		}

		//logical consistency check: allow only create  or  extract/toc , but require some command
		if (create && (extract||toc)) {
			usageError("you can create or extract/list, not both");
            if (errorStr == null) {
                errorStr = "you can create or extract/list, not both" + "\n";
            } else {
                errorStr += "you can create or extract/list, not both" + "\n";
            }
        } else if (!create && !extract && ! toc) {
            usageError("a command is required");
        if (errorStr == null) {
            errorStr = "a command is required" + "\n";
        } else {
            errorStr += "a command is required" + "\n";
        }
		}
	}

	 void report(String s) {
		System.out.print(s);
	}
	 void reportln(String s) {
		System.out.println(s);
	}
	 void flushReport() {
		System.out.println();
	}
     void display(String str) {
        if (displayedStr == null) {
            displayedStr = str + "\n";
        } else {
            displayedStr += str + "\n";
        }
    }
}
