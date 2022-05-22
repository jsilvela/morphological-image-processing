# Morphological image processing

I wrote this code in 2001 for my thesis on image processing, and
from that I got an article published:

*Breadth-First Search and Its Application to Image Processing Problems*,
published on IEEE Transactions on Image Processing, VOL. 10, NO. 8, August 2001.
[link](https://silvela.org/jaime/BFSpaper.pdf)

I have made no functionality change since then, nor have I tried to update
to a more modern version of GTK+.

Tested on a Raspberry Pi running *stretch*, May 2022.

## Installing

You need `libtiff` to be installed.
In a Debian-derived system:

``` sh
apt-get install libtiff libtiff-dev
```

If you want to run the GTK+ UI, you will need `libgtk-dev`
too. This was developed way back when, on GTK+ version 2.

``` sh
sudo apt-get install libgtk2.0-dev
```

Then:

``` sh
make
```

Will build the two executables: `pro-image` and `pro-gtk-image`.
You can skip building the GTK+ bit, by doing:

``` sh
make pro-image
```

## Using

The two executables take in a single TIFF image as input, and after
starting, become a command-line interpreter.

For example, we start processing the image `manos/a30000.tif`.
In the terminal:

``` sh
./pro-gtk-image manos/a30000.tif
```

Now, we can use the terminal as a REPL.
One or more commands may be issued, followed by a newline.
After each, a GTK+ window will display the result of the command.

E.g.

``` sh
threshold 100
```

→ a window opens showing the image after thresholding

``` sh
regions
```

→ a window opens showing the image after cleaning the noisy regions

The commands may also be chained. For example:

``` sh
threshold 100 regions
```

… a window will open after each command.

### Non-graphical executable

The non-graphical executable does not produce a window with the results,
of course. We should write out to a TIFF file to show progress.

``` sh
% ./pro-image manos/a30000.tif
% threshold 100
% regions
% boundary
% skeleton
% write skel.tif
```
