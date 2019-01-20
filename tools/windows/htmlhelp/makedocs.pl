#!/usr/local/bin/perl -w

#######################################################################
#
# makedocs.pl - HTML Help doc conversion tool for POV-Ray.
#
# This tool generates the output files needed for the POVWIN HTML Help
# documentation. It has only been tested under Win32.
#
# This was originally an internal tool not meant to be released to the
# public however we now make it available so others can re-generate the
# .CHM file from our doc sources. It is not meant to be pretty (or even
# good perl), but it works, which is the main thing.
#
# See the README that accompanies this file for more information about
# using it.
#
#######################################################################
#
# Comments ? What comments ? This was hacked together as a temporary
# stop-gap tool to meet a need, while we at the same time tried to learn
# how HTML Help works. But it works well enough to have been kept rather
# than replaced with a C/C++ program.
#
#######################################################################
#
# Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
# Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
#
# POV-Ray is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# POV-Ray is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#######################################################################

my %index ;
my %section_titles;
my %section_links;
my %section_definitions;

# sort function
sub by_section { $section_sort{$a} <=> $section_sort{$b} }

# make the HTML Help index file
sub hh_index
{
  %primaryindex = %index ;
  open (OUT, ">output/povray.hhk") || die "Cannot open povray.hhk: $!" ;
  print OUT qq*<html>
<head>
<meta name="GENERATOR" content="POV-Ray Documentation Generation System">
<!-- Sitemap 1.0 -->
</head>
<body>
<object type="text/site properties">
  <param name="Auto Generated" value="Yes">
  <param name="FrameName" value="current">
</object>
<ul>
* ;
  $indent = "" ;
  my $target ;
  $level = 1 ;
  foreach $indexentry (sort keys %primaryindex)
  {
    @lastwords = @words if @words ;
    # we use \x1f as the separator so that it sorts before a space
    @words = split (/\s*\x1f\s*/, $indexentry) ;
    $count = $#words + 1 ;
    if ($indexentry !~ /.*\x1fthe$/i)
    {
      if ($level > 1 && $count > 1 && @lastwords)
      {
        foreach $levelindex (0 .. $#lastwords)
        {
          next if $lastwords[$levelindex] eq $words[$levelindex] ;
          # if we get here, the prefix must have changed
          while ($levelindex + 1 < $level)
          {
            $indent = "  "x--$level ;
            print OUT "$indent</ul>\n" ;
          }
          last ;
        }
      }
      if ($count > $level)
      {
        $lhs = "" ;
        if ($level > 1)
        {
          foreach $temp (0 .. $level - 2)
          {
            $lhs .= "\x1f" if $lhs ;
            $lhs .= $words[$temp] ;
          }
        }
        while ($count > $level)
        {
          $word = $words[$level - 1] ;
          $lhs .= "\x1f" if $lhs ;
          $lhs .= $word ;
          unless (defined $primaryindex{$lhs})
          {
            $primaryindex{$lhs} = "" ;
            $word =~ s/^ *(.*) *$/$1/ ;
            print OUT "$indent<li> <object type=\"text/sitemap\">\n" ;
            print OUT "$indent       <param name=\"Name\" value=\"$word\">\n" ;
            print OUT "$indent       <param name=\"Local\" value=\"JavaScript:htmlhelp.TextPopup(ForInformation,PopupFont,9,9,-1,-1)\">\n" ;
            print OUT "$indent     </object>\n" ;
          }
          print OUT "$indent<ul>\n" ;
          $indent = "  "x$level++ ;
        }
      }
      elsif ($count < $level)
      {
        while ($count < $level)
        {
          $indent = "  "x--$level ;
          print OUT "$indent</ul>\n" ;
        }
      }
      $word = $words[$count - 1] ;
    }
    else
    {
      $count-- ;
      $word = $words[$count - 1] . ", the" ;
    }
    $indent = "  "x$level ;
    print OUT "$indent<li> <object type=\"text/sitemap\">\n" ;
    print OUT "$indent       <param name=\"Name\" value=\"$word\">\n" ;
    if ($primaryindex{$indexentry})
    {
      undef %seentargets ;
      foreach $target (sort split (',', $primaryindex{$indexentry}))
      {
        next if $seentargets{$target} ;
        print OUT "$indent       <param name=\"Local\" value=\"$target\">\n" ;
        $seentargets{$target} = 1 ;
      }
    }
    else
    {
      print OUT "$indent       <param name=\"See Also\" value=\"$word\">\n" ;
    }
    print OUT "$indent     </object>\n" ;
  }
  print OUT qq*</ul>
</body>
</html>
* ;
  close (OUT) ;
}

# make the HTML Help Table of Contents (TOC)
sub hh_toc
{
  open (OUT, ">output/povray.hhc") || die "Cannot open povray.hhc: $!" ;
  print OUT qq*<html>
<head>
<meta name="GENERATOR" content="POV-Ray Documentation Generation System">
<!-- Sitemap 1.0 -->
</head>
<body>
<object type="text/site properties">
  <param name="Auto Generated" value="Yes">
</object>
* ;
  my $lastcount = -1 ;
  $indent = "" ;
  foreach $section (sort by_section keys %section_numbers)
  {
    # print "Section '$section' (val $section_sort{$section})\n";
    $count = 0 ;
    if ($section !~ /^\d+\.0$/)
    {
      foreach (split (/\./, $section)) { $count++ } ;
      $count-- ;
    }
    if ($lastcount < $count)
    {
      while ($lastcount < $count)
      {
        $indent = "  " . "  "x$lastcount++ ;
        print OUT "$indent<ul>\n" ;
      }
    }
    elsif ($lastcount > $count)
    {
      while ($lastcount > $count)
      {
        $indent = "  " . "  "x--$lastcount ;
        print OUT "$indent</ul>\n" ;
      }
    }
    $indent = "  " . "  "x$count ;
    my $title = $section_numbers{$section} ;
    $title =~ s/"/&quot;/g ;
    print OUT qq*$indent<li> <object type="text/sitemap">
$indent       <param name="Name" value="$title">
$indent       <param name="Local" value="$section_links{$section}">
$indent     </object>
* ;
  }
  while ($lastcount > 0)
  {
    $indent = "  " . "  "x--$lastcount ;
    print OUT "$indent</ul>\n" ;
  }
  print OUT qq*</body>
</html>
* ;
  close (OUT) ;
}

sub check_images
{
  print "Fixing up targets\n" ;
  foreach $i (0 .. $filecount)
  {
    $file = $i ;
    $file = "0$file" while length ($file) < 3 ;
    $file = "povdoc_$file.html" ;
    next unless -e $file ;
    open (IN, "<$file") || die "Cannot open $file for read: $!" ;
    undef @lines ;
    print "\r $file" ;
    while ($line = <IN>)
    {
      chomp $line ;
      push (@lines, $line) ;
    }
    close (IN) ;
    foreach $line (@lines)
    {
      $imagetext = $line ;
      while ($imagetext =~ s/src=\"([^"]+\.(gif)|(jpg)|(png))\"//ig)
      {
        $image = $1 ;
        print "\nMissing image file $image\n" unless -e "output/$image" ;
      }
    }
  }
  print "\n" ;
}

##############################################################################
#
# Mainline code starts here
#
##############################################################################

# remove old output files
unlink glob "output/*.html";
# unlink glob "output/*.chm";
unlink glob "output/*.chw";
unlink glob "output/*.hhc";
unlink glob "output/*.hhk";

open (IN, "<filelist.txt") || die "Cannot open filelist.txt: $!" ;
my @files = <IN> ;
close (IN) ;
foreach (@files) { chomp ($_) } ;

# suck in each source file.
$filecount = 0 ;
foreach $file (@files)
{
  next if $file eq "" ;
  my $lineno = 0 ;
  my $result = "";
  my $toc = $file =~ /^[a-z]\d_0\.html$/;

  open (IN, "<input/$file") || die "Cannot open $file: $!" ;
  print "Processing $file\n";
  
  while ($line = <IN>)
  {
    $lineno++ ;
    last if $line =~ /<\/head>/;
    $result .= $line;
  }

  $result .= "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=9\"/>\n";
  $result .= "<object classid=\"clsid:adb880a6-d8ff-11cf-9377-00aa003b7a11\" id=\"htmlhelp\" type=\"application/x-oleobject\"></object>\n";
  $result .= "<script language=\"javascript\" src=\"povray.js\"></script>\n";
  $result .= $line;

  while ($line = <IN>)
  {
    $lineno++ ;
    last if $line =~ /<!-- NavPanel Begin -->/;
    $result .= $line;
  }
  while ($line = <IN>)
  {
    $lineno++ ;
    if (!$toc)
    {
      if ($line =~ /<a title="([^"]+)" href="([^"]+)">([^<]+)<\/a>/)
      {
        $secno = $1;
        $link = "$2";
        $title = $3;
        $link = "$file$link" if $2 =~ /^#/;
        
        next if $secno eq "You're already here!";
        $secno = "1.0" if $secno eq "Windows Table of Contents";
        $secno = "2.0" if $secno eq "Tutorial Table of Contents";
        $secno = "3.0" if $secno eq "Reference Table of Contents";
        
        $section_numbers{$secno} = $title;
        $section_titles{$title} = $secno;
        $section_links{$secno} = $link;
        $section_definitions{$title} = "$file:$lineno";

        $sectionval = 0;
        $mult = 10000000;
        my @parts = split(/\./, $secno);
        foreach $part (@parts)
        {
          $sectionval += $part * $mult;
          $mult /= 100;
        }
        $section_sort{$secno} = $sectionval;
      }
    }
    last if $line =~ /<!-- NavPanel End -->/;
  }
  
  $link = "$file";
  while ($line = <IN>)
  {
    $lineno++ ;
    chomp $line ;
    
    if ($line =~ /<div\s+class="content-level-h(\d)"\s+contains="([^"]*)"\s+id="([^"]*)">/i)
    {
      die "$file:$lineno: $line" unless $1 && $2 && $3 ;
      $link = "$file#$3";
      $secname = $2 ;
      $secname =~ s/&#39;/'/g ;
      
      $indexentry = lc($secname) ;
      $indexentry =~ s/^\s+// ;
      $indexentry =~ s/\s+$// ;
      if ($indexentry =~ /^object (.*)$/i)
      {
        $indexentry = $1 ;
        $index{"objects\x1f$indexentry"} .= "," if defined $index{"objects\x1f$indexentry"};
        $index{"objects\x1f$indexentry"} .= $link;
      }
      if ($indexentry !~ /^(how|what|why|can|when|where|any|is|will|does|I'm|about) /i)
      {
        if ($indexentry !~ /^the / && $indexentry !~ /\w+\W+\w+\W+/)
        {
          if ($indexentry !~ /\w+\W+\w+\W+\w+\W+\w+\W+\w+\W+.*/)
          {
            $temp = $indexentry ;
            $temp =~ s/^(the) (.*)$/$2/i ;
            $temp =~ s/^(text) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(vector) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(csg) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(file) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(float) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(language) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(render) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(string) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(working with) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(array) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(atmospheric) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(camera) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(clock) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(color) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(isosurface) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(light) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(new) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(output) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(pov-ray) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(radiosity) (.*)$/$1\x1f$2/i ;
            $temp =~ s/^(using) (.*)$/$1\x1f$2/i ;
            $index{$temp} .= "," if defined $index{$temp} ;
            $index{$temp} .= $link;
            # print "$temp $link\n";
          }
        }
      }
    }
    elsif ($line =~ /content-level-h(\d)/i)
    {
      die "$file:$lineno: Stray content-level in '$line'";
    }
    while ($line =~ s/\{\{#indexentry:([^}]+)\}\}//i)
    {
      # separate out multiple entries separated by '|'
      $text = $1;
      foreach $entry (split(/\|/, $text))
      {
        # now get the primary entry (the first) and its sub-entries
        $entry =~ s/&#39;/'/g ;
        @parts = split(/ *, */, lc($entry));
        print "$file:$lineno: $entry\n" if $#parts > 2;
        # die $entry if $#parts > 1;
        $key = $parts[0];
        $key .= "\x1f" . $parts[1] if $#parts > 0;
        $key .= "\x1f" . $parts[2] if $#parts > 1;
        $index{$key} .= "," if defined $index{$key} ;
        $index{$key} .= $link ;
        # print "$key $link\n";
      }
    }
    die "$file:$lineno - Orphaned indexentry '$line'" if $line =~ /indexentry/i ;
    $newline = "" ;
    while ($line =~ /(.*?)(<a ([^>]+)>)(.*)/i)
    {
      my $left = $1 ;
      my $attributes = $3 ;
      my $right = $4 ;
      my $external = 0;
      
      $line = $right ;
      if ($attributes =~ /href\s*=\s*["']([^"']+)["']/i)
      {
        # we have an <a href="...">, now see if it's an external link
        $destination = $1 ;
        if ($destination =~ /^[a-z]*:\/\//i)
        {
          if ($attributes !~ /target\s*=\s*"([^"]+)"/i)
          {
            # there doesn't appear to be a target window, issue a warning
            # print "Warning: link '$destination' is missing target window at $file:$lineno\n" ;
            $attributes .= " target=\"new\"";
          }
	  $attributes .= " class=\"ExternalLink\"";
          $external = 1;
        }
      }
      $newline .= "$left<a $attributes>" ;
    
      # if it's an external link prepend this character to the link description
      $newline .= "»" if $external;
    }
    
    $line = "$newline$line" ;
    $result .= "$line\n";
  }
  close (IN);
  open(OUT, ">output/$file") || die "Could not open $file for output: $!";
  print OUT $result;
  close(OUT);  
}

open (OUT, ">debug.txt") || die "Cannot open debug.txt: $!" ;
print OUT "\nPrimary targets - \n" ;
foreach $primary (sort keys %targets)
{
  print OUT "  $primary -> $targets{$primary}\n" ;
}

print OUT "\nPrimary index entries - \n" ;
foreach $primary (sort keys %index)
{
  print OUT "  $primary -> $index{$primary}\n" ;
}
close (OUT) ;

# now eliminate duplicate destinations for each target
foreach $target (sort keys %targets)
{
  my %dest ;
  my $result ;
  foreach (split (/,/, $targets{$target})) { $dest{$_} = 1 ; } ;
  foreach (sort keys %dest)
  {
    $result .= "," if $result ;
    $result .= $_ ;
  }
  $targets{$target} = $result ;
}

# now start writing the output files
print "Postprocessing\n" ;
hh_toc();
hh_index();

