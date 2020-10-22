'''
write for paper.
'''
DEBUG_MERGETWO = False

class _entry:
	'''
	v(value): a string of '0'/'1'/'x'
	match_list: a list match rules id
	'''
	def __init__(self, v):
		self.v = v

def intersec(e1, e2):
	v = ""
	assert(len(e1.v) == len(e2.v))
	for i in range(len(e1.v)):
		if e1.v[i]==e2.v[i]:
			v+=e1.v[i]
		elif e1.v[i] == '*':
			v+=e2.v[i]
		elif e2.v[i] == '*':
			v+=e1.v[i]
		else:
			return None #not intersec
	e = _entry(v)
	return e

def CoveredByE(e, E):
	'''time-consuming, for in inner-loop'''
	return False

	for ei in E:
		ie = intersec(ei, e)
		if ie != None and ie.v==e.v:
			return True
	return False

def MergeTwo(E1, E2):
	if DEBUG_MERGETWO:
		print("enter MergeTwo()")
		print("E1 size:", len(E1), "E2 size:", len(E2))
	E=[]
	for i in range(len(E1)):
		for j in range(len(E2)):
			e = intersec(E1[i], E2[j])
			if e == None:
				continue
			elif(not CoveredByE(e, E)):
				e.match_list = E1[i].match_list + E2[j].match_list
				E.append(e)
	for i in range(len(E1)):
		if(not CoveredByE(E1[i], E)):
			E.append(E1[i])
	for i in range(len(E2)):
		if(not CoveredByE(E2[i], E)):
			E.append(E2[i])
	if DEBUG_MERGETWO:
		print("TE size:", len(E))
		print("exit MergeTwo()")
	return E

def sort_size(E_set):
	sorted_E_set = []
	len_E = len(E_set)
	for i in range(len_E):
		maxLen = -1
		maxE = None
		for E in E_set:
			if len(E) > maxLen:
				maxLen = len(E)
				maxE = E
		sorted_E_set.append(maxE)
		E_set.remove(maxE)
	for E in sorted_E_set:
		E_set.append(E)
	return sorted_E_set

def MergeGreedy(E_set, n):
	TE_set = []
	for i in range(n):
		TE_set.append([])
	'''little accelarating trick: sort according the size from big to small'''
	E_set = sort_size(E_set)
	for E in E_set:
		print(len(E))
	for E in E_set:
		'''little accelarating trick: find empty-set directly add'''
		flag = False
		for i in range(len(TE_set)):
			if len(TE_set[i]) == 0:
				TE_set[i] = MergeTwo(E, TE_set[i])
				flag = True
				break
		if flag:
			continue

		min_increment = 0xffffffff #INF
		choose_index = 1
		choose_E = None
		for i in range(len(TE_set)):
			tmp_E = MergeTwo(E, TE_set[i])
			increment = len(tmp_E) - len(E) - len(TE_set[i])
			if increment < min_increment:
				min_increment = increment
				choose_E = tmp_E
				choose_index = i
		print("choose_index:", choose_index)
		print("choose_E size:", len(choose_E))
		TE_set[choose_index] = choose_E
	assert(len(TE_set)==n)
	return TE_set

def main():
	print("hello, TEM!")

if __name__ == '__main__':
	main()