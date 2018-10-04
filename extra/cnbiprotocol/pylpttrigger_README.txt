Instructions to use the LPT triggers from within a python program:

1) Copy/paste the attached shared library pylpttrigger.so into the same
path as your main *.py file running the feedback.
2) In your main *.py file import the module of the shared lib:
    import pylpttrigger
3) Somewhere in the initialization of your program, open the LPT port:
    lpt = pylpttrigger
    lpt.open(0,100,0)
    Note: The 3 arguments here are:
    a) Baselevel, an integer specifying the resting level of the pins,
typically 0.
    b) Delay, an integer specifying the duration of the pulses in
milliseconds, Unfortunately, not configurable, as    soon as you set it
up, all the pulses will have the same duration, however this is no
problem if this is used for signaling events as in your case, since you
are interested on the rising edge rather than the duration of the pulse.
    c) The parallel port number (in linux, /dev/parportX), typically 0.
Here, you have to be careful if you are using the PCMCIA2lpt adapters in
the laptops: if you insert the adapter after starting the parport is
incremented, and the same argument will not work. Rule of thumb is to
always have the adapter inserted before you start the machine, then you
can hardcode this last argument to 0.
4) To send a trigger whenever needed inside your program:
    lpt.signal(X), where X the integer you want to send, aka the
amplitude of the pulse
5) At the end of your program, close the port:
    lpt.close()

PS: No need to rename lpt = pylpttrigger in the beginning, but if you do
not do that, you have to replace lpt by pylpttrigger thereafter, which is
more cumbersome (to write) :)
