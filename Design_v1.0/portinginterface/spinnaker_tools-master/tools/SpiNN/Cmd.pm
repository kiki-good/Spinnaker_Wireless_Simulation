
##------------------------------------------------------------------------------
##
## SpiNN/Cmd.pm	    A Perl package for communicating with SpiNNaker systems
##
## Copyright (C)    The University of Manchester - 2012-2013
##
## Author           Steve Temple, APT Group, School of Computer Science
## Email            temples@cs.man.ac.uk
##
## Status	    Experimental software - liable to change at any time !!
##
##------------------------------------------------------------------------------


package SpiNN::Cmd;

use strict;
use warnings;

use SpiNN::SCP;

our @ISA = qw/SpiNN::SCP/;


#------------------------------------------------------------------------------


my $CMD_VER = 0;
my $CMD_RUN = 1;
my $CMD_READ = 2;
my $CMD_WRITE = 3;
my $CMD_APLX = 4;
my $CMD_FILL = 5;

my $CMD_REMAP = 16;
my $CMD_LINK_READ = 17;
my $CMD_LINK_WRITE = 18;
my $CMD_AR = 19;

my $CMD_NNP = 20;
my $CMD_P2PC = 21;
my $CMD_SIG = 22;
my $CMD_FFD = 23;

my $CMD_AS = 24;
my $CMD_LED = 25;
my $CMD_IPTAG = 26;
my $CMD_SROM = 27;

# BMP

my $CMD_FLASH_COPY = 49;
my $CMD_FLASH_ERASE = 50;
my $CMD_FLASH_WRITE = 51;
my $CMD_RESET = 55;
my $CMD_POWER = 57;

my $TYPE_BYTE = 0;
my $TYPE_HALF = 1;
my $TYPE_WORD = 2;

my $NN_CMD_FFS = 6;
my $NN_CMD_FFE = 15;

my %mem_type = (byte => 0, half => 1, word => 2);


my $IPTAG_NEW  = 0;
my $IPTAG_SET  = 1;
my $IPTAG_GET  = 2;
my $IPTAG_CLR  = 3;
my $IPTAG_TTO  = 4;


#------------------------------------------------------------------------------


sub new
{
    my $parent = shift;
    my $self = $parent->SUPER::new (@_);
#    bless $self, $parent;
    return $self;
}


#------------------------------------------------------------------------------


sub next_id
{
    my $self = shift;

    my $id = $self->{nn_id} + 1;
    $id = 1 if $id > 127;
    $self->{nn_id} = $id;

    return 2 * $id;
}


#------------------------------------------------------------------------------


sub ver
{
    my ($self, %opts) = @_;

    my $addr = $opts{addr};
    my $raw = $opts{raw};

    my $data = $self->scp_cmd ($CMD_VER, addr => $addr);

    my ($vc, $pc, $cy, $cx, $size, $ver_num, $time, $ver_str) = 
	unpack "C4 v2 V! a*", $data;

    chop $ver_str; # remove trailing null

    return [$vc, $pc, $cy, $cx, $size, $ver_num, $time, $ver_str] if $raw;

    my ($name, $hw) = split /\//, $ver_str;

    return sprintf "$name %0.2f at $hw:$cx,$cy,$vc (built %s) \[$pc]",
      $ver_num / 100, scalar localtime $time;
}


#------------------------------------------------------------------------------


sub read
{
    my ($self, $base, $length, %opts) = @_;

    my $addr = $opts{addr};
    my $type = $opts{type} || "byte";
    my $format = $opts{format};
    my $unpack = $opts{unpack};

    my $data = "";
    my $buf_size = $self->{buf_size};

    die "bad memory type" unless exists $mem_type{$type};
    die "misaligned address\n" if $type eq "word" && ($base & 3) != 0;
    die "misaligned address\n" if $type eq "half" && ($base & 1) != 0;

    $type = $mem_type{$type};

    while ($length > 0)
    {
	my $l = ($length > $buf_size) ? $buf_size : $length;
	$data .= $self->scp_cmd ($CMD_READ,
				 arg1 => $base,
				 arg2 => $l,
				 arg3 => $type,
				 addr => $addr);
	$length -= $l;
	$base += $l;
    }

    $data = [unpack $unpack, $data] if $unpack;
    $data = sprintf $format, @$data if $format;

    return $data;
}


#------------------------------------------------------------------------------


sub link_read
{
    my ($self, $link, $base, $length, %opts) = @_;

    my $addr = $opts{addr};
    my $format = $opts{format};
    my $unpack = $opts{unpack};

    my $data = "";
    my $buf_size = $self->{buf_size};

    die "bad link ($link)\n" unless $link >= 0 && $link <= 5;

    my $rem = $length % 4;
    $length += 4 - $rem if $rem != 0;

    while ($length > 0)
    {
	my $l = ($length > $buf_size) ? $buf_size : $length;
	$data .= $self->scp_cmd ($CMD_LINK_READ,
				 arg1 => $base,
				 arg2 => $l,
				 arg3 => $link,
				 addr => $addr);
	
	$length -= $l;
	$base += $l;
    }

    $data = [unpack $unpack, $data] if $unpack;
    $data = sprintf $format, @$data if $format;

    return $data;
}


#------------------------------------------------------------------------------


sub write_file
{
    my ($self, $base, $file, %opts) = @_;

    my $addr = $opts{addr};
    my $type = $opts{type} || "byte";

    my $size = 4096;

    open my $fh, "<", $file or die "Can't open file\n";

    eval
    {
	while (1)
	{
	    my $len = sysread $fh, my ($buf), $size;
	    last if $len <= 0;
	    $self->write ($base, $buf, addr => $addr);
	    $base += $size;
	}
    };

    close $fh;

    die $@ if $@;
}


sub write
{
    my ($self, $base, $data, %opts) = @_;

    my $addr = $opts{addr};
    my $type = $opts{type} || "byte";

    die "bad memory type" unless exists $mem_type{$type};
    die "misaligned address\n" if $type eq "word" && ($base & 3) != 0;
    die "misaligned address\n" if $type eq "half" && ($base & 1) != 0;

    $type = $mem_type{$type};

    my $offset = 0;
    my $buf_size = $self->{buf_size};

    while (1)
    {
	my $buf = substr $data, $offset, $buf_size;

	last unless defined $buf;

	my $length = length $buf;

	last if $length == 0;

	$self->scp_cmd ($CMD_WRITE,
			arg1 => $base,
			arg2 => $length,
			arg3 => $type,
			data => $buf,
			addr => $addr);

	last if $length < $buf_size;

	$base += $buf_size;
	$offset += $buf_size;
    }
}


#------------------------------------------------------------------------------


sub srom_read
{
    my ($self, $base, $length, %opts) = @_;

    my $addr = $opts{addr};
    my $format = $opts{format};
    my $unpack = $opts{unpack};
    my $addr_size = $opts{addr_size} || 24;

    my $data = "";
    my $buf_size = $self->{buf_size};

    while ($length > 0)
    {
	my $l = ($length > $buf_size) ? $buf_size : $length;

	$data .= $self->scp_cmd ($CMD_SROM,
				 arg1 => ($l << 16) + $addr_size + 8, # len=$l
				 arg2 => 0x03000000 + ($base << (24 - $addr_size)), # cmd=3, addr=base
				 addr => $addr);

	$length -= $l;
	$base += $l;
    }

    $data = [unpack $unpack, $data] if $unpack;
    $data = sprintf $format, @$data if $format;

    return $data;
}


#------------------------------------------------------------------------------


sub srom_write
{
    my ($self, $base, $data, %opts) = @_;

    my $addr = $opts{addr};
    my $page_size = $opts{page_size} || 256;
    my $addr_size = $opts{addr_size} || 24;

    my $offset = 0;

    while (1)
    {
	my $buf = substr $data, $offset, $page_size;

	last unless defined $buf;

	my $len = length $buf;

	last if $len == 0;

	$self->scp_cmd ($CMD_SROM,
			arg1 => ($len << 16) + 0x1c0 + $addr_size + 8, # wr, WREN, wait
			arg2 => 0x02000000 + ($base << (24 - $addr_size)), # cmd=2, addr=base
			data => $buf,
			addr => $addr);

	last if $len < $page_size;

	$base += $page_size;
	$offset += $page_size;
    }
}


#------------------------------------------------------------------------------


sub srom_erase
{
    my ($self, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_SROM,
		    arg1 => 0xc8, # len=0, bits=8, sent WREN, wait
		    arg2 => 0xc7000000, # cmd=c7, addr=0
		    addr => $addr);
}


#------------------------------------------------------------------------------


sub led
{
    my ($self, $leds, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_LED,
		    arg1 => $leds,
		    addr => $addr);
}


#------------------------------------------------------------------------------


sub link_write
{
    my ($self, $link, $base, $data, %opts) = @_;

    my $addr = $opts{addr};

    die "bad link ($link)\n" unless $link >= 0 && $link <= 5;

    my $offset = 0;
    my $buf_size = $self->{buf_size};

    my $rem = length ($data) % 4;
    $data .= chr (0) x (4 - $rem) if $rem != 0;

    while (1)
    {
	my $buf = substr $data, $offset, $buf_size;

	last unless defined $buf;

	my $length = length $buf;

	last if $length == 0;

	$self->scp_cmd ($CMD_LINK_WRITE,
			arg1 => $base,
			arg2 => $length,
			arg3 => $link,
			data => $buf,
			addr => $addr);

	last if $length < $buf_size;

	$base += $buf_size;
	$offset += $buf_size;
    }
}


#------------------------------------------------------------------------------


sub fill
{
    my ($self, $base, $data, $length, %opts) = @_;

    my $addr = $opts{addr};

    die "address not a multiple of 4\n" if $base & 3;
    die "length not a multiple of 4\n" if $length & 3;
    die "length less than 0\n" if $length <= 0;

    $self->scp_cmd ($CMD_FILL,
		    arg1 => $base,
		    arg2 => $data,
		    arg3 => $length,
		    addr => $addr);
}


#------------------------------------------------------------------------------


sub flood_fill
{
    my ($self, $buf, $region, $mask, $app_id, $app_flags, %opts) = @_;

    my $addr = $opts{addr};
    my $base = $opts{base} || 0x67800000;

    my $debug = $self->{debug};

    my $size = length $buf;
    my $blocks = int ($size / 256);
    $blocks++ if $size % 256 != 0;

    print "# FF $size bytes, $blocks blocks\n" if $debug;

    my ($sfwd, $srty) = (0x3f, 0x18);	# Forward, retry parameters
    my $id = $self->next_id ();		# ID for FF packets
    my $fr = ($sfwd << 8) + $srty;	# Pack up fwd, rty
    my $sfr = (1 << 31) + $fr;		# Bit 31 says allocate ID on Spin

    # Send FFS packet

    my $key = ($NN_CMD_FFS << 24) + ($id << 16) + ($blocks << 8) + 0; # const
    my $data = $region;

    printf "FFS %08x %08x %08x\n", $key, $data, $sfr if $debug;

    $self->nnp ($key, $data, $sfr, addr => $addr);

    # Send FFD data blocks

    my $ptr = 0;

    for (my $block = 0; $block < $blocks; $block++)
    {
	my $data = substr $buf, $ptr, 256;
	my $len = length $data;

	my $size = ($len / 4) - 1;

	my $arg1 = ($sfwd << 24) + ($srty << 16) + (0 << 8) + $id;
	my $arg2 = (0 << 24) + ($block << 16) + ($size << 8) + 0;

	printf "FFD %08x %08x %08x\n", $arg1, $arg2, $base if $debug;

	$self->scp_cmd ($CMD_FFD,
			arg1 => $arg1,
			arg2 => $arg2,
			arg3 => $base,
			data => $data,
			addr => $addr);

	$base += $len;
	$ptr += $len;
    }

    # Send FFE packet

    $key = ($NN_CMD_FFE << 24) + (0 << 16) + (0 << 8) + $id; # const
    $data = ($app_id << 24) + ($app_flags << 18) + ($mask & 0x3ffff);

    printf "FFE %08x %08x %08x\n", $key, $data, $fr if $debug;

    $self->nnp ($key, $data, $fr, addr => $addr);
}


sub flood_boot
{
    my ($self, $buf, %opts) = @_;

    my $debug = $self->{debug};

    my $size = length $buf;
    my $blocks = int ($size / 256);
    $blocks++ if $size % 256 != 0;

    print "FF Boot - $size bytes, $blocks blocks\n" if $debug;

    my ($sfwd, $srty) = (0x3f, 0x18);
    my ($nnp, $ffd) = ($CMD_NNP - 8, $CMD_FFD - 8);
    my ($base, $mask) = (0xf5000000, 1);
    my $fr = ($sfwd << 8) + $srty;

    # First send a SIG0 to set global fwd/rty

##    my $sig0 = 0x003f0000 + $self->next_id ();

##    printf "FF SIG0 %08x %08x %08x\n", $sig0, 0x3f00, 0x3f00 if $debug;

##    my $e = $self->scp_cmd ($nnp, $sig0, 0x3f00, 0x3f00, addr => "root");
##    return $self->{error} = $e if $e;

    my $id = $self->next_id ();

    # Send FFS packet (buffer address in data field)

    my $key = ($NN_CMD_FFS << 24) + (0 << 16) + ($blocks << 8) + $id; # const

    printf "FFS %08x %08x %08x\n", $key, $base, $fr if $debug;

    $self->scp_cmd ($nnp,
		    arg1 => $key,
		    arg2 => $base,
		    arg3 => $fr,
		    addr => []);

    # Send FFD data blocks

    my $ptr = 0;

    for (my $block = 0; $block < $blocks; $block++)
    {
	my $data = substr $buf, $ptr, 256;
	my $len = length $data;

	my $size = ($len / 4) - 1;

	my $arg1 = ($sfwd << 24) + ($srty << 16) + (0 << 8) + $id;
	my $arg2 = (0 << 24) + ($block << 16) + ($size << 8) + 0;

	printf "FFD %08x %08x %08x\n", $arg1, $arg2, $base if $debug;

	$self->scp_cmd ($ffd,
			arg1 => $arg1,
			arg2 => $arg2,
			arg3 => $base,
			data => $data,
			addr => []);

	$base += $len;
	$ptr += $len;

    }

    # Send FFE packet (mask (= 1) in data field)

    $key = ($NN_CMD_FFE << 24) + (0 << 16) + (0 << 8) + $id; # const

    printf "FFE %08x %08x %08x\n", $key, $mask, $fr if $debug;

    $self->scp_cmd ($nnp,
		    arg1 => $key,
		    arg2 => $mask,
		    arg3 => $fr,
		    addr => []);
}


#------------------------------------------------------------------------------


sub p2pc
{
    my ($self, $x, $y, %opts) = @_;

    my $id = $self->next_id ();

    my $arg1 = (0x00 << 24) + (0x3e << 16) + (0x00 << 8) + $id;
    my $arg2 = ($x << 24) + ($y << 16) + (0x00 << 8) + 0x00;
    my $arg3 = (0x00 << 24) + (0x00 << 16) + (0x3f << 8) + 0xf8;

    $self->scp_cmd ($CMD_P2PC,
		    arg1 => $arg1,
		    arg2 => $arg2,
		    arg3 => $arg3,
		    addr => []);
}


sub as
{
    my ($self, $base, $mask, $app_id, $app_flags, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_AS,
		    arg1 => $base,
		    arg2 => ($app_id << 24) + ($app_flags << 18) + $mask,
		    addr => $addr);
}


sub ar
{
    my ($self, $mask, $app_id, $app_flags, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_AR,
		    arg1 => ($app_id << 24) + ($app_flags << 18) + $mask,
		    addr => $addr);
}


sub signal
{
    my ($self, $type, $data, $mask, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_SIG,
		    arg1 => $type,
		    arg2 => $data,
		    arg3 => $mask,
		    addr => $addr);
}


sub nnp
{
    my ($self, $arg1, $arg2, $arg3, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_NNP,
		    arg1 => $arg1,
		    arg2 => $arg2,
		    arg3 => $arg3,
		    addr => $addr);
}


#------------------------------------------------------------------------------


sub iptag_set
{
    my ($self, $tag, $host, $port, %opts) = @_;

    my $addr = $opts{addr};

    $host = $self->{host_ip} if $host eq ".";

    my $ip = gethostbyname ($host);

    die "unknown host \"$host\"\n" unless defined $ip;

    ($ip) = unpack "V", $ip;

    $self->scp_cmd ($CMD_IPTAG,
		    arg1 => ($IPTAG_SET << 16) + $tag,
		    arg2 => $port,
		    arg3 => $ip,
		    addr => $addr);
}


sub iptag_clear
{
    my ($self, $tag, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_IPTAG,
		    arg1 => ($IPTAG_CLR << 16) + $tag,
		    addr => $addr);
}


sub iptag_get
{
    my ($self, $tag, $count, %opts) = @_;

    my $addr = $opts{addr};

    $count ||= 1;

    $self->scp_cmd ($CMD_IPTAG,
		    arg1 => ($IPTAG_GET << 16) + $tag,
		    arg2 => $count,
		    addr => $addr);
}


sub iptag_tto
{
    my ($self, $tto, %opts) = @_;

    my $addr = $opts{addr};

    $self->scp_cmd ($CMD_IPTAG,
		    arg1 => ($IPTAG_TTO << 16),
		    arg2 => $tto,
		    addr => $addr);
}


#------------------------------------------------------------------------------


sub flash_write
{
    my ($self, $addr, $data, %opts) = @_;

    my $update = $opts{update} || 0;
    my $size = length $data;
    my ($base, $offset) = ($addr, 0);

    die "no data\n" unless $size;
    die "not on 4k byte boundary\n" unless ($addr & 4095) == 0;
    die "crosses flash 4k/32k boundary\n"
	if $addr < 65536 && ($addr + $size) > 65536;
    die "address not in flash\n" if ($addr + $size) > 524288;

    # Erase and get address of flash buffer

    my $t = $self->scp_cmd ($CMD_FLASH_ERASE,
			    arg1 => $addr,
			    arg2 => $addr + $size,
			    unpack => "V");

    # Write as many times as needed

    while (1)
    {
	my $buf = substr $data, $offset, 4096;
	last unless defined $buf;

	my $length = length $buf;
	last if $length == 0;

	$self->write ($t->[0], $buf);

	$self->scp_cmd ($CMD_FLASH_WRITE,
		       arg1 => $base,
		       arg2 => 4096);

	last if $length < 4096;
	$base += 4096;
	$offset += 4096;
    }

    # Update if requested

    if ($update)
    {
	$self->scp_cmd ($CMD_FLASH_COPY,
		       arg1 => 0x10000,
		       arg2 => $addr,
		       arg3 => $size);
    }
}


#------------------------------------------------------------------------------

# Need testing...

sub reset
{
    my ($self, $mask) = @_;

    $self->scp_cmd ($CMD_RESET,
		    arg1 => 2,
		    arg2 => $mask);
}


sub power
{
    my ($self, $on, $mask) = @_;

    $self->scp_cmd ($CMD_POWER,
		    arg1 => $on,
		    arg2 => $mask);
}


#------------------------------------------------------------------------------

1;
