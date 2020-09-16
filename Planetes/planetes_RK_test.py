#
# TODO:
#   - make RK4 work
#   - use planet radius
#   - use rocket engines
#   - compute simulations first, then animate the calculated paths
#


import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches


class State:
    def __init__(self, x=np.array([0.0, 0.0]), v=np.array([0.0, 0.0])):
        self.x = x
        self.v = v
        

class Derivative:
    def __init__(self, dx=np.array([0.0, 0.0]), dv=np.array([0.0, 0.0])):
        self.dx = dx
        self.dv = dv
    

def squared_distance_func(position1, position2):
    magnitude = np.sum(np.power(position1 - position2, 2))
    angle = np.arctan2(position1[1] - position2[1],  # y
                       position1[0] - position2[0])  # x
    
    return [magnitude, angle]


class Planet:

    # keeps track of all planets
    all_planets = []    
    nplanets = 0

    def __init__(self, name, mass, state):
        self.name = name
        self.mass = mass
        self.state = state
        self.next_state = State()

        Planet.all_planets.append(self)
        Planet.nplanets += 1


    def evaluate(self, initial_state, dt, deriv):
        new_state = State(initial_state.x + deriv.dx * dt, # x
                          initial_state.v + deriv.dv * dt) # v


        #print new_state.x
        output = Derivative(new_state.v,                          # dx
                            self.total_acceleration(new_state))   
                            # dv

        #return output
        return Derivative()

    def integrate(self, state, dt):
        a = self.evaluate(state, 0.0, Derivative())
        b = self.evaluate(state, 0.5*dt, a)
        c = self.evaluate(state, 0.5*dt, b)
        d = self.evaluate(state, dt, c)


        print 'a', a.dx, a.dv
        print 'b', b.dx, b.dv
        print 'c', c.dx, c.dv
        print 'd', d.dx, d.dv


        dxdt = (1.0 / 6.0) * a.dx + 2 * b.dx + 2 * c.dx + d.dx
        dvdt = (1.0 / 6.0) * a.dv + 2 * b.dv + 2 * c.dv + d.dv

        self.next_state.x += dxdt * dt
        self.next_state.v += dvdt * dt


    # calculates acceleration at a given state
    def total_acceleration(self, state):
        """ Total acceleration caused by all other planets on this one """
        acc = np.array([0.0, 0.0])  #(x,y) components

        for planet in Planet.all_planets:
            if planet is not self:
                squared_distance, angle = squared_distance_func(planet.state.x, state.x)

                acc += np.array([np.cos(angle) * G * planet.mass / squared_distance,
                                 np.sin(angle) * G * planet.mass / squared_distance])

        return acc    


    def semi_implicit_euler(self):
        acc = self.total_acceleration(self.state)
        
        new_vel = self.state.v + acc * dt
        new_pos = self.state.x + new_vel * dt

        self.next_state.v = new_vel
        self.next_state.x = new_pos


    # inspired in https://www.thanassis.space/gravity.html
    def update_rk4(self):
        #dt=1
        self.integrate(self.state, dt)
        # Runge-Kutta 4th order
        
        # this should be equivalent to using the derivative functions

        #k1_vel, k1_acc = self.velocity, self.total_acceleration()
        #k2_vel, k2_acc = self.velocity + k1_acc * 0.5 * dt, k1_acc
        #k3_vel, k3_acc = self.velocity + k2_acc * 0.5 * dt, k2_acc
        #k4_vel, k4_acc = self.velocity + k3_acc * dt, k3_acc


class Rocket():
    all_rockets = []    
    nrockets = 0

    def __init__(self, name, mass, state, heading, rotation_speed):
        self.name = name
        self.mass = mass
        self.state = state
        self.next_state = State()
        self.heading = heading
        self.rotation_speed = rotation_speed  # in rad/s anticlockwise
        self.engine_power = 20  #acceleration m/s2
        self.engine_on = False

        Rocket.all_rockets.append(self)
        Rocket.nrockets += 1

    def semi_implicit_euler(self):
        acc = self.total_acceleration(self.state)
        

        new_vel = self.state.v + acc * dt
        new_pos = self.state.x + new_vel * dt

        self.next_state.v = new_vel
        self.next_state.x = new_pos

        self.heading += self.rotation_speed * dt

        return self.next_state



    # calculates acceleration at a given state
    def total_acceleration(self, state):
        """ Total acceleration caused by all other planets on this one """
        acc = np.array([0.0, 0.0])  #(x,y) components

        for planet in Planet.all_planets:
            if planet is not self:
                squared_distance, angle = squared_distance_func(planet.state.x, state.x)

                acc += np.array([np.cos(angle) * G * planet.mass / squared_distance,
                                 np.sin(angle) * G * planet.mass / squared_distance])

        return acc    





def simulate(nsteps):
    rocket = Rocket.all_rockets[0]

    rocket_positions  = np.zeros((nsteps, 2))
    rocket_velocities = np.zeros((nsteps, 2))


     
    for i, step in enumerate(range(nsteps)):
        # update planet position and speed
        new_state = rocket.semi_implicit_euler()
        rocket_positions[i,:] = new_state.x
        rocket_velocities[i,:] = new_state.v
        
        update_rockets_state()

    plt.plot(rocket_positions[:,0], rocket_positions[:,1], 'b.')
    plt.show()
    print rocket_velocities
            

# updates the state of all planets from next_state
def update_planets_state():
    for planet in Planet.all_planets:
        planet.state = planet.next_state
        planet.next_state = State()


# updates the state of all planets from next_state
def update_rockets_state():
    for rocket in Rocket.all_rockets:
        rocket.state = rocket.next_state
        rocket.next_state = State()
        



def press(event):

    if event.key == 'a':
        print 'increasing speed anticlockwise'
        Rocket.all_rockets[0].rotation_speed += 0.0005
    elif event.key == 'd':
        print 'decreasing speed anticlockwise'
        Rocket.all_rockets[0].rotation_speed -= 0.0005

    print 'rotation speed:', Rocket.all_rockets[0].rotation_speed

def animate(dummy):
    # stores the new position of all planets to draw the next frame
    planets_position = np.zeros((Planet.nplanets, 2))
    rockets_position = np.zeros((Rocket.nrockets, 2))
    rockets_velocity = np.zeros((Rocket.nrockets, 2))
    rockets_acceleration = np.zeros((Rocket.nrockets, 2))
    rockets_heading = np.zeros((Rocket.nrockets, 2))
    
    # calculate planets next_state
    for i, planet in enumerate (Planet.all_planets):
        # update planet position and speed
        planet.semi_implicit_euler()
        
        planets_position[i, 0] = planet.next_state.x[1]
        planets_position[i, 1] = planet.next_state.x[0]

    # calculate rockets next_state
    for i, rocket in enumerate (Rocket.all_rockets):
        rocket.semi_implicit_euler()

        rockets_position[i, 0] = rocket.next_state.x[1]
        rockets_position[i, 1] = rocket.next_state.x[0]

        rockets_velocity[i, 0] = rocket.next_state.x[1] + rocket.next_state.v[1] *600
        rockets_velocity[i, 1] = rocket.next_state.x[0] + rocket.next_state.v[0] *600

        acc = rocket.total_acceleration(rocket.state)

        rockets_acceleration[i, 0] = rocket.next_state.x[1] + acc[1] *500000
        rockets_acceleration[i, 1] = rocket.next_state.x[0] + acc[0] *500000

        rockets_heading[i, 0] = rocket.next_state.x[1] + 2000000 * np.sin(rocket.heading)
        rockets_heading[i, 1] = rocket.next_state.x[0] + 2000000 * np.cos(rocket.heading)



    # update planets state from next_state
    update_planets_state()
    update_rockets_state()
        
    plt_planets.set_data(planets_position[:,1].tolist(), planets_position[:,0].tolist())
    plt_rockets.set_data(rockets_position[:,1].tolist(), rockets_position[:,0].tolist())
    plt_velocity.set_data([rockets_position[:,1].tolist(), rockets_velocity[:,1].tolist()], 
                       [rockets_position[:,0].tolist(), rockets_velocity[:,0].tolist()])

    plt_acceleration.set_data([rockets_position[:,1].tolist(), rockets_acceleration[:,1].tolist()], 
                             [rockets_position[:,0].tolist(), rockets_acceleration[:,0].tolist()])

    plt_heading.set_data([rockets_position[:,1].tolist(), rockets_heading[:,1].tolist()], 
                             [rockets_position[:,0].tolist(), rockets_heading[:,0].tolist()])

    plt_planets.set_label('Position: (%f, %f)'%(rockets_position[i,0], rockets_position[i,1]))


    return plt_planets, plt_rockets, plt_velocity, plt_acceleration, plt_heading, 


def initialize_universe():

    Planet('Earth',                     # name
           mass_earth,                  # mass
           State(np.array([0.0, 0.0]),  # position
                 np.array([0.0, 0.0]))) # velocity
  

    # Moon without mass, so it does not affect earth position
    # set initial speed equal to orbital speed for a circular orbit
    #orbital_speed = np.sqrt(G * mass_earth / distance_moon)

    #Planet('Moon', 
    #       1e22,                                 # mass   
    #       State(np.array([distance_moon, 0.0]),   # position
    #             np.array([0.0, orbital_speed])))  # velocity


    #orbital_speed2 = np.sqrt(G * 1e20 / distance_moon*1.1)
    
    #Planet('Moon2', 
    #       1e-100,                                 # mass   
    #       State(np.array([distance_moon*1.01, 0.0]),   # position
    #             np.array([0.0, orbital_speed*0.6])))  # velocity


    orbital_speed = np.sqrt(G * mass_earth / distance_moon)

    Rocket('ApoloXIII', 
           1e-100,                                 # no mass   
           State(np.array([distance_moon, 0.0]),   # position
                 np.array([0.0, orbital_speed*1.07])),  # velocity
           0.0,                                    # heading angle (rad)
           0.0)                                    # rotation speed (rad/s anticlockwise)

    plt.plot(distance_moon, 0.0, 'ro')



def main():
    ani = animation.FuncAnimation(fig, animate, frames=None, interval=0.0, blit=True)
    plt.show()


if __name__ == '__main__':

    # time increment for each estep
    dt = 40

    # constants
    G = 6.673e-11               # m3 kg-1 s-2

    mass_earth = 5.972e24       # kg
    radius_earth = 6371000           # m

    mass_moon = 5e24
    radius_moon = 10    
    distance_moon = 10000000.0

    # plotting stuff
    fig, ax = plt.subplots()
    fig.canvas.mpl_connect('key_press_event', press)
    
    plt_planets, = ax.plot([], [], 'ko')
    plt_rockets, = ax.plot([], [], 'bo')
    plt_velocity, = ax.plot([], [], 'g-')
    plt_acceleration, = ax.plot([], [], 'r-')
    plt_heading, = ax.plot([], [], 'b-')

    axes = plt.gca()
    axes.set_xlim([-20000000.0, 20000000.0]) 
    axes.set_ylim([-20000000.0, 20000000.0])

    # expected circular orbit
    circ1 = plt.Circle((0.0, 0.0), distance_moon, facecolor='none', edgecolor='black', linestyle='--')
    circ2 = plt.Circle((0.0, 0.0), radius_earth, facecolor='black', edgecolor='black', linestyle='-')


    #plt.plot([0.0, 200000.0], [0.0, 200000.0], 'r-')
    ax.add_patch(circ1)
    ax.add_patch(circ2)
    



    initialize_universe()
    
    simulate(400)

    #main()
