package visitors;
import types.paths.*;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.Line2D;
import java.awt.geom.QuadCurve2D;
import java.awt.geom.CubicCurve2D;
import java.awt.geom.Arc2D;

import java.awt.image.BufferedImage;
import java.util.Hashtable;
import java.util.ArrayList;
import static javax.swing.JFrame.setDefaultLookAndFeelDecorated;

import elements.SvgContainerElement;
import elements.SvgElement;
import utility.SVGcolour;
/** 
* @module PathRenderer.java
* @version 1.0 
* @since 2013-05-05
* @author Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
* 
* Edited by MacVim
* Class Info auto-generated by Snippet 
* */
public class PathRenderer extends Java2DRenderer implements PathVisitor {
    static final boolean PRINTING = false;
    private SvgElement e;
    private Point2D firstPoint = null; // used for closepath
    private Point2D currentPosition;

    private boolean drawable = false;
    public boolean isDrawable () { return this.drawable; }

    private boolean isLastInstructionMoveTo = false;
    public PathRenderer (SvgElement e, Graphics2D gbi, double scale, BufferedImage buffer) {
        super();
        this.e = e;
        this.gbi = gbi;
        this.scale = scale;
        this.buffer = buffer;
    }

    /** Check whether the first point of this path
     * is null, if it is, take the argument as first point.
     * */
    public void checkFirstPoint(double x, double y) {
        if (this.firstPoint == null) {
            this.firstPoint = new Point2D (x, y);
        }
    }

    public boolean isFirstInstruction() {
        // evalute whether this path instruction is 
        // the first instruction in currently visited
        // path svg node.
        return !this.currentPosition.isPointLegal();
    }

    public void visitMoveTo(MoveTo moveto) {
        if (PRINTING) {
            System.out.println("PCI: visitMoveTo invoked." + moveto.toString());
        }
        double x1=-1, y1=-1, x2=-1, y2=-1; // initialise to be illegal
        String command = moveto.getCommand();
        if (command.compareTo("M") == 0){
            // absolute movement
            x2 = moveto.getAbscissa();
            y2 = moveto.getOrdinate();
            if (this.isLastInstructionMoveTo) {
                // not first instruction, move to is treated as
                // a LineTo command.
                x1 = this.currentPosition.getAbscissa();
                y1 = this.currentPosition.getOrdinate();
                this.drawable = true;
            } else {
                this.drawable = false;
            }
        } else if (command.compareTo("m") == 0) {
            // relative movement
            if (!this.isFirstInstruction()) {
                // relative movement not the first instruction
                // of certain path.. so drawing is required.
                x1 = this.currentPosition.getAbscissa();
                y1 = this.currentPosition.getOrdinate();
                x2 = moveto.getAbscissa() + x1;
                y2 = moveto.getOrdinate() + y1;
                if (this.isLastInstructionMoveTo) {
                    // treated as line to since this is not the first
                    // move to instruction ..
                    // second parameter should be summation of relative
                    // location and currentPosition's absolute location
                    this.drawable = true;
                } else {
                    // treated as simple relative movement with
                    // no drawing required.
                    this.drawable = false;
                }
            } else {
                // first path instruction.. no drawing required.
                // no base for relative movement
                // So treat this as "absolute movement"
                x2 = moveto.getAbscissa();
                y2 = moveto.getOrdinate();
                this.drawable = false;
            }
        }
        this.currentPosition = new Point2D (x2, y2); 
        // update the indicator for the interpretation of 
        // next instruction of path. 
        this.isLastInstructionMoveTo = true;

        if (this.isDrawable() && x1 > 0 && x2 > 0) {
            // but the move command is in its first occurrence
            shape = new Line2D.Double(x1*scale,y1*scale,x2*scale,y2*scale);
            checkStyles(e);
            drawBorder(e, gbi, shape);
            //System.out.println("draw:"+ moveto.toString());
        }
        this.checkFirstPoint(x2, y2);
        if (moveto.hasNext()) {
            moveto.getNext().acceptPV(this);
        }
    }

    public void visitLineTo(LineTo lineto) {
        if (PRINTING) {
            System.out.println("PCI: visitLineTo invoked."+ lineto.toString());
            System.out.println("current position: "+ this.currentPosition.toString());
        }
        double x1=-1, y1=-1, x2=-1, y2=-1; // initialise to be illegal
        String command = lineto.getCommand();
        this.drawable = true;
        x1 = this.currentPosition.getAbscissa();
        y1 = this.currentPosition.getOrdinate();
        if (command.compareTo("L") == 0){
            // absolute movement
            x2 = lineto.getAbscissa();
            y2 = lineto.getOrdinate();
        } else if (command.compareTo("l") == 0) {
            x2 = lineto.getAbscissa() + x1;
            y2 = lineto.getOrdinate() + y1;
        } else if (command.compareTo("H") == 0) {
            x2 = lineto.getAbscissa() ;     
            y2 = y1 ;
        } else if (command.compareTo("h") == 0) {
            x2 = lineto.getAbscissa() + x1;     
            y2 = y1 ;
        } else if (command.compareTo("V") == 0) {
            x2 = x1 ;     
            y2 = lineto.getOrdinate() ;
        } else if (command.compareTo("v") == 0) {
            x2 = x1 ;     
            y2 = lineto.getOrdinate() + y1;
        }   
 
        this.currentPosition = new Point2D (x2, y2); 
        if (this.isDrawable() ) {
            // but the move command is in its first occurrence
            shape = new Line2D.Double(x1*scale,y1*scale,x2*scale,y2*scale);
            checkStyles(e);
            drawBorder(e, gbi, shape);
            //System.out.println("draw:"+ moveto.toString());
        }
        this.checkFirstPoint(x2, y2);
        this.isLastInstructionMoveTo = false;
        if (lineto.hasNext()) {
            lineto.getNext().acceptPV(this);
        }
    }
    public void visitClosePath(ClosePath closepath){
        double x1 = this.currentPosition.getAbscissa();
        double y1 = this.currentPosition.getOrdinate();
        double x2 = this.firstPoint.getAbscissa();
        double y2 = this.firstPoint.getAbscissa();
        if ( this.firstPoint != null ) {
            // but the move command is in its first occurrence
            shape = new Line2D.Double(x1*scale,y1*scale,x2*scale,y2*scale);
            checkStyles(e);
            drawBorder(e, gbi, shape);
            gbi.fill(shape);
            this.firstPoint = null; // after closepath, set first point to null
            //System.out.println("draw:"+ moveto.toString());
        }

        this.isLastInstructionMoveTo = false;
    };
    public void visitArc(Arc arc) {
        if (PRINTING) {
            System.out.println("PCI: visitArc invoked."+ arc.toString());
            System.out.println("current position: "+ this.currentPosition.toString());
        }
         // initialise to be illegal
        double x1 = this.currentPosition.getAbscissa();
        double y1 = this.currentPosition.getOrdinate();
        double x2=-1, y2=-1; // x,y for end point 
        double radiusX = arc.getRadiusX();
        double radiusY = arc.getRadiusY();
        double rotationX = arc.getRotationX();
        double largeArcFlag = arc.getLargeArcFlag();
        double sweepFlag = arc.getSweepFlag();
        
        String command = arc.getCommand();
        if (command.compareTo("A") == 0){
            // absolute movement
            x2 = arc.getEndPoint().getAbscissa();
            y2 = arc.getEndPoint().getOrdinate();
        } else if (command.compareTo("a") == 0) {
            x2 = arc.getEndPoint().getAbscissa() + x1;
            y2 = arc.getEndPoint().getOrdinate() + y1;
        } 
        this.drawable = true; 

        this.currentPosition = new Point2D (x2, y2); 
        if (this.isDrawable() ) {
            // but the move command is in its first occurrence
            // TODO create an arc2D from given parameters
            double x3 = (x1 + x2) /2.0 - radiusX * Math.sin(rotationX);
            double y3 = (y1 + y2) /2.0 + radiusX * Math.cos(rotationX);
            shape = new QuadCurve2D.Double(x1*scale,y1*scale,x3*scale,y3*scale,
                    x2*scale,y2*scale);
            checkStyles(e);
            drawBorder(e, gbi, new Line2D.Double(x1, y1, x2, y2));
            //System.out.println("draw:"+ arc.toString());
        }
        this.checkFirstPoint(x2, y2);
        this.isLastInstructionMoveTo = false;
        if (arc.hasNext()) {
            arc.getNext().acceptPV(this);
        }
    };

    public void visitCubicCurveTo(CubicCurveTo cct) {
        if (PRINTING) {
            System.out.println("PCI: visitCubicCurveTo invoked."+ cct.toString());
            System.out.println("current position: "+ this.currentPosition.toString());
        }
         // initialise to be illegal
        double x1=-1, y1=-1, x2=-1, y2=-1, x3=-1, y3=-1, x4=-1, y4=-1;
        String command = cct.getCommand();
        x1 = this.currentPosition.getAbscissa();
        y1 = this.currentPosition.getOrdinate();
        Point2D controlPointA = cct.getControlPointA();
        Point2D controlPointB = cct.getControlPointB();
        Point2D endPoint = cct.getEndPoint();
        
        if (command.compareTo("C") == 0){
            // absolute movement
            x2 = controlPointA.getAbscissa();
            y2 = controlPointA.getOrdinate();
            x3 = controlPointB.getAbscissa();
            y3 = controlPointB.getOrdinate();
            x4 = endPoint.getAbscissa();
            y4 = endPoint.getOrdinate();
        } else if (command.compareTo("c") == 0) {
            x2 = controlPointA.getAbscissa() + x1;
            y2 = controlPointA.getOrdinate() + y1;
            x3 = controlPointB.getAbscissa() + x2;
            y3 = controlPointB.getOrdinate() + y2;
            x4 = endPoint.getAbscissa() + x3;
            y4 = endPoint.getOrdinate() + y3;
        } 
        this.drawable = true; 

        this.currentPosition = new Point2D (x4, y4); 
        if (this.isDrawable() ) {
            // but the move command is in its first occurrence
            shape = new CubicCurve2D.Double(x1*scale,y1*scale,x2*scale,y2*scale,
                    x3*scale,y3*scale,x4*scale,y4*scale);
            checkStyles(e);
            drawBorder(e, gbi, shape);
            //System.out.println("draw:"+ cct.toString());
        }
        this.checkFirstPoint(x4, y4);
        this.isLastInstructionMoveTo = false;
        if (cct.hasNext()) {
            cct.getNext().acceptPV(this);
        }
    };

    public void visitQuadraticCurveTo(QuadraticCurveTo qct) {
        if (PRINTING) {
            System.out.println("PCI: visitQuadraticCurveTo invoked."+ qct.toString());
            System.out.println("current position: "+ this.currentPosition.toString());
        }
         // initialise to be illegal
        double x1=-1, y1=-1, x2=-1, y2=-1, x3=-1, y3=-1;
        String command = qct.getCommand();
        x1 = this.currentPosition.getAbscissa();
        y1 = this.currentPosition.getOrdinate();
        Point2D controlPoint = qct.getControlPoint();
        Point2D endPoint = qct.getEndPoint();
        
        if (command.compareTo("Q") == 0){
            // absolute movement
            x2 = controlPoint.getAbscissa();
            y2 = controlPoint.getOrdinate();
            x3 = endPoint.getAbscissa();
            y3 = endPoint.getOrdinate();
        } else if (command.compareTo("q") == 0) {
            x2 = controlPoint.getAbscissa() + x1;
            y2 = controlPoint.getOrdinate() + y1;
            x3 = endPoint.getAbscissa() + x2;
            y3 = endPoint.getOrdinate() + y2;
        } 
        this.drawable = true; 

        this.currentPosition = new Point2D (x3, y3); 
        if (this.isDrawable() ) {
            // but the move command is in its first occurrence
            shape = new QuadCurve2D.Double(x1*scale,y1*scale,x2*scale,y2*scale,
                    x3*scale,y3*scale);
            checkStyles(e);
            drawBorder(e, gbi, shape);
            //System.out.println("draw:"+ qct.toString());
        }
        this.checkFirstPoint(x3, y3);
        this.isLastInstructionMoveTo = false;
        if (qct.hasNext()) {
            qct.getNext().acceptPV(this);
        }
    };
}
