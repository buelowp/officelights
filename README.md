# Control Hue Lights with a Raspberry PI
This C++ Qt5 app allows me to control some Hue light bulbs in my office. 
I have 4, and like them to turn on and off on a schedule. Plus, being able
to set a specific color is kinda fun.

It also has a USB button service which allows me to manage inputs. This
is done by the X-Keys USB buttons.

Finally, I have some WS2801 strips which do some great work for LED lighting,
which again, is fun. This is handled by FastLED.

## Dependencies
###FastLED for Linux

The repository listed is great, but may not be the most up to date. Use git,
checkout the linux branch, and run make. I suggest modifying the makefile
to point to /usr instead of /usr/local, but it's a minor point.

```bash
git clone https://github.com/mikeburg/FastLED.git
git co linuxport
make -f Makefile.linux
make -f Makefile.linux install
```

### C++ Qt Hue Library

The shine library works well, but I've modified it a bit from the original.
I use mine instead to link here, and reference my forked version. The original
is in good shape, and could be used as well, but hasn't been updated in a while
and has little information on how to use it.

The shine library has a hard Qt4 dependency, but you can just delete all paths
except libhue. This isn't exactly right though, so documentation updates are
coming.

```bash
git clone git@github.com:buelowp/shine.git
mkdir build
cd build
cmake ..
make
sudo make install
```

### XKeys and the QT wrapper for them

The Pie engineering group did the Xkeys, which are nice USB enabled buttons with
LED accents that are controllable. They work well for button control of a project.
They are used here to control on/off, and color and LED program choices.

```bash
git clone https://github.com/piengineering/xkeys.git
make
sudo make install
```

And the Qt wrapper.

```bash
git clone git@github.com:buelowp/qxkeys8.git
make
sudo make install
```


