import tkinter as tk
import serial
import struct
import time
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
matplotlib.use("TkAgg")

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure

root = tk.Tk()
root.geometry("400x400")

class GUI:
    def __init__(self):
        self.ser = None
        self.mouse_pos = [0, 0]    
        self.points_x = [0]
        self.points_y = [0]  
        self.populate()

    def connect(self):
        # port = '/dev/rfcomm0'
        port = '/dev/rfcomm0'
        if self.ser:
            self.ser.close()
            self.ser = None
            print("Disconnected")
        else:
            self.ser = serial.Serial(port, baudrate=9600)
            print(self.ser.name)
            print(self.ser.is_open)
    
    def send(self):
        self.ser.write('[100;80]'.encode('ascii'))
        #self.ser.write('60'.encode('ascii'))
        #self.ser.write('a'.encode('ascii'))

    def populate(self):
        tk.Button(root, text="Connect", command=self.connect).pack()
        tk.Button(root, text="Send", command=self.send).pack()
        
        self.fig = Figure(figsize=(5, 4), dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.line1, = self.ax.plot(0, 0, 'r-')
        self.line2, = self.ax.plot(0, 0, 'b.-')
        # self.ax.set_xlim([-10000,10000])
        # self.ax.set_ylim([-10000,10000])
        self.ax.set_xlim([0,50000])
        self.ax.set_ylim([-1.1,1.1])
        self.ax.axis('off')
        # plt.tight_layout()
        
        self.canvas = FigureCanvasTkAgg(self.fig, master=root)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
        #w = tk.Canvas(root, width=200, height=200)
        #w.pack()

    def update_plot(self, x, y): #, angle):
        # self.line1.set_xdata([x+155.5634*np.cos((angle)*3.1416/180), 
        #                       x+155.5634*np.cos((angle+45)*3.1416/180), 
        #                       x+155.5634*np.cos((angle+135)*3.1416/180),
        #                       x+155.5634*np.cos((angle-135)*3.1416/180),
        #                       x+155.5634*np.cos((angle-45)*3.1416/180),
        #                       x+155.5634*np.cos((angle)*3.1416/180)])
        # self.line1.set_ydata([y+155.5634*np.sin((angle)*3.1416/180),
        #                       y+155.5634*np.sin((angle+45)*3.1416/180), 
        #                       y+155.5634*np.sin((angle+135)*3.1416/180),
        #                       y+155.5634*np.sin((angle-135)*3.1416/180),
        #                       y+155.5634*np.sin((angle-45)*3.1416/180),
        #                       y+155.5634*np.sin((angle)*3.1416/180)])
        print(self.points_x,self.points_y)
        self.points_x.append(200+x/10)
        self.points_y.append(200+y/10)

        self.line2.set_xdata(self.points_x)
        self.line2.set_ydata(self.points_y)
        # self.line2.set_xdata(200+x/100)
        # self.line2.set_ydata(200+y/100)

        # self.line1.set_xdata([x, x+50*np.cos(angle*3.1416/180)])
        # self.line1.set_ydata([y, y+50*np.sin(angle*3.1416/180)])
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()
    
    def read_serial(self):
        if self.ser:
            data = ""
            # print("Hi")
            while self.ser.inWaiting():
                # print("AA")
                aux = (struct.unpack('c', self.ser.read(1))[0]).decode('ascii')
                if (aux == '('):
                    aux = (struct.unpack('c', self.ser.read(1))[0]).decode('ascii')
                    while(aux != ')'):    
                        data += aux
                        aux = (struct.unpack('c', self.ser.read(1))[0]).decode('ascii')
                    
                    auxlist = data.split(';')
                    # print(auxlist)
                    self.update_plot(float(auxlist[0]), float(auxlist[1])) #, float(auxlist[2]))
            
            
        root.after(40, self.read_serial)


def motion(event):
    gui.mouse_pos = [event.x, event.y]
    if gui.ser:
        gui.ser.write('['.encode('ascii'))
        gui.ser.write(200)
        gui.ser.write(';'.encode('ascii'))
        gui.ser.write(200)
        gui.ser.write(']'.encode('ascii'))
        time.sleep(0.5)


    print('{}'.format(gui.mouse_pos))

def button_press(event):
    pass

def button_release(event):
    gui.ser.write('A'.encode('ascii'))
    gui.ser.write(0)
    gui.ser.write(0)
    pass



# root.bind('<Motion>', motion)
# root.bind('<ButtonPress>', button_press)
# root.bind('<ButtonRelease>', button_release)
gui = GUI()

root.after(40, gui.read_serial)
root.mainloop()

