# ENGSCI263: Lab Exercise 2
# lab2.py

# PURPOSE:
# IMPLEMENT a lumped parameter model and CALIBRATE it to data.

# PREPARATION:
# Review the lumped parameter model notes and use provided data from the kettle experiment.

# SUBMISSION:
# - Show your calibrated LPM to the instructor (in Week 3).

# imports
import numpy as np
from matplotlib import pyplot as plt
from numpy.lib.function_base import interp
from scipy.interpolate import interp1d

def ode_model(t, x, q, a, b, x0):
    ''' Return the derivative dx/dt at time, t, for given parameters.

        Parameters:
        -----------
        t : float
            Independent variable.
        x : float
            Dependent variable.
        q : float
            Source/sink rate.
        a : float
            Source/sink strength parameter.
        b : float
            Recharge strength parameter.
        x0 : float
            Ambient value of dependent variable.

        Returns:
        --------
        dxdt : float
            Derivative of dependent variable with respect to independent variable.

        Notes:
        ------
        None

        Examples:
        ---------
        >>> ode_model(0, 1, 2, 3, 4, 5)
        22

    '''
    dxdt = a*q-b*(x-x0)
    return dxdt

def solve_ode(f, t0, t1, dt, x0, pars):
    ''' Solve an ODE numerically.

        Parameters:
        -----------
        f : callable
            Function that returns dxdt given variable and parameter inputs.
        t0 : float
            Initial time of solution.
        t1 : float
            Final time of solution.
        dt : float
            Time step length.
        x0 : float
            Initial value of solution.
        pars : array-like
            List of parameters passed to ODE function f. 

        Returns:
        --------
        t : array-like
            Independent variable solution vector.
        x : array-like
            Dependent variable solution vector.

        Notes:
        ------
        ODE should be solved using the Improved Euler Method. 

        Function q(t) should be hard coded within this method. Create duplicates of 
        solve_ode for models with different q(t).

        Assume that ODE function f takes the following inputs, in order:
            1. independent variable
            2. dependent variable
            3. forcing term, q
            4. all other parameters
    '''

    nt = int(np.ceil(t1-t0)/dt)     # Number of steps
    ts = t0 + np.arange(nt+1)*dt    # Vector of times
    xs = 0.*ts                      # Vector of solutions
    xs[0] = x0
    # Function q(t) - for this, let q(t) = -1
    q = -1

    # Improved euler method steps
    for k in range(nt):
            dxdt = f(ts[k],xs[k],q,*pars)    
            xk1 = xs[k] + dt*dxdt
            dxdt_2 = f(ts[k]+dt,xk1,q,*pars)

            # Corrected estimate with average slope
            xk1_c = xs[k] + dt/2*(dxdt + dxdt_2)
            xs[k+1] = xk1_c
    
    return ts,xs

    

def plot_benchmark():
    ''' Compare analytical and numerical solutions.

        Parameters:
        -----------
        none

        Returns:
        --------
        none

        Notes:
        ------
        This function called within if __name__ == "__main__":

        It should contain commands to obtain analytical and numerical solutions,
        plot these, and either display the plot to the screen or save it to the disk.
        
    '''
    # Analytical Solution (hardcoded) 
    # P = -aq/b (1-exp(-bt))+p0

    # Benchmark, analytic vs numerical with t in (0,10)

    ts = np.linspace(0,10,101)
    ts1,x_numeric = solve_ode(ode_model,0,10,0.1,0,pars = [1,1,0])
    x_analytic = -(1-np.exp(-ts))

    fig,ax = plt.subplots(nrows=1,ncols=3)
    ax[0].plot(ts1,x_numeric, 'b*', markersize = 8)
    ax[0].plot(ts1,x_analytic, 'r-', markersize = 11)
    ax[0].set_title('Analytic vs numerical solution')
    ax[0].set_xlabel('Time')
    ax[0].set_ylabel('X(t)')

    # Benchmark, relative error of analytic - numerical 
    x_error = x_numeric-x_analytic
    x_error = np.abs(x_error/x_analytic)
    ax[1].plot(ts1,x_error,'k-')
    ax[1].set_title('Error analysis')
    ax[1].set_xlabel('Time')

    # Timesetp convergence, at t = 10.
    dt1 = np.linspace(0.01,0.4,41)
    dt_inv = 1/dt1
    x_10 = dt1*0
    for k in range(len(dt1)):
        step = dt1[k]
        ts1,x_numeric = solve_ode(ode_model,0,10,step,x0 = 0,pars = [1,1,0])
        x_10[k]=x_numeric[-1]
    
    ax[2].plot(dt_inv,x_10,'k*')
    ax[2].set_title('Timestep convergence')
    ax[2].set_xlabel('1/dt')  



    plt.show()


def load_kettle_temperatures():
    ''' Returns time and temperature measurements from kettle experiment.

        Parameters:
        -----------
        none

        Returns:
        --------
        t : array-like
            Vector of times (seconds) at which measurements were taken.
        T : array-like
            Vector of Temperature measurements during kettle experiment.

        Notes:
        ------
        It is fine to hard code the file name inside this function.

        Forgotten how to load data from a file? Review datalab under Files/cm/
        engsci233 on the ENGSCI263 Canvas page.
    '''
    data = np.genfromtxt("263_Kettle_Experiment_22-07-19.csv", dtype = (float, float), delimiter = ",",skip_header = 7,autostrip=True )
    t = data[:,0]   # Time
    T = data[:,3]   # Temperature

    return t,T

def solve_kettle_ode(f, t0, t1, dt, x0, pars):
    ''' Solve the kettle ODE numerically.

        Parameters:
        -----------
        f : callable
            Function that returns dxdt given variable and parameter inputs.
        t0 : float
            Initial time of solution.
        t1 : float
            Final time of solution.
        dt : float
            Time step length.
        x0 : float
            Initial value of solution.
        pars : array-like
            List of parameters passed to ODE function f. 

        Returns:
        --------
        t : array-like
            Independent variable solution vector.
        x : array-like
            Dependent variable solution vector.

        Notes:
        ------
        ODE is solved with the improved Euler method. Q is a piecewise function given by interpolate_kettle.

        Assume that ODE function f takes the following inputs, in order:
            1. independent variable
            2. dependent variable
            3. forcing term, q
            4. all other parameters
    '''

    nt = int(np.ceil(t1-t0)/dt)     # Number of steps
    ts = t0 + np.arange(nt+1)*dt    # Vector of times
    xs = 0.*ts                      # Vector of solutions
    xs[0] = x0

    # Function q(t) 
    qt = interpolate_kettle_heatsource(ts)

    # Improved euler method steps
    for k in range(nt):
            q = qt[k]
            dxdt = f(ts[k],xs[k],q,*pars)    
            xk1 = xs[k] + dt*dxdt
            dxdt_2 = f(ts[k]+dt,xk1,q,*pars)

            # Corrected estimate with average slope
            xk1_c = xs[k] + dt/2*(dxdt + dxdt_2)
            xs[k+1] = xk1_c
    
    return ts,xs

def interpolate_kettle_heatsource(t):
    ''' Return heat source parameter q for kettle experiment.

        Parameters:
        -----------
        t : array-like
            Vector of times at which to interpolate the heat source.

        Returns:
        --------
        q : array-like
            Heat source (Watts) interpolated at t.

        Notes:
        ------
        This doesn't *have* to be coded as an interpolation problem, although it 
        will help when it comes time to do your project if you treat it that way. 

        Linear interpolation is fine for this problem, no need to go overboard with 
        splines. 
        
        Forgotten how to interpolate in Python, review sdlab under Files/cm/
        engsci233 on the ENGSCI263 Canvas page.
    '''

    # Getting data
    time,T = load_kettle_temperatures()

    # Hardcoded approximate qsrc from data, using energy = voltage x current
    qsrc = [0,71.9*2.74,71.9*2.74,99.3*3.78,99.3*3.78,0,0]
    tsrc = [0,30,180,210,360,390,1200]

    # Linear interpolate
    f = interp1d(tsrc,qsrc)
    q = f(t)

    return q
        

def plot_kettle_model():
    ''' Plot the kettle LPM over top of the data.

        Parameters:
        -----------
        none

        Returns:
        --------
        none

        Notes:
        ------
        This function called within if __name__ == "__main__":

        It should contain commands to read and plot the experimental data, run and 
        plot the kettle LPM for hard coded parameters, and then either display the 
        plot to the screen or save it to the disk.

    '''

    # Raw data for comparison
    time,Temp = load_kettle_temperatures()
    data = interp1d(time,Temp)

    # Analytic solution
    ts_anal, temp_anal = solve_kettle_ode(ode_model,t0=0,t1=1200,dt=30,x0=22,pars = [1/1900,15/21000,22])
    
    # Plotting
    f,ax = plt.subplots()
    ax.plot(time,Temp,'bo',label = 'Data')
    ax.plot(ts_anal,temp_anal,'k-',label= 'Modeled solution')
    ax.set_title('Kettle temperature v time')
    ax.set_xlabel('Time')
    ax.set_ylabel('Temperature')
    plt.show()
    plt.savefig('kettle_temp.png',dpi=500)


if __name__ == "__main__":
    # plot_benchmark()
    load_kettle_temperatures()
    plot_kettle_model()

    # t = np.linspace(0,1199,1200)
    # y = interpolate_kettle_heatsource(t)
    # f,ax = plt.subplots()
    # ax.plot(t,y,'ko')
    # plt.show()

