'''
tem caller
'''
import tem

def get_DC_protocol(s):
	if s=="*":
		return '*'*8
	protocol = int(s)
	v = ""
	for i in range(8):
		toAnd = 1<<(8-i-1)
		if protocol&toAnd:
			v+="1"
		else:
			v+="0"
	assert(len(v)==8)
	return v

def get_DC_port(s):
	if s=="*":
		return '*'*16
	ls = s.split('/')
	assert(len(ls)==2)
	maskLen = int(ls[1])
	port = int(ls[0])
	portv = ""
	for i in range(maskLen):
		toAnd = 1 << (16-i-1)
		if port&toAnd:
			portv+='1'
		else:
			portv+='0'
	portv = portv + '*'*(16-maskLen)
	assert(len(portv)==16)
	return portv

def get_DC_ip(s):
	if s=="*":
		return '*'*32
	ls = s.split('/')
	assert(len(ls)==2)
	maskLen = int(ls[1])
	ls = ls[0].split('.')
	assert(len(ls) == 4)
	ip = 0
	for i in  ls:
		ip = ip*256 + int(i)
	ipv = ""
	for i in range(maskLen):
		toAnd = 1 << (32-i-1)
		if ip&toAnd:
			ipv+='1'
		else:
			ipv+='0'
	ipv = ipv + '*'*(32-maskLen)
	assert(len(ipv)==32)
	return ipv

def get_DC_e(ls):
	assert(len(ls) == 7)
	v = ""
	v += get_DC_ip(ls[2])
	v += get_DC_ip(ls[3])
	v += get_DC_port(ls[4])
	v += get_DC_port(ls[5])
	v += get_DC_protocol(ls[6])
	e = tem._entry(v)
	e.match_list=[ls[0]+"-"+ls[1]]
	return e

def DC_rules_to_entrys(fname):
	E_set = []
	with open(fname) as lines:
		last_operator = ""
		new_E = []
		for line in lines:
			line = line.strip()
			if len(line) < 1:
				continue
			ls = line.split(' ')
			if last_operator != "" and ls[0]!=last_operator: #fininsh adding one rule-set/operator
				E_set.append(new_E)
				new_E = []
			last_operator = ls[0]
			e = get_DC_e(ls)
			'''print("line:",line)
			print("e.v:",e.v)
			print("e.match_list:",e.match_list)
			input("any key...")'''
			new_E.append(e)
		E_set.append(new_E)
	assert(len(E_set) == 17)
	return E_set

'''
too slow, use c++ implement the final stage.
'''
def test_DC():
	print("enter test_DC()")
	E_set = DC_rules_to_entrys('DC_R.txt')
	print("finish DC_rules_to_entrys()")
	#for i in range(1, len(E_set)):
	for i in range(len(E_set), 0, -1):
		print("##########")
		print("target TCAM entry-set number:", i)
		TE_set = tem.MergeGreedy(E_set, i)
		tsize = 0
		for TE in TE_set:
			tsize += len(TE)
		print("target TCAM size:", tsize)
	print("exit test_DC()")

def get_DC_entry_sets():
	E_set = DC_rules_to_entrys('DC_R.txt')
	cnt = 0
	for E in E_set:
		cnt+=1
		f = open("./DC/entrySet_"+str(cnt), "w")
		for e in E:
			f.write(e.v)
			for ruleid in e.match_list:
				f.write(" ")
				f.write(ruleid)
			f.write('\n')
		f.close()

def main():
	#test_DC()
	get_DC_entry_sets()

if __name__ == '__main__':
	main()