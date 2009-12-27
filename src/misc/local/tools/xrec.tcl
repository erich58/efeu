package require -exact snack 1.6

option add *font {Helvetica 10 bold}

set file [lindex $argv 0]
sound s -debug 0
catch {s read $file}
wm title . "record: $file"

set feedback Waveform
set sampfreq [s cget -frequency]
set sampfmt  [s cget -format]
set chan     [s cget -channels]
set width 600
set time "Length 00:00.00"
set secs [expr int([s length -units seconds])]
set dec [format "%.2d" [expr int(100*([s length -units seconds] - $secs))]]
set time [clock format $secs -format "Length %M:%S.$dec"]
set afterid ""
set max ([s max]
set min [s min])

snack::menuInit
snack::menuPane File
snack::menuCommand File Open... OpenFile
snack::menuCommand File {Save as...} SaveFile
snack::menuCommand File Exit exit

snack::menuPane Samplefreq
foreach f [audio frequencies] {
    snack::menuRadio Samplefreq $f sampfreq $f "s config -frequency $f"
}

snack::menuPane Format 5
foreach f [audio formats] {
    snack::menuRadio Format $f sampfmt $f "s config -format $f"
}

snack::menuPane Channel
snack::menuRadio Channel Mono chan 1 "s config -channels Mono"
snack::menuRadio Channel Stereo chan 2 "s config -channels Stereo"

if {[audio inputs] != ""} {
    snack::menuPane Input
    foreach jack [audio inputs] {
	audio input $jack v(in$jack)
	snack::menuCheck Input $jack v(in$jack)
    }
}

if {[audio outputs] != ""} {
    snack::menuPane Output
    foreach jack [audio outputs] {
	audio output $jack v(out$jack)
	snack::menuCheck Output $jack v(out$jack)
    }
}

snack::menuPane Gain
snack::menuCommand Gain {Gain Control} {snack::gainBox pr}

snack::menuPane Visual
snack::menuRadio Visual Waveform feedback Waveform Draw
snack::menuRadio Visual Spectrogram feedback Spectrogram Draw
snack::menuRadio Visual Nothing feedback Nothing Draw

snack::createIcons
pack [ frame .f] -anchor w
pack [ button .f.bO -image snackOpen -command OpenFile] -side left
pack [ button .f.b3 -image snackSave -command SaveFile] -side left
pack [frame .f.f1 -width 1 -height 20 -highlightth 1] -side left -padx 5
pack [ button .f.bR -bitmap record -fg red -command {s record -frequency $sampfreq -format $sampfmt -channels $chan ; set afterid [after 100 Update]}] -side left
pack [ button .f.bS  -bitmap stop -command {s stop ; after cancel $afterid}] -side left
pack [ button .f.b2 -bitmap play -command {s play}] -side left

pack [ frame .f2] -anchor w
pack [ label .f2.l   -textvar time] -side left
pack [ label .f2.max -textvar max -wi 6]  -side left
pack [ label .f2.min -textvar min -wi 6]  -side left

proc OpenFile {} {
    global s file sampfreq sampfmt time max min

    set file [lindex [file split $file] end]
    set file [snack::getOpenFile -initialfile $file]
    if [string compare $file ""] {
	s read $file
	set sampfreq [s cget -frequency]
	set sampfmt  [s cget -format]
	set secs [expr int([s length -units seconds])]
	set dec [format "%.2d" [expr int(100*([s length -units seconds] - $secs))]]
	set time [clock format $secs -format "Length %M:%S.$dec"]
	set max ([s max]
	set min [s min])
    }
    wm title . "record: $file"
}

proc SaveFile {} {
    global s file

    set file [lindex [file split $file] end]
    set file [snack::getSaveFile -initialfile $file]
    if [string compare $file ""] {
	s write $file
    }
}

proc Draw {} {
    global feedback width

    catch {destroy .c}
    if {$feedback == "Waveform"} {
	pack [ canvas .c -width $width -height 50] -before .f2
	.c create waveform 0 0 -sound s -height 50 -width $width -tags w -pixels 250
    } elseif {$feedback == "Spectrogram"} {
	pack [ canvas .c -width $width -height 50] -before .f2
	.c create spectrogram 0 0 -sound s -height 50 -width $width -tags w -pixels 250
    }
}

proc Update {} {
    global time afterid feedback max min

    set secs [expr int([s length -units seconds])]
    set dec [format "%.2d" [expr int(100*([s length -units seconds] - $secs))]]
    set time [clock format $secs -format "Length %M:%S.$dec"]
    set afterid [after 100 Update]
    set max ([s max]
    set min [s min])
}

Draw