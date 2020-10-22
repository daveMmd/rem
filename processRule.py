'''
process rules
'''
import random

def process_wj():
	rules = []
	with open("./wj/nomask_etc.rules") as lines:
		for line in lines:
			line = line.strip()
			if len(line) < 1:
				continue
			ind = line.find(' ')
			ind = line.find(' ', ind+1)
			rules.append(line[ind+1:])
	print("wj rules len:", len(rules))#6870
	#rules = rules[::-1]
	#random.shuffle(rules) #dramatically increase the intersections

	#approximatly split rules into 17 rule-sets
	#tnums = [253, 272, 254, 270, 381, 330, 352, 357, 351, 407, 458, 360, 359, 371, 399, 411, 485]
	#tnums = [num-200 for num in tnums]
	tnums = [11, 15, 27, 37, 41, 57, 60, 77, 101, 107, 158, 160, 259, 271, 399, 411, 485]
	
	print(sum(tnums))
	print(len(tnums))

	f = open("DC_R.txt",'w')
	cnt=0 #index rules
	for i in range(len(tnums)):
		for j in range(1, tnums[i]+1):
			rule_set = "operator"+str(i+1)
			rid = str(j)
			rule = rule_set+" "+rid+" "+rules[cnt]
			cnt+=1
			f.write(rule+"\n")
	f.close()
	
def main():
	#process_wj()
	#process_classbench_acl()

if __name__ == '__main__':
	main()