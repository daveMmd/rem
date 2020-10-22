'''
process Classbench rules to entries
'''
import copy,random

class Rule:
	pass

def get_ip(s):
	ls = s.split('/')
	assert(len(ls) == 2)
	mask = int(ls[1])
	ls = ls[0].split('.')
	ip = 0
	for i in ls:
		ip = ip*256 + int(i)
	return ip, mask

def get_port(s):
	ls = s.split(' ')
	assert(len(ls)==3)
	return int(ls[0]), int(ls[-1])

def get_protocol(s):
	ls = s.split('/')
	assert(len(ls)==2)
	protocol = int(ls[0][2:], 16)
	mask_value = int(ls[1][2:], 16)
	if mask_value == 255:
		mask = 8
	elif mask_value == 0:
		mask = 0
	else:
		print("unexpected mask_value:", mask_value)
		mask = 8
	return protocol, mask

def get_rule(line):
	ls = line.split('\t')
	assert(len(ls) == 6)
	rule = Rule()
	rule.sip, rule.sip_mask = get_ip(ls[0])
	rule.dip, rule.dip_mask = get_ip(ls[1])
	rule.sport_beg, rule.sport_end = get_port(ls[2])
	rule.dport_beg, rule.dport_end = get_port(ls[3])
	rule.protocol, rule.protocol_mask = get_protocol(ls[4])
	return rule

def get_rules(fname):
	rules = []
	with open(fname) as lines:
		for line in lines:
			line = line.strip()
			if len(line) < 1 or line[0]!='@':
				continue
			line = line[1:] #delete first '@' character
			rule = get_rule(line)
			rules.append(rule)
	return rules

def get_nxt_before(cur, max_length):
	last_length = 1
	for i in range(18):
		length = (1<<i)
		if (cur&(1<<i)) or length>max_length:
			return (cur + last_length -1)
		last_length = length

def get_nxt(cur, max_length):
	last_length = 1
	for i in range(18):
		length = (1<<i)
		if length>max_length:
			return (cur + last_length -1)
		if (cur&(1<<i)):
			return (cur + length -1)
		last_length = length

def rule_prefix_expansion(rule):
	rules1 = []
	#print("original sport range:",rule.sport_beg,"-",rule.sport_end)
	cur = rule.sport_beg
	while cur <= rule.sport_end:
		max_length = rule.sport_end - cur + 1
		nxt = get_nxt(cur, max_length)
		new_rule = copy.copy(rule)
		new_rule.sport_beg = cur
		new_rule.sport_end = nxt
		#print("expansion:",cur,"-",nxt)
		rules1.append(new_rule)
		cur = nxt+1
	#print("original dport range:",rule.dport_beg,"-",rule.dport_end)
	rules2 = []
	cur = rule.dport_beg
	while cur <= rule.dport_end:
		max_length = rule.dport_end - cur + 1
		nxt = get_nxt(cur, max_length)
		new_rule = copy.copy(rule)
		new_rule.dport_beg = cur
		new_rule.dport_end = nxt
		#print("expansion:",cur,"-",nxt)
		rules2.append(new_rule)
		cur = nxt+1
	rules = []
	for r1 in rules1:
		for r2 in rules2:
			new_rule = copy.copy(r1)
			new_rule.dport_beg = r2.dport_beg
			new_rule.dport_end = r2.dport_end
			rules.append(new_rule)
	return rules

def ip_to_value(ip, mask):
	ip_bin = bin(ip)[2:]
	ip_bin = (32-len(ip_bin))*'0' + ip_bin
	for i in range(mask, 32):
		#ip_bin[i] = '*':
		ip_bin = ip_bin[:i] + '*' + ip_bin[i+1:]
	assert(len(ip_bin)==32)
	return ip_bin

def port_to_value(beg, end):
	#print("beg=",beg,"; end=",end)
	#print("bin(",beg,")=",bin(beg))
	#print("bin(",end,")=",bin(end))
	for i in range(18):
		length = (1<<i)
		if beg + length - 1 == end:
			value = bin(beg)[2:]
			value = (16-len(value))*'0' + value
			'''
			for j in range(15, 15-i-1, -1):
				#value[j] = '*'
				value = value[:j] + '*' + value[j+1:]'''
			value = value[:16-i] + '*'*i
			assert(len(value)==16)
			return value

def protocol_to_value(protocol, mask):
	value = bin(protocol)[2:]
	value = (8-len(value))*'0' + value
	for i in range(mask,8):
		#value[i] = '*'
		value = value[:i] + '*' + value[i+1:]
	assert(len(value)==8)
	return value

def rule_to_entry(rule):
	value = ""
	value += ip_to_value(rule.sip, rule.sip_mask)
	value += ip_to_value(rule.dip, rule.dip_mask)
	value += port_to_value(rule.sport_beg, rule.sport_end)
	value += port_to_value(rule.dport_beg, rule.dport_end)
	value += protocol_to_value(rule.protocol, rule.protocol_mask)
	assert(len(value) == 104)
	return value

'''may return multiple entries, for row expansion'''
def get_entry(rule):
	rules = rule_prefix_expansion(rule)
	if len(rules) > 100:
		print("expansion factor > 100:")
		print("rule.sport:", rule.sport_beg,"-",rule.sport_end)
		print("rule.dport:", rule.dport_beg,"-",rule.dport_end)
	entries = []
	for rule in rules:
		e = rule_to_entry(rule)
		entries.append(e)
	return entries

def get_entries(rules):
	entries = []
	for rule in rules:
		entries += get_entry(rule)
	return entries

def split_set(entries, target_size):
	print("in split_set...")
	#target_size = 17
	random.shuffle(entries)
	E_set = []
	total_length = len(entries)
	print("total_length of entries:", total_length)
	total_size = 0
	each_size = int(total_length / target_size)
	for i in range(target_size):
		E_set.append(entries[total_size : total_size+each_size])
		total_size += each_size
	print("exit split_set.")
	return E_set

def writeE(i, E):
	#fname = "./ACL/entrySet_"+str(i)
	#fname = "./FW/entrySet_"+str(i)
	fname = "./IPC/entrySet_"+str(i)
	f = open(fname,"w")
	rule_cnt = 0
	for e in E:
		rule_cnt += 1
		f.write(e)
		f.write(" operator"+str(i)+"-"+str(rule_cnt))
		f.write("\n")
	f.close()

'''
first split rules, then expand to entries.
'''
def main():
	#fname = "./ClassBench/m_acl_5k"
	#fname = "./ClassBench/m_fw_2k"
	fname = "./ClassBench/m_ipc_5k"
	rules = get_rules(fname)
	print("len(rules):",len(rules))
	total_size = 16
	R_set = split_set(rules, total_size)
	E_set = []
	for R in R_set:
		print("len(R):", len(R))
		E_set.append(get_entries(R))
	for i in range(len(E_set)):
		writeE(i+1, E_set[i])


if __name__ == '__main__':
	main()

'''
2.18
FW:
 145, 265, 335, 325, 460, 575, 285, 320, 315, 435, 575, 435, 500, 405, 275, 340
 5990
 106771, 20902, 13302, 6676, 6344, 6198, 6046, 6029, 6014, 6004, 5996, 5994, 5992, 5991, 5990


ACL:
377, 376, 349, 369, 393, 403, 397, 357, 352, 418, 378, 418, 364, 382, 346, 363
6042
174879, 14768, 10555, 6992, 6643, 6407, 6235, 6082, 6071, 6063, 6058, 6054, 6050, 6047, 6044

IPC:
388, 357, 357, 345, 338, 356, 378, 341, 345, 363, 374, 376, 371, 314, 351, 360
5714
444406, 47270, 28416, 12596, 10529, 9237, 7990, 7388, 7101, 6817, 6635, 6442, 6297, 6189, 6104
'''