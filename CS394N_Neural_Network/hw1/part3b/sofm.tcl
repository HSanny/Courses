#!/usr/bin/wish
# Sofm.tcl
if [catch { load ./sofm.so sofm} result] {
    puts stderr $result
}

proc Stat {path labl width tvar} {
    frame $path -bd 1 -relief ridge
    label $path.hdr -width $width -text $labl
    label $path.val -width $width -textvar $tvar
    pack $path.hdr $path.val
}

#********************** Net Components *******************************

# simufile
set simufile "mysimu"
set initial_snapshot 1
set som .som
set epoch -1
set strokeid 0

proc GetLabel {i j} {
    global units vectors nvectors

    set unit [get_unit $units $i $j]
    set lblcnt [unitdata_labelcount_get $unit]
    set labels [unitdata_labels_get $unit]
    for {set k 1} {$k <= $lblcnt} {incr k} {
	set lbl [get_label $labels $k]
	if {$lbl < $nvectors} {
	    set vec [get_vector $vectors $lbl]
	    set str [inputvectors_label_get $vec]
	    PlaceText $i $j $k $str
	}
    }
}

proc PlaceText {i j k str} {
    global canvas h w

    set mh [expr $h / 2]
    set mw [expr $w / 2]
    if {$k == 1} {
	set u [expr $j*$w + $mw]
	set v [expr $i*$h + $mh]
	set color blue
    } elseif {$k == 2} {
	set u [expr $j*$w + 12]
	set v [expr $i*$h + 12]
	set color purple
    } elseif {$k == 3} {
	set u [expr $j*$w + ($w - 12)]
	set v [expr $i*$h + ($h - 12)]
	set color orange
    } elseif {$k == 4} {
	set u [expr $j*$w + ($w - 12)]
	set v [expr $i*$h + 12]
	set color cyan
    } elseif {$k == 5} {
	set u [expr $j*$w + 12]
	set v [expr $i*$h + ($h - 12)]
	set color pink
    } elseif {$k == 6} {
	set u [expr $j*$w + $mw]
	set v [expr $i*$h + 12]
	set color yellow
    } elseif {$k == 7} {
	set u [expr $j*$w + $mw]
	set v [expr $i*$h + ($h - 12)]
	set color red
    } elseif {$k == 8} {
	set u [expr $j*$w + 12]
	set v [expr $i*$h + $mh]
	set color white
    } elseif {$k == 9} {
	set u [expr $j*$w + ($w - 12)]
	set v [expr $i*$h + $mh]
	set color gold
    } else {
	return
    }
    $canvas create text $u $v -text $str -fill $color -tag label \
	-font -misc-fixed-bold-r-normal--15-140-75-75-c-90-iso8859-1
}

proc Step {} {
    global continuing initial_snapshot t tend message

    if {$t > $tend} {
	puts "Last snapshot!"
	return
    }
    if {$continuing} {
	if {$initial_snapshot} {
	    iterate_snapshots
	    set initial_snapshot 0
	}
	read_snapshot
	DisplayMap $t
    } else {
	init_weights
	setup_labels
    }
}

proc DisplayMap {t} {
    global nx ny canvas epoch message nvectors misclassified

    set epoch $t
#   set message "Displaying epoch $t"
    set message "Epoch $t: [expr 100.0-100.0*$misclassified/$nvectors]% correct"
    $canvas delete label
    for {set i 0} {$i < $nx} {incr i} {
	for {set j 0} {$j < $ny} {incr j} {
	    GetLabel $i $j
	}
    }
    update
}

proc Train {} {
    global continuing startt tend message

    if {!$continuing} {
	init_weights
	setup_labels

    }
    for {set t $startt} {$t < $tend} {incr t} {
	set d [present_inputs $t]
	if {$d == 1} {
	    DisplayMap $t
	}
    }
    set message "Finished training"
    finish
}

proc Reset {} {
    global simufile lblfil message

    set message "Map cleared"
    if {[winfo exists .som.frame.c]} {
	.som.frame.c delete label
    }
    init_params $simufile
    read_inputs
}

#******************** Cluster borders *****************

proc StrokeBegin {w x y} {
    global stroke strokeid

    catch {unset stroke}
    set stroke($strokeid,N) 0
    set stroke($strokeid,0) [list $x $y]
}

proc Stroke {w x y} {
    global stroke strokeid

    set last $stroke($strokeid,$stroke($strokeid,N))
    incr stroke($strokeid,N)
    set stroke($strokeid,$stroke($strokeid,N)) [list $x $y]
    eval {$w create line} $last {$x $y -fill red -tag segments$strokeid}
}

proc StrokeEnd {w x y} {
    global stroke strokeid canvas

    set points {}
    for {set i 0} {$i <= $stroke($strokeid,N)} {incr i} {
	append points $stroke($strokeid,$i) " "
    }
    $w delete segments$strokeid
    eval {$w create line} $points \
	{-fill red -tag line$strokeid -joinstyle round -smooth true}
    $canvas bind line$strokeid <3> {
	$canvas delete [$canvas find closest %x %y]
    }
    incr strokeid
}


#********************
Reset

frame $som
frame $som.menu -relief ridge -bd 2
button $som.menu.step -text STEP -command Step
button $som.menu.reset -text RESET -command Reset
button $som.menu.train -text TRAIN -command Train
button $som.menu.quit -text QUIT -command exit
label $som.menu.label -relief ridge -text "Simulation file:"
entry $som.menu.vfil -relief ridge -width 20 -textvar simufile
entry $som.menu.lfil -relief ridge -width 20 -textvar lblfil
pack $som.menu.step $som.menu.reset $som.menu.train $som.menu.quit \
    $som.menu.label $som.menu.vfil $som.menu.lfil -side left
pack $som.menu -side top -pady 3

set height [expr $nx*(800/$nx)]
set width [expr $nx*(800/$nx)]

frame $som.frame
pack $som.frame -fill both -expand yes
set canvas $som.frame.c
canvas $canvas -width $width -height $height -relief sunken \
    -bd 2 -bg black
pack $canvas
pack $som
bind $canvas <Button-1> {StrokeBegin %W %x %y}
bind $canvas <B1-Motion> {Stroke %W %x %y}
bind $canvas <ButtonRelease-1> {StrokeEnd %W %x %y}

#******** Stat Bar ***********
frame .s -relief groove -bd 2
Stat .s.fil Vectors 30 vectorfile
Stat .s.epc Epoch 10 epoch
Stat .s.nx NX 8 nx
Stat .s.ny NY 8 ny
Stat .s.tend TEND 8 tend
Stat .s.nph NPHASE 8 phase
Stat .s.seed SEED 8 seed
Stat .s.msg MESSAGE 30 message
pack .s.fil .s.epc .s.nx .s.ny .s.tend .s.nph .s.seed .s.msg -side left
pack .s -side bottom -pady 3

set h [expr $height/$nx]
set w [expr $width/$ny]
for {set i 0} {$i < $nx} {incr i } {
    set row [expr $i * $h]
    $canvas create line 0 $row [expr $w*$ny] $row -fill cyan
}
for {set i 0} {$i < $ny} {incr i } {
    set col [expr $i * $w]
    $canvas create line $col 0 $col [expr $h*$nx] -fill cyan
}
