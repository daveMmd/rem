'''
from expect results get rule-set.
expect results: exist 8 pairs of rule-sets whose intersection ratio is lower than 50% and higher than 2%.
'''

import caller, tem
import random

glb_total_num = 985
glb_num = 0
glb_cnt = 0

def write_to_file(E):
	global glb_cnt, glb_num
	glb_num += len(E)
	glb_cnt += 1
	f = open("./DC/entrySet_"+str(glb_cnt), "w")
	rule_cnt = 0
	for e in E:
		rule_cnt += 1
		f.write(e.v)
		f.write(" operator"+str(glb_cnt)+"-"+str(rule_cnt))
		f.write('\n')
	f.close()

def get_rules():
	rules = []
	with open("./wj/nomask_etc2.rules") as lines:
		for line in lines:
			line = line.strip()
			if len(line) < 1:
				continue
			ind = line.find(' ')
			ind = line.find(' ', ind+1)
			rules.append(line[ind+1:])
	print("wj rules len:", len(rules))
	return rules

pair_value = [[(5,10), (1000, 2000)], [(10,20), (700, 1500)], [(15,30), (500, 800)], [(20,50), (300, 500)]]

def get_one_pair(entrys):
	'''前1000次，取最小交'''
	loop_times = 0
	min_ratio = 0xffffffff
	E1_min = None
	E2_min = None
	#while 1:
	while loop_times < 50:
		loop_times += 1
		random.shuffle(entrys)
		if glb_cnt < 0:
			pair = pair_value[int(glb_cnt/2)]
			n1_start = pair[0][0]
			n1_end = pair[0][1]
			n2_start = pair[1][0]
			n2_end = pair[1][1]
			n1 = random.randint(n1_start, n1_end)
			n2 = random.randint(n2_start, n2_end)
		else:
			n1 = random.randint(40,70)
			n2 = random.randint(40,70)
		E1 = entrys[:n1]
		E2 = entrys[n1:n1+n2]
		E = tem.MergeTwo(E1, E2)
		intersections = len(E) - len(E1) - len(E2)
		#ratio = intersections * 1.0 / (len(E1) + len(E2))
		ratio = intersections * 1.0 / glb_total_num
		if loop_times%10==0:
			print("current min_ratio:",min_ratio)
		if ratio < min_ratio:
			min_ratio = ratio
			E1_min = E1
			E2_min = E2
		'''if ratio < 0.2 and ratio > 0.01:
			print("succccccccc!!!!!")
			for e in E1:
				entrys.remove(e)
			for e in E2:
				entrys.remove(e)
			write_to_file(E1)
			write_to_file(E2)
			break'''
	for e in E1_min:
		entrys.remove(e)
	for e in E2_min:
		entrys.remove(e)
	write_to_file(E1)
	write_to_file(E2)
	print("final min_ratio:", min_ratio)
	print("finish one loop!")

def main():
	rules = get_rules()
	entrys = []
	for rule in rules:
		#print("rule:",rule)
		ls = rule.split(' ')
		ls = ["operator", "rid"] + ls
		entrys.append(caller.get_DC_e(ls))
	for i in range(8):
		get_one_pair(entrys)
	#write left one entry-set
	write_to_file(entrys[:glb_total_num - glb_num])

if __name__ == '__main__':
	main()