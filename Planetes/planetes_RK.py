import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches


class State:
    x = np.zeros((1, 2))  # position
    v = np.zeros((1, 2))  # velocity


class Derivative:
    dx = np.zeros((1, 2))  # dx/dt = velocity
    dv = np.zeros((1, 2))  # dv/dt = acceleration


def evaluate(initial_state, t, dt, deriv):
    new_state = State()
    new_state.x = initial_state.x + deriv.dx * dt
    new_state.v = initial_state.v + deriv.dv * dt

    output = Derivative()
    output.dx = new_state.v
    output.dv = acceleration(state, t+dt)

    return output


def integrate(state, t, dt):
    a = Derivative()
    b = Derivative()
    c = Derivative()
    d = Derivative()

    a = evaluate(state, t, 0.0, Derivative())
    b = evaluate(state, t, 0.5*dt, a)
    c = evaluate(state, t, 0.5*dt, b)
    d = evaluate(state, t, dt, c)



    dxdt = (1.0 / 6.0) * a.dx + 2 * b.dx + 2 * c.dx + d.dx
    dvdt = (1.0 / 6.0) * a.dv + 2 * b.dv + 2 * c.dv + d.dv

    state.x += dxdt * dt
    state.v += dvdt * dt





class Planet:

    # keeps track of all planets
    all_planets = []    
    nplanets = 0

    def __init__(self, name, mass, position, velocity):
        self.name = name
        self.mass = mass
        self.position = position
        self.velocity = velocity

        Planet.all_planets.append(self)
        Planet.nplanets += 1


    def total_acceleration(self):
        """ Total acceleration caused by all other planets on this one """
        acc = np.array([0.0, 0.0])  #(x,y) components

        for planet in Planet.all_planets:
            if planet is not self:
                squared_distance, angle = self.squared_distance_to(planet.position)
                acc += np.array([np.cos(angle) * G * planet.mass / squared_distance,
                                 np.sin(angle) * G * planet.mass / squared_distance])

        return acc


    # calculates the squared distance and the angle to a given position
    def squared_distance_to(self, position):
        magnitude = np.sum(np.power(position - self.position, 2))
        angle = np.arctan2((position - self.position)[1],  # y
                           (position - self.position)[0])  # x
        
        return [magnitude, angle]


    def initial_derivative(self, position, velocity):
        acc = self.total_acceleration()
        return self.velocity, acc


    def next_derivative(self, velocity, acceleration, dt):
        #pos = self.position + velocity * dt
        vel = self.velocity + acceleration * dt
        pos = self.position + vel * dt
        acc = self.total_acceleration()

        return vel, acc

    def update_semi_implicit_euler(self):
        acc = self.total_acceleration()
        
        new_vel = self.velocity + acc * dt
        new_pos = self.position + new_vel * dt

        #print new_pos
        self.velocity = new_vel
        self.position = new_pos

        return new_pos, new_vel


    # inspired in https://www.thanassis.space/gravity.html
    def update_rk4(self):
        # Runge-Kutta 4th order
        
        # this should be equivalent to using the derivative functions

        #k1_vel, k1_acc = self.velocity, self.total_acceleration()
        #k2_vel, k2_acc = self.velocity + k1_acc * 0.5 * dt, k1_acc
        #k3_vel, k3_acc = self.velocity + k2_acc * 0.5 * dt, k2_acc
        #k4_vel, k4_acc = self.velocity + k3_acc * dt, k3_acc


        k1_vel, k1_acc = self.initial_derivative(self.position, self.velocity)
        k2_vel, k2_acc = self.next_derivative(k1_vel, k1_acc, dt*0.5) 
        k3_vel, k3_acc = self.next_derivative(k2_vel, k2_acc, dt*0.5)
        k4_vel, k4_acc = self.next_derivative(k3_vel, k3_acc, dt)

        dxdt = 1.0/6.0 * (k1_vel + 2.0*(k2_vel + k3_vel) + k4_vel)
        dvxdt = 1.0/6.0 * (k1_acc + 2.0*(k2_acc + k3_acc) + k4_acc)

        self.position += dxdt * dt
        self.velocity += dvxdt * dt

        return self.position, self.velocity        
        

def animate(dummy):
    # stores the new position of all planets to draw the next frame
    planet_positions = np.zeros((Planet.nplanets, 2))
    
    # update planets
    for i, planet in enumerate (Planet.all_planets):
        # update planet position and speed
        #planet.update_semi_implicit_euler()
        planet.update_rk4()
        planet_positions[i][0] = planet.position[1]
        planet_positions[i][1] = planet.position[0]

    #print planet_positions
        
    line.set_data(planet_positions[:,1].tolist(), planet_positions[:,0].tolist())

    return line,


def main():
    ani = animation.FuncAnimation(fig, animate, frames=None, interval=1, blit=True)
    plt.show()


if __name__ == '__main__':

    # time increment for each estep
    dt = 0.1

    # constants
    G = 6.673e-11               # m3 kg-1 s-2
    mass_earth = 5.972e24       # kg

    # not real one, just for testing purposes
    distance_moon = 300000.0

    # plotting stuff
    fig, ax = plt.subplots()
    line, = ax.plot([], [], 'ko')
    axes = plt.gca()
    axes.set_xlim([-500000.0, 500000.0]) 
    axes.set_ylim([-500000.0, 500000.0])

    # expected circular orbit
    patch = plt.Circle((0, 0), distance_moon)
    ax.add_patch(patch)

    # Populate universe
    earth = Planet('Earth',                 # name
            mass_earth,                     # mass
            np.array([0.0, 0.0]),           # position
            np.array([0.0, 0.0]))           # velocity
  

    # Moon without mass, so it does not affect earth position
    # set initial speed equal to orbital speed for a circular orbit
    orbital_speed = np.sqrt(G * mass_earth / distance_moon)

    moon = Planet('Moon', 
                  1e-100,                           # no mass         
                  np.array([distance_moon, 0.0]), 
                  np.array([0.0, orbital_speed]))

    

    main()
