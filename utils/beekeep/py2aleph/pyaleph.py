#!/usr/bin/python
import pyaleph_c
import Tkinter
import math
def fill(image, color):
    """Fill image with a color=(r,b,g)."""
    r,g,b = color
    width = image.width()
    height = image.height()
    hexcode = "#%02x%02x%02x" % (r,g,b)
    horizontal_line = "{" + " ".join([hexcode]*width) + "}"
    image.put(" ".join([horizontal_line]*height))


class simpleapp_tk(Tkinter.Tk):
    def clearscreen(self):
        fill(self.oled,(0,0,0))
    def sin(self, period):
        period *=1.0
        for x in range(4 * self.oled_width):
            y = int(self.oled_height/2 + self.oled_height/4 * math.sin(x/period))
            self.oled.put("#ffffff", (x//4,y))
    def __init__(self,parent):
        Tkinter.Tk.__init__(self,parent,"blah", "foobar")
        self.parent = parent
        self.initialize()

    def initialize(self):
        self.grid()

        button0 = Tkinter.Button(self,text=u"button 0",
                                command=self.OnButton0Click)
        button0.grid(column=1,row=3)

        button1 = Tkinter.Button(self,text=u"button 1",
                                command=self.OnButton1Click)
        button1.grid(column=2,row=3)

        button2 = Tkinter.Button(self,text=u"button 2",
                                command=self.OnButton2Click)
        button2.grid(column=3,row=3)

        button3 = Tkinter.Button(self,text=u"button 3",
                                command=self.OnButton3Click)
        button3.grid(column=4,row=3)

        self.buttonM = Tkinter.Button(self,text=u"button M",
                                command=self.OnButtonMClick)
        self.buttonM.grid(column=6,row=0)

        self.oled_width, self.oled_height = 640, 480
        oled_canvas = Tkinter.Canvas(self, width=self.oled_width, height=self.oled_height, bg="#000000")
        oled_canvas.grid(column=1, row=1, columnspan=4, rowspan=2)
        self.oled = Tkinter.PhotoImage(width=self.oled_width, height=self.oled_height)
        oled_canvas.create_image((self.oled_width/2, self.oled_height/2), image=self.oled, state="normal")

        sensitivity=-100
        slider_length=200
        self.encoder0 = Tkinter.Scale(self, from_=-sensitivity, to=sensitivity,command=self.OnEncoder0Drag)
        self.encoder0.config(length=slider_length)
        self.encoder0.bind("<ButtonRelease-1>", self.OnEncoder0Release)
        self.encoder0.grid(column=5, row=1)

        self.encoder1 = Tkinter.Scale(self, from_=-sensitivity, to=sensitivity,command=self.OnEncoder1Drag)
        self.encoder1.config(length=slider_length)
        self.encoder1.bind("<ButtonRelease-1>", self.OnEncoder1Release)
        self.encoder1.grid(column=6, row=1)

        self.encoder2 = Tkinter.Scale(self, from_=-sensitivity, to=sensitivity,command=self.OnEncoder2Drag)
        self.encoder2.config(length=slider_length)
        self.encoder2.bind("<ButtonRelease-1>", self.OnEncoder2Release)
        self.encoder2.grid(column=5, row=2)

        self.encoder3 = Tkinter.Scale(self, from_=-sensitivity, to=sensitivity,command=self.OnEncoder3Drag)
        self.encoder3.config(length=slider_length)
        self.encoder3.bind("<ButtonRelease-1>", self.OnEncoder3Release)
        self.encoder3.grid(column=6, row=2)

        self.grid_columnconfigure(0,weight=1)
        self.resizable(True,False)
        self.update()
        self.geometry(self.geometry())

    def OnButton0Click(self):
        pyaleph_c.button(0)
        #self.sin(40)

    def OnButton1Click(self):
        pyaleph_c.button(1)

    def OnButton2Click(self):
        pyaleph_c.button(2)

    def OnButton3Click(self):
        pyaleph_c.button(3)

    def OnButtonMClick(self):
        pyaleph_c.button(4)
        self.clearscreen()

    def OnEncoder0Release(self, value):
        value=self.encoder0.get()
        pyaleph_c.knob(0,value)
        self.encoder0.set(0)
    def OnEncoder0Drag(self, value):
        print value

    def OnEncoder1Release(self, value):
        value=self.encoder1.get()
        pyaleph_c.knob(1,value)
        self.encoder1.set(0)
    def OnEncoder1Drag(self, value):
        print value

    def OnEncoder2Release(self, value):
        value=self.encoder2.get()
        pyaleph_c.knob(2,value)
        self.encoder2.set(0)
    def OnEncoder2Drag(self, value):
        print value

    def OnEncoder3Release(self, value):
        value=self.encoder3.get()
        pyaleph_c.knob(3,value)
        self.encoder3.set(0)
    def OnEncoder3Drag(self, value):
        print value

app = simpleapp_tk(None)
def start_gui():
    app.title('Aleph controls sim')
    app.mainloop()
def set_play_led(state):
    print "toggle led to ", state
    if (int(state)):
        app.buttonM.configure(bg="green", activebackground="green")
    else:
        app.buttonM.configure(bg="gray", activebackground="gray")
