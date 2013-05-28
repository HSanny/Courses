  Lists of files with large change.. (deserve being examined for marking)
   for specification (1):
    - types/Frame.java (new file)
    - visitors/Validator.java (interface extended)
    - visitors/Java2drenderer.java (responds to extension of interface)
    - visitors/Visitor.java (responds to extension of interface)
   for specification (2):
    - types/Polyline.java (extend fields and methods)
    - visitors/Java2drenderer.java (modify visit method)
   for specification (3):
    - visitors/Java2drenderer.java (I did not create a new visitor but modify original one)
    - many files in types/  (exten fields and methods)
   add switch for controlling report:
    - visitors/validator.java
  
   There is a make.sh file for convenient building.
  
  Note that there are two switch for controlling console output
    - isPrinted: as specified to control reporting element tree, DEFAULT to OFF 
    - TIME_COMPARISON_PRINTED:is for reporting the result of my solution for specification (3)
   the second is DEFAULT to be true for convenient marking
 
  This is my solution for the specification (3) 
  Having read the original framework of Gaga, I prefer not to create a new visitor but directly
       modify the java2DRender file (If create a new visitor, then I have to modify related codes 
       in GUI part of gaga in order to take effect the new visitor)
  First, I have changed the implementation of most of methods, including:
       - visitSvg (invoke the original implementation and my new implementation 
           while each time of resizing for comparison of time consumption)
       - visitRectangle
       - visitFrame (This is also for specification (1) )
       - visitEllipse 
       - visitCircle
       - visitText
  Note that I did not implement the solution to specification (3) for visitPolyline
 
  Besides, I add corresponding fields to almost all the types, these types including:
       - Rectangle 
       - Frame
       - Polyline
       - Ellipse
       - Circle
 
  Another thing deserve being mentioned, I add a new field called isParsed to
    SvgElement file to record whether this element has been parsed before:
       - if not parsed before, we should access the uniform hashmap 
           called attributes and restore parsed data in the newly 
           added fields with specific (not uniform) data type
       - if this element is parsed, we can directly access the 
           newly created field of each type for attributes information

