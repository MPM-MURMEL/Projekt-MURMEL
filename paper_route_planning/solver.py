import sys
import numpy as np
import math
import random as rd
import csv
import argparse
import matplotlib.pyplot as plt
from pathlib import Path
from enum import Enum

class Debug(Enum):
	OFF = 0
	MIN = 1
	FULL = 2
	
	def __str__(self):
		return self.name
	
class Initial(Enum):
	RANDOM = 0
	NEARESTNEIGHBOR = 1
	
	def __str__(self):
		return self.name
	
class Operator(Enum):
	TWOOPT = 0
	RELOCATE = 1
	TWOOPT_AND_RELOCATE = 2

	def __str__(self):
		return self.name

### part 0: settings
filename = 'thesis1'			# select TSPLIB file
debug_output = Debug.FULL		# amount of debugging output
draw_opt = True			# draw the optimal tour, if file 'filename.opt.tour' exists
output_window_size = 10		# in inches
plot_pause = 0.0001			# visualization delay between plot steps
iterations = 10000			# number of improvement steps
initial_sln = Initial.NEARESTNEIGHBOR	# initial solution method
operator = Operator.TWOOPT		# neighborhood operator
show_node_numbers = False		# visualizes the node numbers, if true
weight_murmel = 0.17			# energy consumption kWh per km (MURMEL)
weight_mothership = 0.27		# energy consumption kWh per km (mothership)
murmel_capacity = 4			# on average, a mothership visit is necessary every 'murmel_capacity's waypoint

# textsizes for plots
fsize=28
params = {'legend.fontsize': 'large',
          'axes.labelsize': fsize,
          'axes.titlesize': fsize,
          'xtick.labelsize': fsize*0.75,
          'ytick.labelsize': fsize*0.75,
          'axes.titlepad': 25}
plt.rcParams.update(params)

# parse command line options
arg_parser = argparse.ArgumentParser(allow_abbrev=False)
arg_parser.add_argument('--file', action='store', type=str, help='Filename of the TSP file without ending.')
arg_parser.add_argument('--debug', type=lambda debug: Debug[debug], choices=list(Debug), help='Defines the amount of debugging messages. Default: ' + str(debug_output))
arg_parser.add_argument('--initial', type=lambda initial: Initial[initial], choices=list(Initial), help='Defines the initial solution strategy. Default: ' + str(initial_sln))
arg_parser.add_argument('--operator', type=lambda operator: Operator[operator], choices=list(Operator), help='Defines the neighborhood operator. Default: ' + str(operator))
arg_parser.add_argument('--iterations', action='store', type=int, help='Defines the number of iterations per simmulated annealing run. Default: ' + str(iterations))
args = arg_parser.parse_args()
if debug_output != Debug.OFF and args.debug != Debug.OFF: print('Argument: debug: ' + str(args.debug))

if args.debug != None:
	debug_output=args.debug
if args.file != None:
	filename=args.file
if args.initial != None:
	initial_sln=args.initial
if args.operator != None:
	operator=args.operator
if args.iterations != None:
	iterations=args.iterations
	
if debug_output != Debug.OFF:
	print('Argument: file: ' + str(args.file))
	print('Argument: initial solution: ' + str(args.initial))
	print('Argument: operator: ' + str(args.operator))
	print('Argument: iterations: ' + str(args.iterations))


### part 1: get waypoints and draw them

if debug_output != Debug.OFF: print('Try to open ' + filename + '.tsp: ', end='')
try:
	with open(filename + '.tsp', 'r') as tsp_file:
		tsp_file_data = tsp_file.readlines()
except Exception as e:
	if debug_output != Debug.OFF: print('error!\nExiting..') # more exception details: str(e)
	sys.exit()
if debug_output != Debug.OFF: print('successful!')

#print(tsp_file_data) # print input file

# possible entries in specification part:
specification_list = ['NAME', 'TYPE', 'COMMENT', 'DIMENSION', 'CAPACITY', 'GRAPH_TYPE', 'EDGE_TYPE', 'EDGE_WEIGHT_TYPE', 'EDGE_WEIGHT_FORMAT', 'EDGE_DATA_FORMAT', 'NODE_TYPE', 'NODE_COORD_TYPE', 'COORD1_OFFSET', 'COORD1_SCALE', 'COORD2_OFFSET', 'COORD2_SCALE', 'COORD3_OFFSET', 'COORD3_SCALE', 'DISPLAY_DATA_TYPE']

specification = [None] * len(specification_list)
node_data = False
nodes = []
for data_line in tsp_file_data:
	data_line = data_line.replace('\n', '')
	#print(data_line)
	
	if node_data:
		node = data_line.split()
		if len(node) == 3:
			try:
				node[0], node[1], node[2] = int(node[0]), float(node[1]), float(node[2])
				nodes.append(node)
			except Exception as e: # not expected data format; try to continue parsing
				node_data = False
		else:
			node_data = False
	
	for i in range(len(specification_list)):
		if data_line.find(specification_list[i] + ': ') == 0:
			specification[i] = data_line.replace(specification_list[i] + ': ', '')
			
	if (data_line.find('NODE_COORD_SECTION') == 0):
		node_data = True

if debug_output == Debug.FULL:
	print('#Debug info: input file specification part:')
	for counter, value in enumerate(specification_list):
	    print ('#   ' + value + ': ' + str(specification[counter]))

if debug_output == Debug.FULL:
	print('#Debug info: input file nodes part:')
	for node in nodes:
	    print ('#   ' + str(node))

nodes = np.array(nodes)[:,[1,2]]

### visualization helper functions
def draw_nodes(nodes):
	ax1.plot(nodes[:,0], nodes[:,1], color='orange', marker='o', linestyle='')
	
	for i in range(len(nodes)):
		if show_node_numbers: ax1.text(nodes[i,0], nodes[i,1], str(i), fontsize=fsize*0.5)
	
def draw_line_c(start, end, color, style):
	return ax1.plot([nodes[start,0], nodes[end,0]],[nodes[start,1], nodes[end,1]], color=color, linestyle=style)
	
def draw_line(start, end):
	return draw_line_c(start, end, 'red', 'solid')
	
murmel_lines = [None] * len(nodes)
ms_lines = [None] * len(nodes)

def remove_line(lines, start, end):
	line = lines[min(start,end)].pop(0)
	line.remove()
	lines[min(start,end)] = None
fig, (ax1, ax2, ax3) = plt.subplots(1, 3)
#fig = plt.gcf()
fig.set_size_inches(output_window_size*3, output_window_size)
opt_tour = None
if draw_opt:
	if debug_output != Debug.OFF: print('Try to open ' + filename + '.opt.tour: ', end='')
	try:
		with open(filename + '.opt.tour', 'r') as tsp_opt_file:
			tsp_opt_file_data = tsp_opt_file.readlines()
			
		if debug_output != Debug.OFF: print('successful!')
		opt_tour = []
		for data_line in tsp_opt_file_data:
			try:
				num = int(data_line)
				if num != -1:
					opt_tour.append(num-1)
				else:
					break
			except Exception as e: # not expected data format; try to continue parsing
				continue
		
		if len(opt_tour) != len(nodes):
			if debug_output != Debug.OFF: print('Optimal solution does not fit to number of nodes. Do not show optimal tour!')
		else:
			if debug_output == Debug.FULL: print('Optimal solution length fit to number of nodes. Optimal tour is: \n' + str(opt_tour))
			for i in range(len(opt_tour)):
				draw_line_c(opt_tour[i-1], opt_tour[i], 'lightgray', 'dotted')
			plt.pause(plot_pause*100)
	except IOError as e:
		if debug_output != Debug.OFF: print('file error! Do not show optimal tour!')

	
draw_nodes(nodes)

### part 2: create cost matrix
def euclidean_2d_distance(i, j):
	xd = nodes[i,0] - nodes[j,0]
	yd = nodes[i,1] - nodes[j,1]
	return int(math.sqrt(xd*xd+yd*yd)+0.5)	


costs = np.zeros((len(nodes), len(nodes)))
for i in range(len(nodes)):
	for j in range(i):
		costs[i,j] = euclidean_2d_distance(i,j)
		costs[j,i] = costs[i,j]

def tour_cost(tour, ms_tour):
	cost = 0
	
	for i in range(len(tour)):
		cost += weight_murmel*costs[tour[i-1], tour[i]]
		
	for i in range(len(ms_tour)):
		cost += weight_mothership*costs[ms_tour[i-1], ms_tour[i]]
		
	return cost

if debug_output != Debug.OFF and opt_tour != None: print('Length of given optimal tour: ' + str(tour_cost(opt_tour, [])))

### part 3: create initial solution and draw it
tour = []

if initial_sln == Initial.RANDOM:
	tour = list(np.random.permutation(len(nodes)))
elif initial_sln == Initial.NEARESTNEIGHBOR:
	current = 0
	tour.append(current)
	while len(tour) < len(nodes):
		last = current
		for i in range(1,len(nodes)):
			if not i in tour:
				#print('i: ' + str(i))
				#print('euclidean_2d_distance(last, i): ' + str(euclidean_2d_distance(last, i)))
				#print('euclidean_2d_distance(last, current): ' + str(euclidean_2d_distance(last, current)) + '\n')
				if last == current:
					current = i
				elif euclidean_2d_distance(last, i) < euclidean_2d_distance(last, current):
					current = i
		tour.append(current)
else: # sorted list
	for i in range(len(nodes)):
		tour.append(i)

ms_tour = tour[::murmel_capacity]
	
if debug_output != Debug.OFF: print('Init tour:' + str(tour) + '\nLength of init tour: ' + str(tour_cost(tour, ms_tour)))

def draw_tour(tour, ms_tour, touched):
	for i in range(len(nodes)):
		# MURMEL
		if murmel_lines[i-1] != None:
			remove_line(murmel_lines, i-1, i)
		if i-1 in touched:
			murmel_lines[i-1] = draw_line_c(tour[i-1], tour[i], 'red', 'solid')
		else:
			murmel_lines[i-1] = draw_line(tour[i-1], tour[i])
			
		# Mothership
		if ms_lines[i-1] != None:
			remove_line(ms_lines, i-1, i)
	for i in range(len(ms_tour)):
		ms_lines[ms_tour[i-1]] = draw_line_c(ms_tour[i-1], ms_tour[i], 'blue', 'solid')

# draw initial tour
draw_tour(tour, ms_tour, [])
plt.pause(plot_pause*100)


# removes edges (a0, a1) and (b0, b1) and adds edges (a0, b0) and (a1, b1). Returns new tour.
def two_opt(a0, b0, tour):
	# get a1 and b1
	a1 = a0+1
	b1 = (b0+1) % len(tour)
	
	if debug_output != Debug.OFF: print('2-Opt Nodes: a0: ' + str(a0) + ' a1: ' + str(a1) + ' b0: ' + str(b0) + ' b1: ' + str(b1))
	
	new_tour = list(tour[0:a1]) + list(reversed(tour[a1:b0+1])) + list(tour[b0+1:len(tour)])
	
	return new_tour, [a0, b0]

def do_two_opt(tour):
	l = len(tour)-1
	b0 = a0 = rd.randint(0, l)
	while (abs(a0 - b0) % l) <= 1:
		b0 = rd.randint(0, l)
	
	return two_opt(min(a0, b0), max(a0, b0), tour)
	
def relocate(a0, b0, tour):
	# get a1, a2 and b1
	a1 = a0+1
	a2 = a0+2
	b1 = (b0+1) % len(tour)
	
	if debug_output != Debug.OFF: print('Relocate Nodes: a0: ' + str(a0) + ' a1: ' + str(a1) + ' a2: ' + str(a2) + ' b0: ' + str(b0) + ' b1: ' + str(b1))
	
	new_tour = list(tour[0:a1]) + list(tour[a2:b0+1]) + list(tour[a1:a1+1]) + list(tour[b0+1:len(tour)])
	
	return new_tour, [a0, a1, b0]
	
# TODO combine do_two_opt and do_relocaate to avoid doublicate code
def do_relocate(tour):
	l = len(tour)-1
	b0 = a0 = rd.randint(0, l)
	while (abs(a0 - b0) % l) <= 2:
		b0 = rd.randint(0, l)
	
	return relocate(min(a0, b0), max(a0, b0), tour)


if opt_tour != None:
	t = np.arange(0, iterations, 1)
	s = np.ones(iterations) * tour_cost(opt_tour, [])


t = np.arange(0, 0, 1)
s = []
probs = []

## simulated annealing parameters
T = Tmax = 100.0
Tmin = 0.1
lambd = math.log(Tmax/Tmin)


graph = ax2.plot(t, s)
graphProb = ax3.plot(probs, s)
cost_current = tour_cost(tour, ms_tour)
ax1.set_title('Current solution')
ax2.set_title('Tour length')
ax3.set_title('Simulated annealing temperature')
ax2.set_xlabel('iteration i')
ax3.set_xlabel('iteration i')
ax2.set_ylabel('f(i)')
ax3.set_ylabel('T(i)')
ax3.set_xlim([0,iterations])
ax3.set_ylim([Tmin,Tmax])

counter_twoopt = counter_relocate = 0

for i in range(iterations):
	did_twoopt = did_relocate = False
	# permutate MURMELs route
	if operator == Operator.TWOOPT:
		new_tour, touched = do_two_opt(tour)
		did_twoopt = True
	elif operator == Operator.RELOCATE:
		new_tour, touched = do_relocate(tour)
		did_relocate = True
	elif operator == Operator.TWOOPT_AND_RELOCATE:
		if rd.random() < 0.5: # 50-50 chance for both
			new_tour, touched = do_relocate(tour)
			did_relocate = True
		else:
			new_tour, touched = do_two_opt(tour)
			did_twoopt = True
	
	
	# derive mothership route
	new_ms_tour = new_tour[::murmel_capacity] # grab every 4th element
	
	# calculate costs
	cost_new = tour_cost(new_tour, new_ms_tour)
	cost_change = cost_new - cost_current
	
	# cool down
	T = Tmax * math.exp(-lambd*i/iterations)
	
	
	if cost_change >= 0:
		prob = math.exp(-cost_change/T)
	else:
		prob = 1.0
	if debug_output == Debug.FULL: print('SA: T: ' + str(round(T,2)) + ', delta cost: ' + str(round(cost_change,2)) + ', acceptance probability: ' + str(round(prob,2)) + ' [iteration: ' + str(i) + ']')
	if prob > rd.random():
		cost_current = cost_new
		if debug_output != Debug.OFF: print('SA: Accepted new solution: Cost: ' + str(cost_new))
		tour = new_tour
		ms_tour = new_ms_tour
		if did_twoopt: counter_twoopt += 1
		if did_relocate: counter_relocate += 1

		# draw changes
		draw_tour(tour, ms_tour, touched)

		plt.pause(plot_pause)
	t = np.arange(0, i+1, 1)
	s.append(cost_current)
	
	probs.append(T)
	graph_plot = graph.pop(0)
	graph_plot.remove()
	graph = ax2.plot(t, s, color='black')
	
	graph_plot = graphProb.pop(0)
	graph_plot.remove()
	graphProb = ax3.plot(t, probs, color='black')
	
	if debug_output != Debug.OFF: print('')
	
	#fig.canvas.draw() TODO remove?
	#fig.canvas.flush_events()
print('Finished! Found tour with length ' + str(cost_current))
if debug_output != Debug.OFF: print('Tour (MURMEL): ' + str(tour))
if debug_output != Debug.OFF: print('Tour (Mothership): ' + str(ms_tour))
if debug_output != Debug.OFF: print('Number of two opt operations: ' + str(counter_twoopt))
if debug_output != Debug.OFF: print('Number of relocate operations: ' + str(counter_relocate))


#plt.show(block=True)
plt.show(block=True)
plt.pause(plot_pause*100)
plt.close()


## write results to file
if debug_output != Debug.OFF: print('Try to write results.csv and results_timeline.csv: ', end='')


header = ['file', 'initial', 'operator', 'iterations', 'length']
my_file = Path('results.csv')
header_needed = False
if not my_file.is_file():
	if debug_output != Debug.OFF: print('Need to add header to results.csv.', end='')
	header_needed = True
data = [filename + '.tsp', str(initial_sln), str(operator), iterations, str(cost_current)]
try:
	with open('results.csv', 'a', newline='\n') as results_file:
		writer = csv.writer(results_file)
		if header_needed:
			writer.writerow(header)
		writer.writerow(data)
	with open('results_timeline.csv', 'w', newline='\n') as results_file:
		writer = csv.writer(results_file)
		writer.writerow(['t', 'f'])
		for i in range(0, len(s)):
			writer.writerow([i, s[i]])
	if debug_output != Debug.OFF: print('successful!')
except Exception as e:
	if debug_output != Debug.OFF: print('error!\nExiting..') # str(e)
	sys.exit()




