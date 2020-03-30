sub flush_buffer {
    my $key = $_[0];
    my $buffer = $_[1];
    my $pcs = $_[2];
    my $binfile = $_[3];
    my $last_time = $_[4];
    my $inline = $_[5];
    my $use_pc_as_label = $_[6];
    my $gvsoc = $_[7];

    my $linenum = $buffer =~ tr/\n//;

    if ($linenum < 2) { return; }

    my $funcnames = `riscv32-unknown-elf-addr2line -e $binfile -f -a -i $pcs`;

    $funcnames = "$funcnames\n0x0"; # to let it process the last address in the below loop

    #remove first line
    $funcnames =~ s/^[^\n]*\n//s;

    my @a2l_first_last_lines = {};
    $a2l_first_last_lines[0] = "";
    $a2l_first_last_lines[1] = "";  
    
    my @a2i_first_last_lines = {};
    $a2i_first_last_lines[0] = "";
    $a2i_first_last_lines[1] = "";  


    my $a2l_line_index = 1;
    if (!$inline) { $a2l_line_index = 0;}

    for (split /\n/, $funcnames) {
        my $a2l_line = $_;
        #print "$_\n";
        if ($a2l_line =~ /^(0x[0-9a-f]+)(.*)/ and $buffer =~ /.*\n.*\n.*/) {
            #print "ADDR: $1 $2\n";
	    # https://www.debuggex.com/#cheatsheet
	    # new regex unbreak
	    # my $regex = qr/^\s*([0-9]+)[munpf]?s?\s+([0-9]+)\s+([0-9a-f]+)\s+[0-9a-f]+\s+([^ ]+)\s+(.+?(?=  )).*/;
	    # old regex
            my $regex = qr/^\s*([0-9]+)[munpfs]?s?\s+([0-9]+)\s+([0-9a-f]+)\s+[0-9a-f]+\s+([^ ]+)\s+(.+?(?=  ))(.*)\n\s*[0-9]+[munpfs]?s?\s+([0-9]+).*/;
            if($gvsoc == 1) {
                $regex = qr/^\s*([0-9]+):\s+([0-9]+):\s+\[\e\[[0-9;]*m[^ ]*\s*\e\[[0-9;]*m\]\s+[^ ]+\s+M\s+([0-9a-f]+)\s+([^ ]+)\s+(.+?(?=  ))(.*)\n\s*[0-9]+:\s+([0-9]+):.*/;
            }
            my ($time, $cycles, $pc, $instr, $args, $rest, $next_cycles) = $buffer =~ $regex;
    
            #remove current line from the buffer
            $buffer =~ s/^[^\n]*\n//s;

            my $funcname = $a2l_first_last_lines[$a2l_line_index];
            my $coords = $a2i_first_last_lines[$a2l_line_index];
            my $duration = ($next_cycles - $cycles);
            my $start_time = $cycles;
            
            my $label = $instr;
            if ($use_pc_as_label) { $label = "$pc - $instr"; }

            print "{\"name\": \"$label\", \"cat\": \"$instr\", \"ph\": \"X\", \"ts\": $start_time, \"dur\": $duration, \"pid\": \"$key\", \"tid\": \"$funcname\", \"args\":{\"pc\": \"$pc\", \"instr\": \"$instr $args $rest\", \"time\": \"$cycles\", \"Origin\": \"$coords\"}},\n";

        
            $a2l_first_last_lines[0] = "";
            $a2l_first_last_lines[1] = "";
            $a2i_first_last_lines[0] = "";
            $a2i_first_last_lines[1] = "";
            $last_time = $cycles;

        } elsif ($a2l_line =~ /^[^\/].*/) {
            if ($a2l_first_last_lines[0] eq "") { $a2l_first_last_lines[0] = $a2l_line; }
            $a2l_first_last_lines[1] = $a2l_line;
        } else {
            if ($a2i_first_last_lines[0] eq "") { $a2i_first_last_lines[0] = $a2l_line; }
            $a2i_first_last_lines[1] = $a2l_line;
        }
        
    }
    return $last_time;
}

sub convert_file {
    my $file = $_[0];
    my $binfile = $_[1];
    my $inline = $_[2];
    my $use_pc_as_label = $_[3];
    my $gvsoc = $_[4];


    open my $info, $file or die "Could not open $file: $!";
    my $last_time = 0;
    my $buffer = "";
    my $pcs="";
    my $count = 0;

    while(my $line = <$info>) {
        my $regex = qr/^\s*([0-9]+)[munpf]?s?\s+([0-9]+)\s+([0-9a-f]+)\s+[0-9a-f]+\s+([^ ]+)\s+(.+?(?=  )).*/;
        if($gvsoc == 1) {
            $regex = qr/^\s*([0-9]+):\s+([0-9]+):\s+\[\e\[[0-9;]*m[^ ]*\s*\e\[[0-9;]*m\]\s+[^ ]+\s+M\s+([0-9a-f]+)\s+([^ ]+)\s+(.+?(?=  ))(.*)/;
        }
        if  ($line =~ $regex) {
            $buffer = "$buffer$line"; 
            $pcs = "$pcs $3";
            $count++;

            if ($count==1000){
                $last_time = flush_buffer($file, $buffer, $pcs, $binfile, $last_time, $inline, $use_pc_as_label, $gvsoc);
                $buffer="$line";
                $pcs="$3";
                $count=0;
            }
        } else {
	    print "died on regex\n";
	    print $line;
	    exit;
	}
    }

    #in case we didn't reach the flushing threshold
    $last_time = flush_buffer($file, $buffer, $pcs, $binfile, $last_time, $inline, $use_pc_as_label, $gvsoc);
    close $info;
    return $last_time;
}

if ($#ARGV < 1) {
    print "Usage: $0 [-i] <bin_file> <trace_file_1> .. <trace_file_n>\n";
    exit;
}

my $arg_index = 0;

my $inline = 0;
my $use_pc_as_label = 0;
my $gvsoc = 0;
if ($ARGV[$arg_index] eq "-i") {
    $inline = 1;
    $arg_index++;
    shift;
}

if ($ARGV[$arg_index] eq "-p") {
    $use_pc_as_label=1;
    $arg_index++;
    shift;
}

if ($ARGV[$arg_index] eq "-g") {
    $gvsoc=1;
    $arg_index++;
    shift;
}

my $binfile=shift; #$ARGV[$arg_index++];


print "{\"traceEvents\": [\n";

my $last_time=0;
foreach my $file (@ARGV) {
    $last_time = convert_file($file, $binfile, $inline, $use_pc_as_label, $gvsoc);
}

print "{}]}\n";


