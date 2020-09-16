import matplotlib.pyplot as plt
import numpy as np


class PID:

	def __init__(self, p, i, d, integrator_min, integrator_max):
		self.kp = p
		self.ki = i
		self.kd = d
		self.derivator = 0.0
		self.integrator = 0.0
		self.integrator_min = integrator_min
		self.integrator_max = integrator_max

		self.set_point = 0.0
		self.error = 0.0

	def update(self, feedback_value):

		self.error = self.set_point - feedback_value


		self.p_value = self.kp * self.error

		self.d_value = self.kd * (self.error - self.derivator)
		self.derivator = self.error
		
		self.integrator += self.error

		if self.integrator > self.integrator_max:
			self.integrator = self.integrator_max
		elif self.integrator < self.integrator_min:
			self.integrator = self.integrator_min

		self.i_value = self.integrator * self.ki

		#print 'error', self.error
		#print 'p_value', self.p_value


		pid = self.p_value + self.i_value + self.d_value

		return pid


def test_pid(p=0.5, i=0.4, d=0.0):
	pid = PID(p, i, d, -1000, 1000)

	feedback = 0.0

	x = np.arange(1,5,0.1)
	target_list = [0.0]*3
	#target_list.extend([10.0]*50)
	target_list.extend(np.exp(x))

	feedback_list = []

	for i in range(len(target_list)):
		pid.set_point = target_list[i]
		output = pid.update(feedback)
		# heres the plant, converts output (V) into feedback(rpm)
		feedback += 2.0**output - 1
		feedback_list.append(feedback)

		print output, feedback

	plt.plot(feedback_list,'b')
	plt.plot(target_list,'r')
	
	plt.show()




if __name__ == "__main__":
    test_pid()
