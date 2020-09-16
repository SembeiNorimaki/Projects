import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def quad_bezier(p0,p1,p2,t):
    return (1-t)**2*p0 + 2*(1-t)*t*p1 + t**2*p2

def cubic_bezier(p0,p1,p2,p3,t):
    return (1-t)**3*p0 + 3*(1-t)**2*t*p1 + 3*(1-t)*t**2*p2 + t**3*p3


def quad_bezier_orbit(i):
    # bezier intermediate point
    p1 = np.array([0.0, p_end[1]+p_end[0]*np.tan(angle)])
    plt.plot([p_ini[0],p1[0],p_end[0]], [p_ini[1],p1[1],p_end[1]],'k:')

    pos = quad_bezier(p_ini,p1,p_end,i/200.0)
    xdata.append(pos[0])
    ydata.append(pos[1])
    
    line.set_data(xdata, ydata)

    return line,


def cubic_bezier_orbit(i):
    p1 = np.array([0.0, r_planet + d1])
    p2 = np.array([p_end[0] - d2 * np.cos(angle),
                   p_end[1] + d2 * np.sin(angle)])

    plt.plot([p_ini[0],p1[0]], [p_ini[1],p1[1]],'b--')
    plt.plot([p2[0],p_end[0]], [p2[1],p_end[1]],'b--')

    pos = cubic_bezier(p_ini,p1,p2,p_end,i/200.0)
    xdata.append(pos[0])
    ydata.append(pos[1])
    
    line.set_data(xdata, ydata)

    return line,




if __name__ == '__main__':

    r_planet = 15.0   # radius of the planet
    r_orbit = 20.0   # radius of the orbit (relative to planet center)
    angle = 40.0 * np.pi / 180.0  # angle of the destination orbit point
   
    d1 = 0.5
    d2 = 0.5

    # initial point
    p_ini = np.array([0.0, r_planet])

    # destination point
    p_end = np.array([r_orbit * np.sin(angle), 
                      r_orbit * np.cos(angle)])
    
    

    
    fig, ax = plt.subplots()
    line, = ax.plot([], [], 'k-')
    plt.plot([p_ini[0], p_end[0]], [p_ini[1], p_end[1]], 'k.')

    circ1 = plt.Circle((0.0, 0.0), r_planet, facecolor='none', edgecolor='black', linestyle='-')
    circ2 = plt.Circle((0.0, 0.0), r_orbit, facecolor='none', edgecolor='red', linestyle=':')

    ax.add_patch(circ1)
    ax.add_patch(circ2)

    axes = plt.gca()
    axes.set_xlim([-22.0, 25.0]) 
    axes.set_ylim([-22.0, 25.0])

    xdata, ydata = [], []

    
    ani = animation.FuncAnimation(fig, cubic_bezier_orbit, np.arange(1, 200), 
                                  interval=25, blit=True, repeat=False)

    #ani.save('bezier_orbit.mp4')
    plt.show()

    #for t in range(0, 1, 0.01):
    #    x = bezier(p0,p1,p2,t)
    #    plt.plot(x[0], x[1],'k.')


